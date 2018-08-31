/**
 *  Copyright (C) 2017 Xilinx, Inc. All rights reserved.
 *  Author: Sonal Santan
 *  Code copied verbatim from SDAccel xcldma kernel mode driver
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include "mgmt-core.h"
#include "xvc_pcie_ioctl.h"

static int err_info_ioctl(struct xclmgmt_dev *lro, void __user *arg) {

#ifdef AXI_FIREWALL
	struct xclErrorStatus obj;
	u16 i;
	printk(KERN_INFO "%s: %s \n", DRV_NAME, __FUNCTION__);
	obj.mNumFirewalls = 3;
	for(i = 0; i < obj.mNumFirewalls; ++i) {
		obj.mAXIErrorStatus[i].mErrFirewallID = i;
		obj.mAXIErrorStatus[i].mErrFirewallTime = lro->err_firewall_time[i];
		obj.mAXIErrorStatus[i].mErrFirewallStatus = lro->err_firewall_status[i];
	}

	if (copy_to_user(arg, &obj, sizeof(struct xclErrorStatus)))
		return -EFAULT;
#endif
	return 0;
}

static int version_ioctl(struct xclmgmt_dev *lro, void __user *arg)
{
	struct xclmgmt_ioc_info obj;
	printk(KERN_INFO "%s: %s \n", DRV_NAME, __FUNCTION__);
	device_info(lro, &obj);
	if (copy_to_user(arg, &obj, sizeof(struct xclmgmt_ioc_info)))
		return -EFAULT;
	return 0;
}

static long reset_ocl_ioctl(struct xclmgmt_dev *lro)
{
	/* If compute units are not busy then nothing to do */
//	if (!compute_unit_busy(lro))
//		return 0;

	freezeAXIGate(lro);
	freeAXIGate(lro);
	return compute_unit_busy(lro) ? -EBUSY : 0;
}



long mgmt_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct xclmgmt_char *lro_char = (struct xclmgmt_char *)filp->private_data;
	struct xclmgmt_dev *lro;
	long result = 0;
	BUG_ON(!lro_char);
	lro = lro_char->lro;
	BUG_ON(!lro);

	//printk(KERN_DEBUG "MGMT IOCTL request %u\n", cmd & 0xff);
    //printk(KERN_INFO "%s %s PCI Slot: %d \n", DRV_NAME, __FUNCTION__, PCI_SLOT(lro->pci_dev->devfn));

	if (lro_char != lro->user_char_dev)
		return -ENOTTY;

	if ((_IOC_TYPE(cmd) != XCLMGMT_IOC_MAGIC) && (_IOC_TYPE(cmd) != _IOC_TYPE(XDMA_IOCXVC)))
		return -ENOTTY;

	if (_IOC_DIR(cmd) & _IOC_READ)
		result = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		result =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));

	if (result)
		return -EFAULT;

#ifdef AXI_FIREWALL
	mutex_lock(&lro->proc_mutex);
	if (xclmgmt_check_proc(lro, task_tgid(current))) {
		mutex_unlock(&lro->proc_mutex);
		return -EFAULT;
	}
	mutex_unlock(&lro->proc_mutex);
#endif

	mutex_lock(&lro->busy_mutex);
	if (lro->reset_firewall) {
		mgmt_err(lro, "Firewall tripped!");
		mutex_unlock(&lro->busy_mutex);
		return -EBUSY;
	}

	switch (cmd) {
	case XCLMGMT_IOCINFO:
		result = version_ioctl(lro, (void __user *)arg);
		break;
	case XCLMGMT_IOCICAPDOWNLOAD:
		result = bitstream_ioctl(lro, (void __user *)arg);
		break;
	case XCLMGMT_IOCICAPDOWNLOAD_AXLF:
		result = bitstream_ioctl_axlf(lro, (void __user *)arg);
		break;
	case XCLMGMT_IOCOCLRESET:
		result = reset_ocl_ioctl(lro);
		break;
	case XCLMGMT_IOCHOTRESET:
		result = reset_hot_ioctl(lro);
		break;
	case XCLMGMT_IOCFREQSCALE:
		result = ocl_freqscaling_ioctl(lro, (void __user *)arg);
		break;
	case XCLMGMT_IOCREBOOT:
		result = capable(CAP_SYS_ADMIN) ? pci_fundamental_reset(lro) : -EACCES;
		break;
	case XCLMGMT_IOCERRINFO:
		result = err_info_ioctl(lro, (void __user *)arg);
		break;
	case XDMA_IOCXVC:
		result = xvc_ioctl(lro, (void __user *)arg);
		break;
	default:
		printk(KERN_DEBUG "MGMT default IOCTL request %u\n", cmd & 0xff);
		result = -ENOTTY;
	}

	mutex_unlock(&lro->busy_mutex);
	return result;
}

