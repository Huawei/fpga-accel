/**
 *  Copyright (C) 2017 Xilinx, Inc. All rights reserved.
 *
 *  Thread to check sysmon/firewall status for errors/issues
 *  Author: Umang Parekh
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

#include <linux/kthread.h>
#include "mgmt-core.h"

#define LOW_TEMP 	0
#define HI_TEMP 	85
#define LOW_VOLT 	500
#define HI_VOLT 	2500


extern int health_interval;
extern int health_check;

// Commercial Temperature range
void check_temp_within_range(int16_t temp) {
	if(temp < LOW_TEMP || temp > HI_TEMP) {
		printk(KERN_ERR "%s: Temperature outside normal range (%d-%d) %d. \n", DRV_NAME,
							       LOW_TEMP, HI_TEMP, temp);
	}
}

void check_volt_within_range(unsigned short volt) {
	if(volt < LOW_VOLT || volt > HI_VOLT) {
		printk(KERN_ERR "%s: Voltage outside normal range (%d-%d)mV %d. \n", DRV_NAME,
		       LOW_VOLT, HI_VOLT, volt);
	}
}


static void check_sysmon(const struct xclmgmt_dev *lro) {
	u32 val;
//	printk(KERN_DEBUG "%s: check_sysmon. \n", DRV_NAME);
	val = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + SYSMON_BASE + SYSMON_TEMP);
	check_temp_within_range(onchip_temp(val));
	val = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + SYSMON_BASE + SYSMON_VCCINT);
	check_volt_within_range(to_volt(val));
	val = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + SYSMON_BASE + SYSMON_VCCAUX);
	check_volt_within_range(to_volt(val));
	val = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + SYSMON_BASE + SYSMON_VCCBRAM);
	check_volt_within_range(to_volt(val));
}

int health_thread(void *data) {
	struct xclmgmt_dev *lro = (struct xclmgmt_dev *)data;
	while (!kthread_should_stop()) {
		ssleep(health_interval);
		if (health_check == 1) {
			mutex_lock(&lro->busy_mutex);
			if (!lro->reset_firewall) {
				check_sysmon(lro);
#ifdef AXI_FIREWALL
				check_axi_firewall(lro);
			} else {
				check_and_reset_firewall(lro, false);
			}
#endif
			mutex_unlock(&lro->busy_mutex);
		}
	}
	printk(KERN_INFO "The health thread has terminated.");
	return 0;
}

void init_health_thread(struct xclmgmt_dev *lro) {
	lro->kthread = kthread_run(health_thread, (void *) lro, "mgmt-thread");
	printk(KERN_INFO "init_health_thread.");
	if(IS_ERR(lro->kthread)) {
		printk(KERN_INFO "ERROR! mgmt lro->kthread init");
	}
}

void fini_health_thread(const struct xclmgmt_dev *lro) {
	int ret = kthread_stop(lro->kthread);
	printk(KERN_INFO "fini_health_thread. ret = %d\n", ret);
	if(ret != EINTR) {
		printk(KERN_INFO "fini_health_thread: The health thread has terminated.");
	}
}


