/*
 * Simple Driver for Management PF
 *
 * Copyright (C) 2017 Xilinx, Inc.
 *
 * Code borrowed from Xilinx SDAccel XDMA driver
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
 *
 */
#include "mgmt-core.h"
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>

//#define USE_FEATURE_ROM


#define MGMT_MODULE_VERSION				      \
	__stringify(XCLMGMT_DRIVER_MAJOR) "."		      \
	__stringify(XCLMGMT_DRIVER_MINOR) "."		      \
	__stringify(XCLMGMT_DRIVER_PATCHLEVEL)


MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Sonal Santan <sonal.santan@xilinx.com>");
MODULE_VERSION(MGMT_MODULE_VERSION);

MODULE_DESCRIPTION("Xilinx SDx management function driver");

static const struct pci_device_id pci_ids[] = {
	{ PCI_DEVICE(0x10ee, 0x4A47), },
	{ PCI_DEVICE(0x10ee, 0x4A87), },
	{ PCI_DEVICE(0x10ee, 0x4B47), },
	{ PCI_DEVICE(0x10ee, 0x4B87), },
	{ PCI_DEVICE(0x10ee, 0x684F), },
	{ PCI_DEVICE(0x10ee, 0x688F), },
	{ PCI_DEVICE(0x10ee, 0x694F), },
	{ PCI_DEVICE(0x10ee, 0x698F), },
	{ PCI_DEVICE(0x10ee, 0x6A4F), },
	{ PCI_DEVICE(0x10ee, 0x6A8F), },
	{ PCI_DEVICE(0x10ee, 0x6E4F), },
	{ PCI_DEVICE(0x10ee, 0x6B0F), },
	{ PCI_DEVICE(0x10ee, 0x6F8F), },
	{ PCI_DEVICE(0x13fe, 0x006C), },
	{ PCI_DEVICE(0x19e5, 0xd518), },
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, pci_ids);

int health_interval = 5;
module_param(health_interval, int, (S_IRUGO|S_IWUSR));
MODULE_PARM_DESC(health_interval,
	"Interval (in sec) after which the health thread is run. (1 = Minimum, 5 = default)");

int health_check = 1;
module_param(health_check, int, (S_IRUGO|S_IWUSR));
MODULE_PARM_DESC(health_check,
	"Enable health thread that checks the status of AXI Firewall and SYSMON. (0 = disable, 1 = enable)");

int skip_load_dsabin = 0;
module_param(skip_load_dsabin, int, (S_IRUGO|S_IWUSR));
MODULE_PARM_DESC(skip_load_dsabin,
	"Enable skip_load_dsabin to force driver to load without vailid dsabin. Thus xbsak flash is able to upgrade firmware. (0 = load dsabin, 1 = skip load dsabin)");

static int instance = 0;
static dev_t xclmgmt_devnode;
static struct class *xclmgmt_class;

/*
 * Called when the device goes from unused to used.
 */
static int char_open(struct inode *inode, struct file *file)
{
	struct xclmgmt_char *lro_char;
	struct xclmgmt_dev *lro;
	/* pointer to containing data structure of the character device inode */
	lro_char = container_of(inode->i_cdev, struct xclmgmt_char, cdev);

	/* create a reference to our char device in the opened file */
	file->private_data = lro_char;
	lro = lro_char->lro;
	BUG_ON(!lro);

	mutex_lock(&lro->busy_mutex);
	if (lro->reset_firewall) {
		mutex_unlock(&lro->busy_mutex);
		mgmt_err(lro, "resetting firewal");
		return -EAGAIN;
	}
	mutex_unlock(&lro->busy_mutex);

	printk(KERN_INFO "/dev/xclmgmt0 %s opened by pid: %d\n", DRV_NAME, pid_nr(task_tgid(current)) );
#ifdef AXI_FIREWALL
	mutex_lock(&lro->proc_mutex);
	xclmgmt_list_add(lro_char->lro, task_tgid(current));
	mutex_unlock(&lro->proc_mutex);
#endif
	return 0;
}

/*
 * Called when the device goes from used to unused.
 */
static int char_close(struct inode *inode, struct file *file)
{
	struct xclmgmt_dev *lro;
	struct xclmgmt_char *lro_char = (struct xclmgmt_char *)file->private_data;
	BUG_ON(!lro_char);

	/* fetch device specific data stored earlier during open */
	printk(KERN_INFO "Closing node %s (0x%p, 0x%p) opened by pid: %d\n", DRV_NAME, inode, file, pid_nr(task_tgid(current)) );
	lro = lro_char->lro;
	BUG_ON(!lro);
#ifdef AXI_FIREWALL
	mutex_lock(&lro->proc_mutex);
	xclmgmt_list_remove(lro_char->lro, task_tgid(current));
	mutex_unlock(&lro->proc_mutex);
#endif

	return 0;
}

/*
 * Unmap the BAR regions that had been mapped earlier using map_bars()
 */
static void unmap_bars(struct xclmgmt_dev *lro)
{
	int i;
	for (i = 0; i < XCLMGMT_MAX_BAR; i++) {
		/* is this BAR mapped? */
		if (lro->bar[i]) {
			/* unmap BAR */
			pci_iounmap(lro->pci_dev, lro->bar[i]);
			/* mark as unmapped */
			lro->bar[i] = NULL;
		}
	}
}

/* map_bars() -- map device regions into kernel virtual address space
 *
 * Map the device memory regions into kernel virtual address space after
 * verifying their sizes respect the minimum sizes needed, given by the
 * bar_map_sizes[] array.
 */
static int map_bars(struct xclmgmt_dev *lro)
{
	int rc;
	int i;

	/* iterate through all the BARs */
	for (i = 0; i < XCLMGMT_MAX_BAR; i++) {
		resource_size_t bar_length = pci_resource_len(lro->pci_dev, i);
		resource_size_t map_length = bar_length;
		lro->bar[i] = NULL;
		printk(KERN_INFO "%s: %s Idx: %d, bar len: %d \n", DRV_NAME, __FUNCTION__, i, (int)bar_length);

		/* skip non-present BAR2 and higher */
		if (!bar_length) continue;

		lro->bar[i] = pci_iomap(lro->pci_dev, i, map_length);
		if (!lro->bar[i]) {
			printk(KERN_INFO "Could not map BAR #%d. See bar_map_size option to reduce the map size.\n", i);
			rc = -EIO;
			goto fail;
		}

		lro->bar_map_size[i] = bar_length;
	}
	/* succesfully mapped all required BAR regions */
	rc = 0;
	goto success;
fail:
	/* unwind; unmap any BARs that we did map */
	unmap_bars(lro);
success:
	return rc;
}


//static u8 get_lanes(const struct xclmgmt_dev *lro)
//{
//	return lro->pci_dev->device & 0x1f;
//}

static void fill_sysmon_info(struct xclmgmt_dev *lro, struct xclmgmt_ioc_info *obj)
{
	u32 val;

	printk(KERN_DEBUG "%s: Sysmon supported. \n", DRV_NAME);
	// From Sysmon
	val = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + SYSMON_BASE + SYSMON_TEMP);
	obj->onchip_temp = onchip_temp(val);
	val = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + SYSMON_BASE + SYSMON_VCCINT);
	obj->vcc_int = to_volt(val);
	val = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + SYSMON_BASE + SYSMON_VCCAUX);
	obj->vcc_aux= to_volt(val);
	val = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + SYSMON_BASE + SYSMON_VCCBRAM);
	obj->vcc_bram= to_volt(val);

	// TODO: Fill in the right values.
	obj->fan_temp	= 0;
	obj->fan_speed	= 0;
}

static u64 find_feature_id(const struct xclmgmt_dev *lro)
{
	return lro->header.TimeSinceEpoch;
}

static void read_featurerom_info(struct xclmgmt_dev *lro) {
	BUG_ON(!lro);

	if(get_dsa_version(lro) <= 0x33) {
		printk(KERN_INFO "%s DSA version less than 3.3 detected. Skip reading feature rom.", DRV_NAME);
		return;
	}

	memcpy_fromio(&lro->header, lro->bar[XCLMGMT_MAIN_BAR] + FEATURE_ROM_BASE,sizeof(struct FeatureRomHeader));

	printk(KERN_INFO "ROM magic : %s\n", lro->header.EntryPointString);
	printk(KERN_INFO "VBNV: %s", lro->header.VBNVName);
	if (strstr(lro->header.VBNVName, "-xare")) {//This is ARE device
	  lro->header.DDRChannelCount = lro->header.DDRChannelCount - 1;//ARE is mapped as another DDR
	  printk(KERN_INFO "This an ARE Device\n");
	}
	printk(KERN_INFO "DDR channel count : %d\n", lro->header.DDRChannelCount);
	printk(KERN_INFO "DDR channel size: %d GB\n", lro->header.DDRChannelSize);
	printk(KERN_INFO "Major Version: %d \n", lro->header.MajorVersion);
	printk(KERN_INFO "Minor Version: %d \n", lro->header.MinorVersion);
	printk(KERN_INFO "IPBuildID: %u\n", lro->header.IPBuildID);
	printk(KERN_INFO "TimeSinceEpoch: %llx\n", lro->header.TimeSinceEpoch);
	printk(KERN_INFO "FeatureBitMap: %llx\n", lro->header.FeatureBitMap);

//	if(lro->header.MajorVersion >= 10)
//		printk(KERN_INFO "DRBaseAddress: %llx\n", lro->header.DRBaseAddress);


}

static void fill_featurerom_info(struct xclmgmt_dev *lro, struct xclmgmt_ioc_info *obj)
{
    u32 are_link_status_reg;
    u32 are_error_status_reg;
	BUG_ON(!lro);
	BUG_ON(!obj);

	printk(KERN_INFO "%s %s", DRV_NAME, __FUNCTION__);

	if(get_dsa_version(lro) <= 0x33) {
		printk(KERN_INFO "%s DSA version less than 3.3 detected. Skip reading feature rom.", DRV_NAME);
		return;
	}

	obj->ddr_channel_num = lro->header.DDRChannelCount;
	obj->ddr_channel_size = lro->header.DDRChannelSize;
	memcpy(obj->vbnv, lro->header.VBNVName, 64);
	memcpy(obj->fpga, lro->header.FPGAPartName, 64);
	obj->time_stamp = lro->header.TimeSinceEpoch;

	//Sarab: Adding here temporarily to check ARE channel up status.
	//VBNV is: xilinx:xil-accel-rd-vu9p:4ddr-xpr-xare:4.6
	//FPGAPartName is: xcvu9p-flgb2104-2-i
	//Link Status is: 0x03
	//Error Status is: 0x00000000

	if (strstr(lro->header.VBNVName, "-xare")) {//This is ARE device
	    are_link_status_reg = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + 0x90010) & 0x3;
	    are_error_status_reg = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + 0x90018);
	    printk(KERN_INFO "ARE INFO: Link Status is: 0x%02X\n", are_link_status_reg);
	    printk(KERN_INFO "ARE INFO: Error Status is: 0x%08X\n", are_error_status_reg);
	    printk(KERN_INFO "ARE INFO: DSA ROM Timestamp: %llx\n", lro->header.TimeSinceEpoch);
	}

	obj->num_clocks = 0;
	obj->isXPR = is_XPR(lro);
	printk(KERN_INFO "%s %s isXPR: %d", DRV_NAME, __FUNCTION__, obj->isXPR);

}

static void fill_pcie_link_info(const struct xclmgmt_dev *lro, struct xclmgmt_ioc_info *obj)
{
	u16 stat;
	long result;

	printk(KERN_INFO "%s %s", DRV_NAME, __FUNCTION__);
	obj->pcie_link_width = 0;
	obj->pcie_link_speed = 0;
	result = pcie_capability_read_word(lro->pci_dev, PCI_EXP_LNKSTA, &stat);
	if (result) {
		printk(KERN_INFO "%s %s PCIe link status is busy. \n", DRV_NAME, __FUNCTION__);
		return;
	}
	obj->pcie_link_width = (stat & PCI_EXP_LNKSTA_NLW) >> PCI_EXP_LNKSTA_NLW_SHIFT;
	obj->pcie_link_speed = stat & PCI_EXP_LNKSTA_CLS;
	return;
}

void device_info(struct xclmgmt_dev *lro, struct xclmgmt_ioc_info *obj)
{
	u32 val;
	memset(obj, 0, sizeof(struct xclmgmt_ioc_info));
	obj->vendor = lro->pci_dev->vendor;
	obj->device = lro->pci_dev->device;
	obj->subsystem_vendor = lro->pci_dev->subsystem_vendor;
	obj->subsystem_device = lro->pci_dev->subsystem_device;
	obj->driver_version = XCLMGMT_DRIVER_MAJOR * 1000 + XCLMGMT_DRIVER_MINOR * 100 + XCLMGMT_DRIVER_PATCHLEVEL;
	obj->pci_slot = PCI_SLOT(lro->pci_dev->devfn);
	printk(KERN_INFO "%s %s PCI Slot: %d \n", DRV_NAME, __FUNCTION__, obj->pci_slot);

	val = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + GENERAL_STATUS_BASE);
	obj->mig_calibration[0] = (val & BIT(0)) ? true : false;
	obj->mig_calibration[1] = (val & BIT(0)) ? true : false;
	obj->mig_calibration[2] = (val & BIT(0)) ? true : false;
	obj->mig_calibration[3] = (val & BIT(0)) ? true : false;
	printk(KERN_INFO "%s %s MIG Calibration: %d \n", DRV_NAME, __FUNCTION__, val);

	fill_frequency_info(lro, obj);
	fill_sysmon_info(lro, obj);
	fill_featurerom_info(lro, obj);
	fill_pcie_link_info(lro, obj);
}

/* maps the PCIe BAR into user space for memory-like access using mmap() */
static int bridge_mmap(struct file *file, struct vm_area_struct *vma)
{
	int rc;
	struct xclmgmt_dev *lro;
	struct xclmgmt_char *lro_char = (struct xclmgmt_char *)file->private_data;
	unsigned long off;
	unsigned long phys;
	unsigned long vsize;
	unsigned long psize;
	BUG_ON(!lro_char);
	lro = lro_char->lro;
	BUG_ON(!lro);

	if (!capable(CAP_SYS_ADMIN))
		return -EACCES;

	off = vma->vm_pgoff << PAGE_SHIFT;
	/* BAR physical address */
	phys = pci_resource_start(lro->pci_dev, lro_char->bar) + off;
	vsize = vma->vm_end - vma->vm_start;
	/* complete resource */
	psize = pci_resource_end(lro->pci_dev, lro_char->bar) - pci_resource_start(lro->pci_dev, lro_char->bar) + 1 - off;

	printk(KERN_INFO "mmap(): lro_char = 0x%08lx\n", (unsigned long)lro_char);
	printk(KERN_INFO "mmap(): lro_char->bar = %d\n", lro_char->bar);
	printk(KERN_INFO "mmap(): lro = 0x%p\n", lro);
	printk(KERN_INFO "mmap(): pci_dev = 0x%08lx\n", (unsigned long)lro->pci_dev);

	printk(KERN_INFO "off = 0x%lx\n", off);
	printk(KERN_INFO "start = 0x%llx\n", (unsigned long long)pci_resource_start(lro->pci_dev, lro_char->bar));
	printk(KERN_INFO "phys = 0x%lx\n", phys);

	if (vsize > psize)
		return -EINVAL;
	/* pages must not be cached as this would result in cache line sized
	   accesses to the end point */
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	/* prevent touching the pages (byte access) for swap-in,
	   and prevent the pages from being swapped out */
#ifndef VM_RESERVED
	vma->vm_flags |= VM_IO | VM_DONTEXPAND | VM_DONTDUMP;
#else
	vma->vm_flags |= VM_IO | VM_RESERVED;
#endif
	/* make MMIO accessible to user space */
	rc = io_remap_pfn_range(vma, vma->vm_start, phys >> PAGE_SHIFT,
				vsize, vma->vm_page_prot);
	printk(KERN_INFO "io_remap_pfn_range(vma=0x%p, vma->vm_start=0x%lx, phys=0x%lx, size=%lu) = %d\n",
	       vma, vma->vm_start, phys >> PAGE_SHIFT, vsize, rc);
	if (rc)
		return -EAGAIN;
	//vma->vm_ops = &vm_ops;
	return 0;
}



/*
 * character device file operations for control bus (through control bridge)
 */
static struct file_operations ctrl_fops = {
	.owner = THIS_MODULE,
	.open = char_open,
	.release = char_close,
	.mmap = bridge_mmap,
	.unlocked_ioctl = mgmt_ioctl,
};

/* create_char() -- create a character device interface to data or control bus
 *
 * If at least one SG DMA engine is specified, the character device interface
 * is coupled to the SG DMA file operations which operate on the data bus. If
 * no engines are specified, the interface is coupled with the control bus.
 */
static struct xclmgmt_char *create_char(struct xclmgmt_dev *lro, int bar)
{
	struct xclmgmt_char *lro_char;
	int rc;
    unsigned int instance;

	printk(KERN_INFO "%s: %s \n", DRV_NAME, __FUNCTION__);
	/* allocate book keeping data structure */
	lro_char = kzalloc(sizeof(struct xclmgmt_char), GFP_KERNEL);
	if (!lro_char)
		return NULL;

	/* dynamically pick a number into cdevno */
	lro_char->lro = lro;
	lro_char->bar = bar;
	/* couple the control device file operations to the character device */
	cdev_init(&lro_char->cdev, &ctrl_fops);
	lro_char->cdev.owner = THIS_MODULE;
    instance = lro->instance;
	lro_char->cdev.dev = MKDEV(MAJOR(xclmgmt_devnode), instance);
	rc = cdev_add(&lro_char->cdev, lro_char->cdev.dev, 1);
	if (rc < 0) {
		printk(KERN_INFO "cdev_add() = %d\n", rc);
		goto fail_add;
	}

	lro_char->sys_device = device_create(xclmgmt_class, &lro->pci_dev->dev, lro_char->cdev.dev, NULL,
					     DRV_NAME "%d", lro->instance);

	if (IS_ERR(lro_char->sys_device)) {
		rc = PTR_ERR(lro_char->sys_device);
		goto fail_device;
	}
	else
		goto success;
fail_device:
	cdev_del(&lro_char->cdev);
fail_add:
	kfree(lro_char);
	lro_char = NULL;
success:
	return lro_char;
}

static int destroy_sg_char(struct xclmgmt_char *lro_char)
{
	BUG_ON(!lro_char);
	BUG_ON(!lro_char->lro);
	BUG_ON(!xclmgmt_class);
	BUG_ON(!lro_char->sys_device);
	if (lro_char->sys_device)
		device_destroy(xclmgmt_class, lro_char->cdev.dev);
	cdev_del(&lro_char->cdev);
	kfree(lro_char);
	return 0;
}

struct pci_dev *find_user_node(const struct pci_dev *pdev, const struct pci_device_id *id)
{
	unsigned int slot = PCI_SLOT(pdev->devfn);
	unsigned int func = PCI_FUNC(pdev->devfn);
	static struct pci_dev *from = NULL;

	printk(KERN_INFO "find_user_node (slot = %d, func = %d, vendor = 0x%04x, device = 0x%04x)\n",
	       slot, func, id->vendor, id->device);

	/* if we are function one then the zero
	 * function has the xdma node */
	if (func == 1)
		return pci_get_slot(pdev->bus, PCI_DEVFN(slot, func-1));

	/* Otherwise, we are likely in a virtualized environment so poke around for
	 * the correct device_id */
	if (from != NULL) {
		// pci_get_device will decrease the refcnt by one of the pci_dev
		// increase refcnt by one pre-emptively to prevent pre-mature deletion
		pci_dev_get(from);
	}

	/* Try to find the next device with the correct device id */
	/* This assumes that the devices are ordered after virtualization */
	from = pci_get_device(id->vendor, id->device+1, from);

	/* TODO: validate that both functions are on the same device */
	/* this could be some kind of loop back function or a unique identifier */
	return from;
}

static int xclmgmt_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	int rc = 0;
	u32 value;
	struct xclmgmt_dev *lro = NULL;

	printk(KERN_DEBUG "probe(pdev = 0x%p, pci_id = 0x%p)\n", pdev, id);

	rc = pci_enable_device(pdev);
	if (rc) {
		printk(KERN_DEBUG "pci_enable_device() failed, rc = %d.\n", rc);
		return rc;
	}

	/* allocate zeroed device book keeping structure */
	lro = kzalloc(sizeof(struct xclmgmt_dev), GFP_KERNEL);
	if (!lro) {
		printk(KERN_DEBUG "Could not kzalloc(xclmgmt_dev).\n");
		goto err_alloc;
	}
	/* create a device to driver reference */
	dev_set_drvdata(&pdev->dev, lro);
	/* create a driver to device reference */
	lro->pci_dev = pdev;
	printk(KERN_DEBUG "probe() lro = 0x%p\n", lro);
	value = lro->pci_dev->subsystem_device;
	printk(KERN_DEBUG "pci_indevice()\n");

	printk(KERN_DEBUG "pci_request_regions()\n");
	rc = pci_request_regions(pdev, DRV_NAME);
	/* could not request all regions? */
	if (rc) {
		printk(KERN_DEBUG "pci_request_regions() = %d, device in use?\n", rc);
		goto err_regions;
	}

	printk(KERN_DEBUG "map_bars()\n");
	/* map BARs */
	rc = map_bars(lro);
	if (rc)
		goto err_map;

	lro->instance = instance++;
	lro->user_char_dev = create_char(lro, 0);
	if (!lro->user_char_dev) {
		printk(KERN_DEBUG "create_char(user_char_dev) failed\n");
		goto err_cdev;
	}

	lro->stash.magic = 0xBBBBBBBBUL;

	lro->user_pci_dev = find_user_node(pdev, id);
	if (!lro->user_pci_dev) {
#ifdef MGMT_1PF
		printk(KERN_INFO "Could not find user physical function for instance %d\n", lro->instance);
#else
		printk(KERN_ERR "Could not find user physical function for instance %d\n", lro->instance);
		goto err_user;
#endif
	}

	// Workaround needed on some platforms. Will clear out any stale data after the platform has been reset
	if(get_dsa_version(lro) == 0x50)
		platform_axilite_flush(lro);

	read_featurerom_info(lro);
	lro->feature_id = find_feature_id(lro);

	if (!skip_load_dsabin) {
		rc = load_boot_firmware(lro);
		if (rc) {
			goto err_user;
		}
	}


	mutex_init(&lro->busy_mutex);

#ifdef AXI_FIREWALL
	init_firewall(lro);
#endif
	if(get_dsa_version(lro) > 0x50)
		mgmt_init_mb(lro);

	mgmt_init_sysfs(&pdev->dev);

	init_health_thread(lro);

	mgmt_xiic_init(lro);

	goto end;

err_user:
	destroy_sg_char(lro->user_char_dev);
err_cdev:
	unmap_bars(lro);
err_map:
	pci_release_regions(pdev);
err_regions:
	kfree(lro);
	dev_set_drvdata(&pdev->dev, NULL);
err_alloc:
	pci_disable_device(pdev);
end:
	return rc;
}

static void xclmgmt_remove(struct pci_dev *pdev)
{
	struct xclmgmt_dev *lro;
    
	if ((pdev == 0) || (dev_get_drvdata(&pdev->dev) == 0)) {        
        printk(KERN_INFO
    	       "remove(dev is null) or pdev->dev.driver_data is null\n");
		return;
	}
    
	lro = (struct xclmgmt_dev *)dev_get_drvdata(&pdev->dev);
    if (0 == lro)
    {
        printk(KERN_INFO
    	       "remove(dev = 0x%p) where lro is null\n", pdev);
        return;
    }
    
	printk(KERN_INFO
	       "remove(dev = 0x%p) where pdev->dev.driver_data = 0x%p\n",
	       pdev, lro);
	if (lro->pci_dev != pdev) {
		printk(KERN_INFO
		       "pdev->dev.driver_data->pci_dev (0x%08lx) != pdev (0x%08lx)\n",
		       (unsigned long)lro->pci_dev, (unsigned long)pdev);
	}

	/* kill the thread */
	fini_health_thread(lro);

#ifdef AXI_FIREWALL
	mutex_lock(&lro->proc_mutex);
	xclmgmt_list_del(lro);
	mutex_unlock(&lro->proc_mutex);
#endif

	mgmt_xiic_fini(lro);
	if(get_dsa_version(lro) > 0x50)
		mgmt_fini_mb(lro);

	/* remove user character device */
	if (lro->user_char_dev) {
		destroy_sg_char(lro->user_char_dev);
		lro->user_char_dev = 0;
	}

	/* unmap the BARs */
	unmap_bars(lro);
	printk(KERN_INFO "Unmapping BARs.\n");
	pci_disable_device(pdev);
	pci_release_regions(pdev);

	kfree(lro);
	mgmt_fini_sysfs(&pdev->dev);
	dev_set_drvdata(&pdev->dev, NULL);
}


static struct pci_driver xclmgmt_driver = {
	.name = DRV_NAME,
	.id_table = pci_ids,
	.probe = xclmgmt_probe,
	.remove = xclmgmt_remove,
	/* resume, suspend are optional */
};

static int __init xclmgmt_init(void)
{
	int res;

	printk(KERN_INFO DRV_NAME " %s init()\n", MGMT_MODULE_VERSION);
	xclmgmt_class = class_create(THIS_MODULE, DRV_NAME);
	if (IS_ERR(xclmgmt_class))
		return PTR_ERR(xclmgmt_class);
	res = alloc_chrdev_region(&xclmgmt_devnode, XCLMGMT_MINOR_BASE,
				  XCLMGMT_MINOR_COUNT, DRV_NAME);
	if (res)
		goto alloc_err;

	res = pci_register_driver(&xclmgmt_driver);
	if (!res)
		return 0;

	unregister_chrdev_region(xclmgmt_devnode, XCLMGMT_MINOR_COUNT);
alloc_err:
	printk(KERN_INFO DRV_NAME " init() err\n");
	class_destroy(xclmgmt_class);
	return res;
}

static void xclmgmt_exit(void)
{
	printk(KERN_INFO DRV_NAME" exit()\n");
	/* unregister this driver from the PCI bus driver */
	pci_unregister_driver(&xclmgmt_driver);
	unregister_chrdev_region(xclmgmt_devnode, XCLMGMT_MINOR_COUNT);
	class_destroy(xclmgmt_class);
}

module_init(xclmgmt_init);
module_exit(xclmgmt_exit);

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
