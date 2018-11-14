/**
 * Copyright (C) 2017 Xilinx, Inc.
 *
 * Author(s):
 * Sonal Santan <sonal.santan@xilinx.com>
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

#ifndef _XCL_MGT_PF_H_
#define _XCL_MGT_PF_H_

#include <linux/cdev.h>
#include <linux/list.h>
#include <linux/signal.h>
#include <linux/init_task.h>
#include <linux/mutex.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/types.h>
#include <asm/io.h>
#include "mgmt-reg.h"
#include "mgmt-ioctl.h"
#include "xclfeatures.h"


#define XCLMGMT_DRIVER_MAJOR 2017
#define XCLMGMT_DRIVER_MINOR 4
#define XCLMGMT_DRIVER_PATCHLEVEL 6

#define XCLMGMT_MINOR_BASE (0)
#define XCLMGMT_MINOR_COUNT (16)

#define DRV_NAME "xclmgmt"

#define	MGMT_READ_REG32(lro, off)	\
	ioread32(lro->bar[XCLMGMT_MAIN_BAR] + off)
#define	MGMT_WRITE_REG32(lro, off, val)	\
	iowrite32(val, lro->bar[XCLMGMT_MAIN_BAR] + off)
#define	MGMT_TOIO(lro, off, addr, len)	\
	memcpy_toio(lro->bar[XCLMGMT_MAIN_BAR] + off, addr, len)

#define	mgmt_err(lro, fmt, args...)	\
	dev_err(&lro->pci_dev->dev, "%s: "fmt, __func__, ##args)
#define	mgmt_info(lro, fmt, args...)	\
	dev_info(&lro->pci_dev->dev, "%s: "fmt, __func__, ##args)

struct xclmgmt_ioc_info;

struct xclmgmt_bitstream_container {
	/* MAGIC_BITSTREAM == 0xBBBBBBBBUL */
	unsigned long magic;
	char *clear_bitstream;
	u32 clear_bitstream_length;
};

struct xclmgmt_microblaze_container {
	char *sche_binary;
	u32 sche_binary_length;
	char *mgmt_binary;
	u32 mgmt_binary_length;
};

// List of processes that are using the mgmt driver
// also saving the task
struct proc_list {

	struct list_head head;
	struct pid 	 *pid;
	bool		signaled;
};

struct power_val {
	s32 max;
	s32 avg;
	s32 curr;
};

struct mgmt_power {
	struct power_val vccint;
	struct power_val vcc1v8;
	struct power_val vcc1v2;
	struct power_val vccbram;
	struct power_val mgtavcc;
	struct power_val mgtavtt;
};

struct xclmgmt_dev {
	/* MAGIC_DEVICE == 0xAAAAAAAA */
	unsigned long magic;
	/* The feature Rom header */
	struct FeatureRomHeader header;
	/* the kernel pci device data structure provided by probe() */
	struct pci_dev *pci_dev;
	struct pci_dev *user_pci_dev;
	int instance;
	void *__iomem bar[XCLMGMT_MAX_BAR];
	resource_size_t bar_map_size[XCLMGMT_MAX_BAR];
	struct xclmgmt_char *user_char_dev;
	struct xclmgmt_bitstream_container stash;
	struct xclmgmt_microblaze_container mb_stash;
	int axi_gate_frozen;
	u64 feature_id;
	unsigned short ocl_frequency[4];
	u64 unique_id_last_bitstream;
#ifdef AXI_FIREWALL
	u32 err_firewall_status[XCLMGMT_NUM_FIREWALL_IPS];
	u64 err_firewall_time[XCLMGMT_NUM_FIREWALL_IPS];
	struct proc_list proc_list;
	struct mutex proc_mutex;
#endif
	struct task_struct *kthread;
	struct mutex busy_mutex;
	bool reset_firewall;
	struct mgmt_power power;
//	struct mutex ioctl_mutex;

	bool mb_on;
	u32 mb_cap;
	struct device *mb_hwmon_dev;
	struct device *sysmon_hwmon_dev;

	struct xiic_i2c *i2c;
	struct i2c_client *lm63;
};

struct xclmgmt_char {
	struct xclmgmt_dev *lro;
	struct cdev cdev;
	struct device *sys_device;
	int bar;
};

struct xclmgmt_ocl_clockwiz {
	/* target frequency */
	unsigned short ocl;
	/* config0 register */
	unsigned long config0;
	/* config2 register */
	unsigned short config2;
};

extern int health_check;

int bitstream_ioctl(struct xclmgmt_dev *lro, const void __user *arg);
int bitstream_ioctl_axlf(struct xclmgmt_dev *lro, const void __user *arg);
int ocl_freqscaling_ioctl(struct xclmgmt_dev *lro, const void __user *arg);
void freezeAXIGate(struct xclmgmt_dev *lro);
void freeAXIGate(struct xclmgmt_dev *lro);
void platform_axilite_flush(struct xclmgmt_dev *lro);
u16 get_dsa_version(struct xclmgmt_dev *lro);
void fill_frequency_info(struct xclmgmt_dev *lro, struct xclmgmt_ioc_info *obj);
void device_info(struct xclmgmt_dev *lro, struct xclmgmt_ioc_info *obj);
long load_boot_firmware(struct xclmgmt_dev *lro);
long ocl_freqscaling(struct xclmgmt_dev *lro, bool force);
long mgmt_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
int mgmt_load_reset_mini_bitstream(struct xclmgmt_dev *lro);

// Thread.c
void init_health_thread(struct xclmgmt_dev *lro);
void fini_health_thread(const struct xclmgmt_dev *lro);

// utils.c
unsigned compute_unit_busy(struct xclmgmt_dev *lro);
int16_t onchip_temp(uint32_t temp);
unsigned short to_volt(uint32_t volt);
void check_temp_within_range(int16_t temp);
void check_volt_within_range(unsigned short volt);
bool check_axi_firewall( struct xclmgmt_dev *lro);
bool check_and_reset_firewall(struct xclmgmt_dev *lro, bool req_reset);
int pci_fundamental_reset(struct xclmgmt_dev *lro);
void xocl_reset(struct pci_dev *pdev, bool prepare);
void xdma_reset(struct pci_dev *pdev, bool prepare);
/* Note: Use reset_hot_ioctl over pcie fundamental reset.
 * This method is known to work better.
 */
long reset_hot_ioctl(struct xclmgmt_dev *lro);
bool is_mb_power(const struct xclmgmt_dev *lro);
bool is_unified(const struct xclmgmt_dev *lro);
bool is_ultrascale(const struct xclmgmt_dev *lro);
bool is_ultrascale_plus(const struct xclmgmt_dev *lro);
bool is_XPR(const struct xclmgmt_dev *lro);
void xdma_reset(struct pci_dev *pdev, bool prepare);
void xclmgmt_reset_pci(struct xclmgmt_dev *lro);

// firewall.c
void init_firewall(struct xclmgmt_dev *lro);
void xclmgmt_killall_processes(struct xclmgmt_dev *lro);
void xclmgmt_list_add(struct xclmgmt_dev *lro, struct pid *new_pid);
//struct proc_list *xclmgmt_find_by_pid(struct xclmgmt_dev *lro, struct pid *find_pid);
void xclmgmt_list_remove(struct xclmgmt_dev *lro, struct pid *remove_pid);
void xclmgmt_list_del(struct xclmgmt_dev *lro);
bool xclmgmt_check_proc(struct xclmgmt_dev *lro, struct pid *pid);

// mgmt-xvc.c
long xvc_ioctl(struct xclmgmt_dev *lro, const void __user *arg);

//mgmt-sysfs.c
int mgmt_init_sysfs(struct device *dev);
void mgmt_fini_sysfs(struct device *dev);

//mgmt-mb.c
int mgmt_init_mb(struct xclmgmt_dev *lro);
void mgmt_fini_mb(struct xclmgmt_dev *lro);
int mgmt_start_mb(struct xclmgmt_dev *lro);
int mgmt_stop_mb(struct xclmgmt_dev *lro);

//mgmt-i2c.c
int mgmt_xiic_init(struct xclmgmt_dev *lro);
void mgmt_xiic_fini(struct xclmgmt_dev *lro);

#endif


