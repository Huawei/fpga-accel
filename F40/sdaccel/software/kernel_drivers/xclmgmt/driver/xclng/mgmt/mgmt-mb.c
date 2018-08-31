/*
 * sysfs for the device attributes.
 *
 * Copyright (C) 2016-2017 Xilinx, Inc. All rights reserved.
 *
 * Authors:
 *    Lizhi Hou <lizhih@xilinx.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/vmalloc.h>
#include "mgmt-core.h"
#include <linux/module.h>

#define	MGMT_MB_MAX_RETRY	50
#define MGMT_MB_RETRY_INTERVAL	100       //ms

int mgmt_stop_mb(struct xclmgmt_dev *lro)
{
	int	ret = 0;
	int	retry = 0;
	u32	reg_val = 0;

	if (!lro->mb_on) {
		return 0;
	}

	reg_val = MGMT_READ_REG32(lro, MB_GPIO);
	mgmt_info(lro, "Reset GPIO 0x%x", reg_val);
	if (reg_val == MB_GPIO_RESET) {
		/* MB in reset status */
		return 0;
	}

	mgmt_info(lro, "MGMT Image magic word, 0x%x, status 0x%x, id 0x%x",
		MGMT_READ_REG32(lro, MB_IMAGE_MGMT),
		MGMT_READ_REG32(lro,MB_REG_STATUS),
		MGMT_READ_REG32(lro, MB_REG_ID));

	if (!MB_SELF_JUMP(MGMT_READ_REG32(lro, MB_IMAGE_MGMT))) {
		/* non cold boot */
		reg_val = MGMT_READ_REG32(lro,MB_REG_STATUS);
		if (!(reg_val & MB_STATUS_MASK_STOPPED)) {
			// need to stop microblaze
			mgmt_info(lro, "stopping microblaze...");
			MGMT_WRITE_REG32(lro, MB_REG_CTL, MB_CTL_MASK_STOP);
			MGMT_WRITE_REG32(lro, MB_REG_STOP_CONFIRM, 1);
			while (retry++ < MGMT_MB_MAX_RETRY &&
				!(MGMT_READ_REG32(lro, MB_REG_STATUS) &
				MB_STATUS_MASK_STOPPED)) {
				msleep(MGMT_MB_RETRY_INTERVAL);
			}
			if (retry >= MGMT_MB_MAX_RETRY) {
				mgmt_err(lro, "Failed to stop microblaze\n");
				mgmt_err(lro, "Error Reg 0x%x\n",
					 MGMT_READ_REG32(lro, MB_REG_ERR));
				ret = -EIO;
				goto failed;
			}
		}
		mgmt_info(lro, "Microblaze Stopped, retry %d\n", retry);
	}

	/* hold reset */
	MGMT_WRITE_REG32(lro, MB_GPIO, MB_GPIO_RESET);
failed:

	return ret;
}

int mgmt_start_mb(struct xclmgmt_dev *lro)
{
	int retry = 0;
	u32 reg_val = 0;

	if (!lro->mb_on) {
		return 0;
	}

	reg_val = MGMT_READ_REG32(lro, MB_GPIO);
	mgmt_info(lro, "Reset GPIO 0x%x", reg_val);
	if (reg_val == MB_GPIO_ENABLED) {
		return 0;
	}

	mgmt_info(lro, "Start Microblaze...");
	mgmt_info(lro, "MGMT Image magic word, 0x%x",
		MGMT_READ_REG32(lro, MB_IMAGE_MGMT));

	if (lro->header.FeatureBitMap & BOARD_MGMT_ENBLD) {
		mgmt_info(lro, "Copying mgmt image len %d\n",
		    lro->mb_stash.mgmt_binary_length);
		MGMT_TOIO(lro, MB_IMAGE_MGMT, lro->mb_stash.mgmt_binary,
		    lro->mb_stash.mgmt_binary_length);
	}
	/*
	 * Loading sche on kcu1500_5.0 causes MB crash. Thus checking DSA ver
	 * as a workaround
	 * DSA ver > 50
	 */
	if ((get_dsa_version(lro) >= 0x51) &&
	    lro->header.FeatureBitMap & MB_SCHEDULER) {
		mgmt_info(lro, "Copying scheduler image len %d\n",
		    lro->mb_stash.sche_binary_length);
		MGMT_TOIO(lro, MB_IMAGE_SCHE, lro->mb_stash.sche_binary,
		    lro->mb_stash.sche_binary_length);
	}

	MGMT_WRITE_REG32(lro, MB_GPIO, MB_GPIO_ENABLED);


	mgmt_info(lro, "MGMT Image magic word, 0x%x, status 0x%x, id 0x%x",
		MGMT_READ_REG32(lro, MB_IMAGE_MGMT),
		MGMT_READ_REG32(lro,MB_REG_STATUS),
		MGMT_READ_REG32(lro, MB_REG_ID));

	do {
		msleep(MGMT_MB_RETRY_INTERVAL);
	} while (retry++ < MGMT_MB_MAX_RETRY && (MGMT_READ_REG32(lro,
	    MB_REG_STATUS) & MB_STATUS_MASK_STOPPED));

	//Extra pulse needed as workaround for axi interconnect issue in DSA
	if (retry >= MGMT_MB_MAX_RETRY) {
		retry = 0;
		MGMT_WRITE_REG32(lro, MB_GPIO, MB_GPIO_RESET);
		MGMT_WRITE_REG32(lro, MB_GPIO, MB_GPIO_ENABLED);

		do {
			msleep(MGMT_MB_RETRY_INTERVAL);
		} while (retry++ < MGMT_MB_MAX_RETRY && (MGMT_READ_REG32(lro,
		    MB_REG_STATUS) & MB_STATUS_MASK_STOPPED));
	}

	if (retry >= MGMT_MB_MAX_RETRY) {
		mgmt_err(lro, "Failed to start microblaze\n");
		mgmt_err(lro, "Error Reg 0x%x\n",
			MGMT_READ_REG32(lro, MB_REG_ERR));
		return -EIO;
	}

	return 0;
}

void mgmt_fini_mb(struct xclmgmt_dev *lro)
{
	/* allocated in load_boot_firmware() */
	vfree(lro->mb_stash.mgmt_binary);
	vfree(lro->mb_stash.sche_binary);

	lro->mb_stash.mgmt_binary = NULL;
	lro->mb_stash.sche_binary = NULL;
	lro->mb_stash.mgmt_binary_length = 0;
	lro->mb_stash.sche_binary_length = 0;

	/*
	 * It is more secure that MB keeps running even driver is unloaded.
	 * Even user unload our driver and use their own stuff, MB will still
	 * be able to monitor the board unless user stops it explicitly
	 */
	/* stop mb for now to avoid unexpected behavior across warm boot
	 * I would remove this if pci_reset recovers mb
	 */
	mgmt_stop_mb(lro);
}

int mgmt_init_mb(struct xclmgmt_dev *lro)
{
	u32	mb_id;
	int	ret;

	if ((lro->header.FeatureBitMap & BOARD_MGMT_ENBLD) ||
	    (lro->header.FeatureBitMap & MB_SCHEDULER)) {
		mgmt_info(lro, "Microblaze is supported.");
		lro->mb_on = true;
	} else {
		mgmt_info(lro, "Microblaze is not supported.");
		return 0;
	}

	ret = mgmt_stop_mb(lro);
	if (ret) {
		lro->mb_on = false;
		return ret;
	}

	ret = mgmt_start_mb(lro);
	if(ret) { 
		lro->mb_on = false;
		return ret;
	}
	mb_id = MGMT_READ_REG32(lro, MB_REG_ID);
	lro->mb_cap = MGMT_READ_REG32(lro, MB_REG_CAP);
	mgmt_info(lro, "Microblaze Ver: %x ID: %x CAP: %x",
		MGMT_READ_REG32(lro, MB_REG_VERSION), mb_id, lro->mb_cap);

	return 0;
}
