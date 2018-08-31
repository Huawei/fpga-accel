/**
 *  Copyright (C) 2017 Xilinx, Inc. All rights reserved.
 *
 *  Utility Functions for AXI firewall IP.
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

#include "mgmt-core.h"

#ifdef AXI_FIREWALL

// Firewall error bits
#define READ_RESPONSE_BUSY                        BIT(0)
#define RECS_ARREADY_MAX_WAIT                     BIT(1)
#define RECS_CONTINUOUS_RTRANSFERS_MAX_WAIT       BIT(2)
#define ERRS_RDATA_NUM                            BIT(3)
#define ERRS_RID                                  BIT(4)
#define WRITE_RESPONSE_BUSY                       BIT(16)
#define RECS_AWREADY_MAX_WAIT                     BIT(17)
#define RECS_WREADY_MAX_WAIT                      BIT(18)
#define RECS_WRITE_TO_BVALID_MAX_WAIT             BIT(19)
#define ERRS_BRESP                                BIT(20)

// Get the timezone info from the linux kernel
extern struct timezone sys_tz;

const static unsigned firewall_baseaddr[XCLMGMT_NUM_FIREWALL_IPS] = {
	FIREWALL_MGMT_CONTROL_BASE,
	FIREWALL_USER_CONTROL_BASE,
	FIREWALL_DATAPATH_BASE
};

#define	FIREWALL_STATUS_BUSY	(READ_RESPONSE_BUSY | WRITE_RESPONSE_BUSY)

#define	READ_STATUS(lro, id)	MGMT_READ_REG32(lro, firewall_baseaddr[id] + \
	AF_MI_FAULT_STATUS_OFFSET)
#define	WRITE_CTRL(lro, id, val) MGMT_WRITE_REG32(lro, firewall_baseaddr[id] + \
	AF_UNBLOCK_CTRL_OFFSET, val)

#define	IS_FIRED(lro, id) (READ_STATUS(lro, id) & ~FIREWALL_STATUS_BUSY)

static void clear_axi_firewall_err(struct xclmgmt_dev *lro, u8 firewall_id)
{
#define MAX_RETRY_COUNT         20
#define RETRY_INTERVAL          100
	u32 val;
	u32 count = 0;

	val = READ_STATUS(lro, firewall_id);
	if(val == 0) {
		mgmt_info(lro, "nothing to be cleared");

		return; // Nothing to be cleared
	}

	while(val & FIREWALL_STATUS_BUSY)
	{
		if(count++ > MAX_RETRY_COUNT) {
			break;
		}

		msleep(RETRY_INTERVAL);
		val = READ_STATUS(lro, firewall_id);
	}

	if (val & FIREWALL_STATUS_BUSY) {
		mgmt_info(lro, "read/write response busy id %d: 0x%x",
			firewall_id, val);
	} else if(val == 0) {
		mgmt_info(lro, "nothing to be cleared");
		return; // Nothing to be cleared
	}

	WRITE_CTRL(lro, firewall_id, 1);
}

static void reset_pcie_and_firewall(struct xclmgmt_dev *lro)
{
	unsigned i;
	u32 val, retry = 0;
	bool reset = false;

	if (lro->user_pci_dev) {
		xdma_reset(lro->user_pci_dev, true);
		xocl_reset(lro->user_pci_dev, true);
	}

#define	MAX_RETRY	4
	/* try manually clear first  */
	while (retry++ < MAX_RETRY) {
		reset = false;
		for(i = 0; i < XCLMGMT_NUM_FIREWALL_IPS; ++i) {
			val = READ_STATUS(lro, i);
			if (val != 0) {
				clear_axi_firewall_err(lro, i);
			}
			msleep(2);
			val = READ_STATUS(lro, i);
			if (val != 0) {
				reset = true;
				break;
			}
		}
		if (!reset) {
			mgmt_info(lro, "Manually reset firewall success");
			goto skip_pci_reset;
		}
	}
		
	if(get_dsa_version(lro) >= 0x51) {
		/* PCIe hot reset, firewall status will be cleared. */
		mgmt_info(lro, "Issuing pcie hot reset.");
		xclmgmt_reset_pci(lro);
	}

	retry = 0;
	while (retry++ < MAX_RETRY) {
		reset = false;
		for(i = 0; i < XCLMGMT_NUM_FIREWALL_IPS; ++i) {
			val = READ_STATUS(lro, i);
			if (val != 0) {
				reset = true;
				break;
			}
		}
		if (!reset)
			break;
	}

skip_pci_reset:
	lro->reset_firewall = reset;

	freezeAXIGate(lro);
	msleep(500);
	freeAXIGate(lro);
	msleep(500);

	if (lro->user_pci_dev) {
		xdma_reset(lro->user_pci_dev, false);
		xocl_reset(lro->user_pci_dev, false);
	}

}

bool check_and_reset_firewall(struct xclmgmt_dev *lro, bool req_reset)
{
	struct timeval time;
	bool reset = false;
	unsigned i;
	u32 val;

	for(i = 0; i < XCLMGMT_NUM_FIREWALL_IPS; ++i) {
		val = IS_FIRED(lro, i);
		if (val) {
			mgmt_info(lro, "AXI Firewall %d tripped.Status: 0x%x",
				i, val);
			lro->err_firewall_status[i] = val;
			/* handle time zones */
			do_gettimeofday(&time);
			lro->err_firewall_time[i] = (u64)(time.tv_sec -
				(sys_tz.tz_minuteswest * 60));
			reset = true;
			break;
		}
	}

	lro->reset_firewall = reset;

	// Issue the hot reset only once.
	if(req_reset && reset) {
		reset_pcie_and_firewall(lro);
	}

	return reset;
}

// Finds a given pid in the linked list (if available)
static struct proc_list *xclmgmt_find_by_pid(struct xclmgmt_dev *lro, struct pid *find_pid)
{
	struct proc_list *proc_list, *tmp;

	printk(KERN_INFO "%s: %s \n", DRV_NAME, __FUNCTION__);
	list_for_each_entry_safe(proc_list, tmp, &(lro->proc_list).head, head) {
		printk(KERN_INFO "%s: pid: %d \n", DRV_NAME, pid_nr(proc_list->pid));
		if (find_pid == proc_list->pid) {
			return proc_list;
		}
	}
	return NULL;
}

// Add a node to the list
void xclmgmt_list_add(struct xclmgmt_dev *lro, struct pid *new_pid)
{
	struct proc_list *new_proc = kmalloc(sizeof(struct proc_list), GFP_KERNEL);
	printk(KERN_INFO "%s: %s \n", DRV_NAME, __FUNCTION__);
	new_proc->pid = new_pid;
	new_proc->signaled = false;
	list_add_tail(&new_proc->head, &(lro->proc_list).head);
}

// remove a node from the list
void xclmgmt_list_remove(struct xclmgmt_dev *lro, struct pid *remove_pid)
{
	struct proc_list *found_prod = NULL;
	printk(KERN_INFO "%s: %s \n", DRV_NAME, __FUNCTION__);

	found_prod = xclmgmt_find_by_pid(lro, remove_pid);
	if(found_prod != NULL) {
		printk(KERN_INFO "%s: pid found \n", DRV_NAME);
		list_del(&found_prod->head);
		kfree(found_prod);
	} else
		printk(KERN_INFO "%s: pid not found \n", DRV_NAME);
}

/* 
 * Expect char close will be called and all the processes will be
 * deleted individually from the list.
 * before that check if proc is signaled (killed) before proceeding ioctls.
 */
bool xclmgmt_check_proc(struct xclmgmt_dev *lro, struct pid *pid)
{
	struct proc_list *found_prod = NULL;

	found_prod = xclmgmt_find_by_pid(lro, pid);
	if (found_prod) {
		return	found_prod->signaled;
	}

	mgmt_err(lro, "pid %d not found", pid_nr(pid));

	/* should not happen, but return signaled anyway */
	return true;
}

// Issue a sigbus to all the processes in the list
void xclmgmt_killall_processes(struct xclmgmt_dev *lro)
{
	struct proc_list *proc_list, *tmp;
	int ret;

	mgmt_info(lro, "kill all processes.");

	mutex_lock(&lro->proc_mutex);

	list_for_each_entry_safe(proc_list, tmp, &(lro->proc_list).head, head) {
		printk(KERN_INFO "%s: Killing pid: %d \n", DRV_NAME, pid_nr(proc_list->pid));
		ret = kill_pid(proc_list->pid, SIGBUS, 1);
		if(ret != 0)
			mgmt_err(lro, "Killing pid: %d unsuccessful. Err: %d\n",
				pid_nr(proc_list->pid), ret);
		/* mark pid */
		proc_list->signaled = true;
	}

	mutex_unlock(&lro->proc_mutex);
}

/*
 * Delete the entire list. This is just for backup purposes. Ideally the list should be empty
 * because we are assuming that the char close will always be called and all the processes will be
 * deleted individually from the list.
 */
void xclmgmt_list_del(struct xclmgmt_dev *lro) {

	struct proc_list *proc_list, *tmp;
	printk(KERN_INFO "%s: %s \n", DRV_NAME, __FUNCTION__);

	list_for_each_entry_safe(proc_list, tmp, &(lro->proc_list).head, head) {
		printk(KERN_INFO "%s: %s %d \n", DRV_NAME, __FUNCTION__, __LINE__);
		list_del(&proc_list->head);
		kfree(proc_list);
	}

}

bool check_axi_firewall(struct xclmgmt_dev *lro) {
	if (check_and_reset_firewall(lro, false)) {
		/*
		 * Kill all the processes currently attached to
		 * xclmgmt: throw a sigbus
		 */
		xclmgmt_killall_processes(lro);

		reset_pcie_and_firewall(lro);
	}
	return true;
}

void init_firewall(struct xclmgmt_dev *lro) {
	INIT_LIST_HEAD(&((lro->proc_list).head));
	printk(KERN_INFO "%s: %s \n", DRV_NAME, __FUNCTION__);

	mutex_init(&lro->proc_mutex);
}
#endif // AXI Firewall
