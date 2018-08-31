/**
 *  Copyright (C) 2017 Xilinx, Inc. All rights reserved.
 *
 *  Utility Functions for sysmon, axi firewall and other peripherals.
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
#include <linux/module.h>

#define XCLMGMT_RESET_MAX_RETRY		10
void xdma_reset_notify(struct pci_dev *pdev, bool prepare);
void xocl_reset_notify(struct pci_dev *pdev, bool prepare);

/**
 * @returns: NULL if AER apability is not found walking up to the root port
 *         : pci_dev ptr to the port which is AER capable.
 */
static struct pci_dev * find_aer_cap(struct pci_dev *bridge)
{
	struct pci_dev *prev_bridge = bridge;
	int cap;

	if (bridge == NULL)
		return NULL;
	/*
	 * Walk the hierarchy up to the root port
	 **/
	do {
		printk(KERN_DEBUG "%s: inside do while loop..find_aer_cap \n", DRV_NAME);
		cap = pci_find_ext_capability(bridge, PCI_EXT_CAP_ID_ERR);
		if (cap) {
			printk(KERN_DEBUG "%s: AER capability found. \n", DRV_NAME);
			return bridge;
		}

		prev_bridge = bridge;
		bridge = bridge->bus->self;

		if (!bridge || prev_bridge == bridge) {
			printk(KERN_DEBUG "%s: AER capability not found. Ignoring boot command. \n", DRV_NAME);
			return NULL;
		}

	} while (pci_pcie_type(bridge) != PCI_EXP_TYPE_ROOT_PORT);

	return NULL;
}

/*
 * pcie_(un)mask_surprise_down inspired by myri10ge driver, myri10ge.c
 */
static int pcie_mask_surprise_down(struct pci_dev *pdev, u32 *orig_mask)
{
	struct pci_dev *bridge = pdev->bus->self;
	int cap;
	u32 mask;

	printk(KERN_INFO "%s: pcie_mask_surprise_down \n", DRV_NAME);

	bridge = find_aer_cap(bridge);
	if(bridge) {
		cap = pci_find_ext_capability(bridge, PCI_EXT_CAP_ID_ERR);
		if (cap) {
			pci_read_config_dword(bridge, cap + PCI_ERR_UNCOR_MASK, orig_mask);
			mask = *orig_mask;
			mask |= 0x20;
			pci_write_config_dword(bridge, cap + PCI_ERR_UNCOR_MASK, mask);
			return 0;
		}
	}

	return -ENOSYS;
}

static int pcie_unmask_surprise_down(struct pci_dev *pdev, u32 orig_mask)
{
	struct pci_dev *bridge = pdev->bus->self;
	int cap;

	printk(KERN_DEBUG "%s: pcie_unmask_surprise_down \n", DRV_NAME);

	bridge = find_aer_cap(bridge);
	if(bridge) {
		cap = pci_find_ext_capability(bridge, PCI_EXT_CAP_ID_ERR);
		if (cap) {
			pci_write_config_dword(bridge, cap + PCI_ERR_UNCOR_MASK, orig_mask);
			return 0;
		}
	}

	return -ENOSYS;
}

void freezeAXIGate(struct xclmgmt_dev *lro)
{
	u8 w = 0x0;
	u32 t;

#if 0
	//freeze SCHED MicroBlaze
	if ((lro->header.FeatureBitMap & MB_SCHEDULER) || (lro->header.FeatureBitMap & BOARD_MGMT_ENBLD))
		iowrite32(0x0,lro->bar[XCLMGMT_MAIN_BAR] + MICROBLAZE_RESET_GPIO);
#endif

	BUG_ON(lro->axi_gate_frozen);
//	printk(KERN_DEBUG "IOCTL %s:%d\n", __FILE__, __LINE__);
	t = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + AXI_GATE_OFFSET_READ);
//	printk("Register %x\n", t);
	iowrite8(w, lro->bar[XCLMGMT_MAIN_BAR] + AXI_GATE_OFFSET);
	t = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + AXI_GATE_OFFSET_READ);
//	printk("Register %x\n", t);
	lro->axi_gate_frozen = 1;
	printk(KERN_DEBUG "%s: Froze AXI gate\n", DRV_NAME);

	// New ICAP reset seq frm Julian/Kapil's email (applicable only to unified dsa;s)
	// The else part should be taken out once all the supported dsa's have been
	// rev'd up and they use the latest version of the ICAP IP.
	if(is_unified(lro)) {
		printk(KERN_DEBUG "%s: Using new unified dsa reset sequence\n", DRV_NAME);
		t = 0x8;
		iowrite32(t, lro->bar[XCLMGMT_MAIN_BAR] + XHWICAP_CR);
		ndelay(2000);

		t = 0x0;
		iowrite32(t, lro->bar[XCLMGMT_MAIN_BAR] + XHWICAP_CR);
		ndelay(2000);

		t = 0x4;
		iowrite32(t, lro->bar[XCLMGMT_MAIN_BAR] + XHWICAP_CR);
		ndelay(2000);

		t = 0x0;
		iowrite32(t, lro->bar[XCLMGMT_MAIN_BAR] + XHWICAP_CR);
		ndelay(2000);
	}
	else {
		t = 0xC;
		iowrite32(t, lro->bar[XCLMGMT_MAIN_BAR] + XHWICAP_CR);
		ndelay(20);
		printk(KERN_DEBUG "%s: Reset all register and cleared all FIFOs \n", DRV_NAME);

	}
}

void freeAXIGate(struct xclmgmt_dev *lro)
{
	/*
	 * First pulse the OCL RESET. This is important for PR with multiple
	 * clocks as it resets the edge triggered clock converter FIFO
	 */
	u8 w = 0x2;
	u32 t;

#if 0
	//freeze MicroBlaze
	if ((lro->header.FeatureBitMap & MB_SCHEDULER) || (lro->header.FeatureBitMap & BOARD_MGMT_ENBLD))
		iowrite32(0x1,lro->bar[XCLMGMT_MAIN_BAR] + MICROBLAZE_RESET_GPIO);
#endif

	//Workaround needed for 5.0 platforms
	if(get_dsa_version(lro) == 0x50)
		check_and_reset_firewall(lro, true);

	BUG_ON(!lro->axi_gate_frozen);
//	printk(KERN_DEBUG "IOCTL %s:%d\n", __FILE__, __LINE__);
	t = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + AXI_GATE_OFFSET_READ);
//	printk("Register %x\n", t);
	iowrite8(w, lro->bar[XCLMGMT_MAIN_BAR] + AXI_GATE_OFFSET);
	ndelay(500);

	w = 0x0;
	t = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + AXI_GATE_OFFSET_READ);
//	printk("Register %x\n", t);
	iowrite8(w, lro->bar[XCLMGMT_MAIN_BAR] + AXI_GATE_OFFSET);
	ndelay(500);

	w = 0x2;
	t = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + AXI_GATE_OFFSET_READ);
//	printk("Register %x\n", t);
	iowrite8(w, lro->bar[XCLMGMT_MAIN_BAR] + AXI_GATE_OFFSET);
	ndelay(500);

	w = 0x3;
	t = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + AXI_GATE_OFFSET_READ);
//	printk("Register %x\n", t);
	iowrite8(w, lro->bar[XCLMGMT_MAIN_BAR] + AXI_GATE_OFFSET);
	ndelay(500);
	t = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + AXI_GATE_OFFSET_READ);
//	printk("Register %x\n", t);
	lro->axi_gate_frozen = 0;
	printk(KERN_DEBUG "%s: Un-froze AXI gate\n", DRV_NAME);

}

/**
 * Prepare the XDMA engine for reset
 * prepare = true will call xdma_offline
 * prepare = false will call xdma online
 */
void xdma_reset(struct pci_dev *pdev, bool prepare)
{
	void (*reset)(struct pci_dev *pdev, bool prepare);
	reset = symbol_get(xdma_reset_notify);
	if (reset) {
		printk(KERN_INFO "%s: Calling xdma_reset_notify \n", DRV_NAME);
		reset(pdev, prepare);
		symbol_put(xdma_reset_notify);
	}
}

u16 get_dsa_version(struct xclmgmt_dev *lro)
{
    //hw dsa version    
    if (lro->pci_dev->vendor == 0x19e5) {
        return 0x50;
    }
    
	return lro->pci_dev->subsystem_device & 0xff;
}

/**
 * Workaround for some DSAs that need axilite bus flushed after reset
 */
void platform_axilite_flush(struct xclmgmt_dev *lro)
{
	u32 val, i;

	if(get_dsa_version(lro) <= 0x33) {
		printk(KERN_INFO "%s DSA version less than 3.3 detected. Skipping axilite flush sequence.", DRV_NAME);
		return;
	}

	printk(KERN_INFO "Flushing axilite busses.");

	/* The flush sequence works as follows:
	 * Read axilite peripheral up to 4 times
	 * Check if firewall trips and clear it.
	 * Touch all axilite interconnects with clock crossing
	 * in platform which requires reading multiple peripherals
	 * (Feature ROM, MB Reset GPIO, Sysmon, ICAP, Fake CU)
	 */
	for (i = 0; i < 4; i++) {
		val = MGMT_READ_REG32(lro, FEATURE_ROM_BASE);
		check_and_reset_firewall(lro, true);
	}

	for (i = 0; i < 4; i++) {
		val = MGMT_READ_REG32(lro, MB_GPIO);
		check_and_reset_firewall(lro, true);
	}

	for (i = 0; i < 4; i++) {
		val = MGMT_READ_REG32(lro, SYSMON_BASE);
		check_and_reset_firewall(lro, true);
	}

	for (i = 0; i < 4; i++) {
		val = MGMT_READ_REG32(lro, MB_IMAGE_SCHE);
		check_and_reset_firewall(lro, true);
	}

	for (i = 0; i < 4; i++) {
		val = MGMT_READ_REG32(lro, XHWICAP_CR);
		check_and_reset_firewall(lro, true);
	}

	for (i = 0; i < 4; i++) {
		val = MGMT_READ_REG32(lro, GPIO_NULL_BASE);
		check_and_reset_firewall(lro, true);
	}

	for (i = 0; i < 4; i++) {
		val = MGMT_READ_REG32(lro, AXI_GATE_BASE);
		check_and_reset_firewall(lro, true);
	}
}

/**
 * Prepare the XOCL engine for reset
 * prepare = true will call xdma_offline
 * prepare = false will call xdma online
 */
void xocl_reset(struct pci_dev *pdev, bool prepare)
{
	void (*reset)(struct pci_dev *pdev, bool prepare);
	reset = symbol_get(xocl_reset_notify);
	if (reset) {
		printk(KERN_INFO "%s: Calling xocl_reset_notify \n", DRV_NAME);
		reset(pdev, prepare);
		symbol_put(xocl_reset_notify);
	}
}


/**
 * Perform a PCIe secondary bus reset. Note: Use this method over pcie fundamental reset.
 * This method is known to work better.
 */

long reset_hot_ioctl(struct xclmgmt_dev *lro)
{
	long err = 0;
	const char *ep_name;
	struct pci_dev *pdev = lro->pci_dev;
	int retry = 0;


	if(get_dsa_version(lro) == 0x50) {
		mgmt_info(lro, "PCI Hot reset is not supported on 5.0 platforms.");
		return -EOPNOTSUPP;
	}

	BUG_ON(!pdev->bus);
	BUG_ON(!pdev->bus->self);

	if (!pdev->bus || !pdev->bus->self) {
		printk(KERN_ERR "%s: Unable to identify device root port for card %d\n", DRV_NAME,
		       lro->instance);
		err = -ENODEV;
		goto done;
	}

	ep_name = pdev->bus->name;
#if defined(__PPC64__)
	printk(KERN_INFO "%s: Ignoring reset operation for card %d in slot %s:%02x:%1x\n", DRV_NAME, lro->instance, ep_name,
	       PCI_SLOT(pdev->devfn), PCI_FUNC(pdev->devfn));
#else
	printk(KERN_INFO "%s: Trying to reset card %d in slot %s:%02x:%1x\n", DRV_NAME, lro->instance, ep_name,
	       PCI_SLOT(pdev->devfn), PCI_FUNC(pdev->devfn));


#ifdef MGMT_1PF
		printk(KERN_INFO "Hot reset cannot be performed with only mgmt PF.\n");
		return -EIO;
#endif

	if (lro->user_pci_dev) {
		/*
 		 * Make the xdma offline before issuing hot reset.
		 * assume all axi access from userpf is stopped
		 * busy lock is taken to make sure no axi access at this time
		 */
		xdma_reset(lro->user_pci_dev, true);
		xocl_reset(lro->user_pci_dev, true);
	}

	if (!lro->reset_firewall) {
		/*
		 * if reset request comes from IOCTL, reset kernel and
		 * Microblaze.
		 */
		mgmt_stop_mb(lro);

		freezeAXIGate(lro);
		msleep(500);
		freeAXIGate(lro);
		msleep(500);
	}

	/*
	 * lock pci config space access from userspace,
	 * save state and issue PCIe secondary bus reset
	 */
	xclmgmt_reset_pci(lro);

	/*
	 * Check firewall status. Status should be 0 (cleared)
	 * Otherwise issue message that a warm reboot is required.
	 */
	do {
		msleep(20);
	} while (retry++ < XCLMGMT_RESET_MAX_RETRY &&
		check_and_reset_firewall(lro, false));

	if (retry >= XCLMGMT_RESET_MAX_RETRY){
		mgmt_err(lro, "Board is not able to recover by PCI Hot reset. "
			"Please warm reboot");
		return -EIO;
	}
	
	//Also freeze and free AXI gate to reset the OCL region.
	freezeAXIGate(lro);
	msleep(500);
	freeAXIGate(lro);
	msleep(500);

	if (lro->user_pci_dev) {
		// Bring the xdma online
		xdma_reset(lro->user_pci_dev, false);
		xocl_reset(lro->user_pci_dev, false);
	}

	/*
 	 * Potential redudant stop on MB in case it was in a bad state
 	 * TODO: We should not need to reload elf, but doing anyways
 	 */
	mgmt_stop_mb(lro);
	mgmt_start_mb(lro);

#endif
done:
	return err;
}
/*
 * Inspired by GenWQE driver, card_base.c
 */
int pci_fundamental_reset(struct xclmgmt_dev *lro)
{
	int rc;
	u32 orig_mask;
	u8 hot;
	struct pci_dev *pci_dev = lro->pci_dev;

#ifdef MGMT_1PF
		printk(KERN_INFO "Fundamental reset cannot be performed with only mgmt PF.\n");
		return -ENODEV;
#endif
	// Make the user pf offline before issuing reset.
	xdma_reset(lro->user_pci_dev, true);
	xocl_reset(lro->user_pci_dev, true);

	//freeze and free AXI gate to reset the OCL region before and after the pcie reset.
	freezeAXIGate(lro);
	msleep(500);
	freeAXIGate(lro);
	msleep(500);

	/*
	 * lock pci config space access from userspace,
	 * save state and issue PCIe fundamental reset
	 */
	printk(KERN_INFO "%s: pci_fundamental_reset \n", DRV_NAME);

	// Save pci config space for botht the pf's
	pci_cfg_access_lock(pci_dev);
	pci_save_state(pci_dev);
	pci_cfg_access_lock(lro->user_pci_dev);
	pci_save_state(lro->user_pci_dev);

	rc = pcie_mask_surprise_down(pci_dev, &orig_mask);
	if (rc)
		goto done;
	printk(KERN_INFO "%s: pci_fundamental_reset 1\n", DRV_NAME);

#if defined(__PPC64__)
	/*
	 * On PPC64LE use pcie_warm_reset which will cause the FPGA to
	 * reload from PROM
	 */
	rc = pci_set_pcie_reset_state(pci_dev, pcie_warm_reset);
	if (rc)
		goto done;
	/* keep PCIe reset asserted for 250ms */
	msleep(250);
	rc = pci_set_pcie_reset_state(pci_dev, pcie_deassert_reset);
	if (rc)
		goto done;
	/* Wait for 2s to reload flash and train the link */
	msleep(2000);
#else
	/*
	 * On x86_64 use special bitstream sequence which forces the FPGA to
	 * reload from PROM
	 */
	rc = mgmt_load_reset_mini_bitstream(lro);
	if (rc)
		goto done;

	printk(KERN_INFO "%s: pci_fundamental_reset 2\n", DRV_NAME);
	/* Now perform secondary bus reset which should reset most of the device */
	pci_read_config_byte(pci_dev->bus->self, PCI_MIN_GNT, &hot);
	/* Toggle the PCIe hot reset bit in the root port */
	pci_write_config_byte(pci_dev->bus->self, PCI_MIN_GNT, hot | 0x40);
	msleep(500);
	pci_write_config_byte(pci_dev->bus->self, PCI_MIN_GNT, hot);
	msleep(500);
#endif
done:
	printk(KERN_INFO "%s: pci_fundamental_reset done routine\n", DRV_NAME);

	// restore pci config space for botht the pf's
	pci_restore_state(pci_dev);
	rc = pcie_unmask_surprise_down(pci_dev, orig_mask);
	pci_cfg_access_unlock(pci_dev);
	pci_restore_state(lro->user_pci_dev);
	pci_cfg_access_unlock(lro->user_pci_dev);

	//Also freeze and free AXI gate to reset the OCL region.
	freezeAXIGate(lro);
	msleep(500);
	freeAXIGate(lro);
	msleep(500);

	// Bring the user pf online
	xdma_reset(lro->user_pci_dev, false);
	xocl_reset(lro->user_pci_dev, false);

	// TODO: Figure out a way to reinit DMA engine which is other PF
	//if (!rc)
	//	rc = reinit(lro);
	return rc;
}

bool is_mb_power(const struct xclmgmt_dev *lro)
{
	if (lro->header.FeatureBitMap & BOARD_MGMT_ENBLD) {
		printk(KERN_INFO "%s Microblaze power monitoring supported. \n", DRV_NAME);
		return 1;
	}
	else {
		printk(KERN_INFO "%s  Microblaze power monitoring not supported. \n", DRV_NAME);
		return 0;
	}
}

/*
 * While the drivers are being developed, I want to keep the printks.
 * Once it stabilized, I think we can consolidate is_ultrascale and
 * is_ultrascale_plus in a single function.
 */
bool is_ultrascale(const struct xclmgmt_dev *lro)
{
	if (((lro->pci_dev->device >> 13) & 0x0007) == 0x0006) {
		printk(KERN_INFO "%s Ultrascale device detected. \n", DRV_NAME);
		return 1;
	}
	else {
		printk(KERN_INFO "%s Ultrascale device not detected. \n", DRV_NAME);
		return 0;
	}
}

bool is_ultrascale_plus(const struct xclmgmt_dev *lro)
{
	if (((lro->pci_dev->device >> 13) & 0x0007) == 0x0006) {
		printk(KERN_INFO "%s Ultrascale plus device detected. \n", DRV_NAME);
		return 1;
	}
	else {
		printk(KERN_INFO "%s Ultrascale plus device not detected. \n", DRV_NAME);
		return 0;
	}
}

bool is_unified(const struct xclmgmt_dev *lro)
{
	return (lro->header.FeatureBitMap & UNIFIED_PLATFORM);
}

bool is_XPR(const struct xclmgmt_dev *lro)
{
	if (((lro->pci_dev->device >> 5) & 0x0007) == 0x0000) {
		printk(KERN_INFO "%s XPR dsa detected. \n", DRV_NAME);
		return 1;
	}
	else if(is_unified(lro)){
		printk(KERN_INFO "%s Unified dsa detected. \n", DRV_NAME);
		return 1;
	}
	else {
		printk(KERN_INFO "%s XPR dsa not detected. \n", DRV_NAME);
		return 0;
	}
}

unsigned compute_unit_busy(struct xclmgmt_dev *lro)
{
	int i = 0;
	unsigned result = 0;
	u32 r = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + AXI_GATE_OFFSET_READ);

	/* r != 0x3 implies that OCL region is isolated and we cannot read CUs' status */
	if (r != 0x3)
		return 0;

	for (i = 0; i < 16; i++) {
		r = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + OCL_CTLR_BASE + i * OCL_CU_CTRL_RANGE);
		if (r == 0x1)
			result |= (r << i);
	}
	return result;
}

// Temperature in C  =   ADC  501.3743
//                     -----------------  273.6777
//                          2^bits
// Max delta with this approximation found to be ~  1.3172 C
int16_t onchip_temp(uint32_t temp)
{
	s64 t = (temp * 50138);
	t = t >> 16;
	t = t / 100;
	t = t - 274;
//	printk(KERN_INFO "temp : %x: Celsius: %lld \n", temp, t);
	return t;

}

unsigned short to_volt(uint32_t volt)
{
	unsigned short volts = ((volt * 1000 * 3) >> 16) ;
//	printk(KERN_INFO "volt : %u: Volts: %u mV\n", volt, volts);
	return volts;
}

void xclmgmt_reset_pci(struct xclmgmt_dev *lro)
{
	struct pci_dev *pdev = lro->pci_dev;
	struct pci_bus *bus;
	int i;
	u16 pci_cmd;
	u8 pci_bctl;

	mgmt_info(lro, "Reset PCI");
	
        pci_cfg_access_lock(pdev);
	pci_save_state(pdev);

	/* what if user PF in VM ? */
	if (lro->user_pci_dev) {
		pci_cfg_access_lock(lro->user_pci_dev);
		pci_save_state(lro->user_pci_dev);
	}
	/* Reset secondary bus. */
	bus = pdev->bus;
	pci_read_config_byte(bus->self, PCI_BRIDGE_CONTROL, &pci_bctl);
	pci_bctl |= PCI_BRIDGE_CTL_BUS_RESET;
	pci_write_config_byte(bus->self, PCI_BRIDGE_CONTROL, pci_bctl);

	msleep(100);
	pci_bctl &= ~PCI_BRIDGE_CTL_BUS_RESET;
	pci_write_config_byte(bus->self, PCI_BRIDGE_CONTROL, pci_bctl);

	for (i = 0; i < 5000; i++) {
		pci_read_config_word(pdev, PCI_COMMAND, &pci_cmd);
		if (pci_cmd != 0xffff)
			break;
		msleep(1);
	}

	mgmt_info(lro, "Resetting for %d ms", i); 

	if (lro->user_pci_dev) {
		pci_restore_state(lro->user_pci_dev);
		pci_cfg_access_unlock(lro->user_pci_dev);
	}

	pci_restore_state(pdev);
	pci_cfg_access_unlock(pdev);
}
