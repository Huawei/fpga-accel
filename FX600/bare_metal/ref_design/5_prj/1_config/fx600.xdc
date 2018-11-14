#pin constraints
set_property PACKAGE_PIN AR26 [get_ports pin_reset_n]
#set_property PACKAGE_PIN AT22 [get_ports pin_reset_sfp0_n]
#set_property PACKAGE_PIN AR22 [get_ports pin_reset_sfp1_n]
#set_property PACKAGE_PIN AT23 [get_ports pin_reset_cxp0_n]
#set_property PACKAGE_PIN AR23 [get_ports pin_reset_cxp1_n]
#set_property PACKAGE_PIN AU24 [get_ports pin_reset_cxp2_n]
#set_property PACKAGE_PIN AT24 [get_ports pin_reset_9485_n]

set_property PACKAGE_PIN AY23 [get_ports pin_sys_clk_p]
set_property PACKAGE_PIN BA23 [get_ports pin_sys_clk_n]

set_property PACKAGE_PIN AP11 [get_ports pin_pcie_ref_clk_p]
set_property PACKAGE_PIN AP10 [get_ports pin_pcie_ref_clk_n]

set_property PACKAGE_PIN M11 [get_ports pin_caui4_ref_clk_p[0]]
set_property PACKAGE_PIN M10 [get_ports pin_caui4_ref_clk_n[0]]

set_property PACKAGE_PIN H11 [get_ports pin_caui4_ref_clk_p[1]]
set_property PACKAGE_PIN H10 [get_ports pin_caui4_ref_clk_n[1]]

set_property PACKAGE_PIN BF23     [get_ports bmc2fpga_i2c_sda]
set_property PACKAGE_PIN BF24     [get_ports bmc2fpga_i2c_scl]
set_property PACKAGE_PIN AV22     [get_ports i2c_fpga_m_scl]
set_property PACKAGE_PIN BF22     [get_ports i2c_fpga_m_sda]
set_property PACKAGE_PIN BE8     [get_ports mac_scl]
set_property PACKAGE_PIN BF9     [get_ports mac_sda]
set_property PACKAGE_PIN BF7     [get_ports sta_secure_scl]
set_property PACKAGE_PIN BE7     [get_ports sta_secure_sda]
set_property PACKAGE_PIN D14    [get_ports dync_secure_scl]
set_property PACKAGE_PIN C17     [get_ports dync_secure_sda]
set_property PACKAGE_PIN BF8    [get_ports rst_sec_sta]
set_property PACKAGE_PIN L15    [get_ports rst_sec_dync]
set_property PACKAGE_PIN AM24    [get_ports fpga_mcu_rsv_dri]
set_property PACKAGE_PIN AP23    [get_ports program_inband]
set_property PACKAGE_PIN BF10    [get_ports mac_wp]
set_property PACKAGE_PIN BD23    [get_ports fpga_act]

set_property IOSTANDARD  LVCMOS18 [get_ports bmc2fpga_i2c_sda]
set_property IOSTANDARD  LVCMOS18 [get_ports bmc2fpga_i2c_scl]
set_property IOSTANDARD  LVCMOS18 [get_ports i2c_fpga_m_scl]
set_property IOSTANDARD  LVCMOS18 [get_ports i2c_fpga_m_sda]
set_property IOSTANDARD  LVCMOS18 [get_ports mac_scl]
set_property IOSTANDARD  LVCMOS18 [get_ports mac_sda]
set_property IOSTANDARD  LVCMOS18 [get_ports sta_secure_scl]
set_property IOSTANDARD  LVCMOS18 [get_ports sta_secure_sda]
set_property IOSTANDARD  LVCMOS12 [get_ports dync_secure_scl]
set_property IOSTANDARD  LVCMOS12 [get_ports dync_secure_sda]
set_property IOSTANDARD  LVCMOS12 [get_ports rst_sec_dync]
set_property IOSTANDARD  LVCMOS18 [get_ports rst_sec_sta]
set_property IOSTANDARD  LVCMOS18 [get_ports fpga_mcu_rsv_dri]
set_property IOSTANDARD  LVCMOS18 [get_ports program_inband]
set_property IOSTANDARD  LVCMOS18 [get_ports mac_wp]
set_property IOSTANDARD  LVCMOS18 [get_ports fpga_act]

set_property DRIVE 8 [get_ports dync_secure_sda]

set_property IOSTANDARD LVDS [get_ports pin_sys_clk_p]
set_property IOSTANDARD LVDS [get_ports pin_sys_clk_n]

set_property PACKAGE_PIN BD24     [get_ports qsfp28a_lpmode]
set_property IOSTANDARD  LVCMOS18 [get_ports qsfp28a_lpmode]
set_property PACKAGE_PIN AU24     [get_ports qsfp28b_lpmode]
set_property IOSTANDARD  LVCMOS18 [get_ports qsfp28b_lpmode]
set_property PACKAGE_PIN AT22     [get_ports qsfp28a_rst]
set_property IOSTANDARD  LVCMOS18 [get_ports qsfp28a_rst]

set_property PACKAGE_PIN AR22     [get_ports qsfp28b_rst]
set_property IOSTANDARD  LVCMOS18 [get_ports qsfp28b_rst]
set_property PACKAGE_PIN AL21     [get_ports qsfp28b_act_link]
set_property IOSTANDARD  LVCMOS18 [get_ports qsfp28b_act_link]

set_property PACKAGE_PIN AL24     [get_ports qsfp28a_act_link]
set_property IOSTANDARD  LVCMOS18 [get_ports qsfp28a_act_link]

######################################################################################################################
#100g constraints
######################################################################################################################
set_property LOC GTYE4_CHANNEL_X1Y48 [get_cells {GEN_MAC_INST[0].u_eth100g_inst/u_eth100g/u_x100g/inst/x100g_gt_i/inst/gen_gtwizard_gtye4_top.x100g_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[35].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[0].GTYE4_CHANNEL_PRIM_INST}]
set_property LOC GTYE4_CHANNEL_X1Y49 [get_cells {GEN_MAC_INST[0].u_eth100g_inst/u_eth100g/u_x100g/inst/x100g_gt_i/inst/gen_gtwizard_gtye4_top.x100g_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[35].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[1].GTYE4_CHANNEL_PRIM_INST}]
set_property LOC GTYE4_CHANNEL_X1Y50 [get_cells {GEN_MAC_INST[0].u_eth100g_inst/u_eth100g/u_x100g/inst/x100g_gt_i/inst/gen_gtwizard_gtye4_top.x100g_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[35].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[2].GTYE4_CHANNEL_PRIM_INST}]
set_property LOC GTYE4_CHANNEL_X1Y51 [get_cells {GEN_MAC_INST[0].u_eth100g_inst/u_eth100g/u_x100g/inst/x100g_gt_i/inst/gen_gtwizard_gtye4_top.x100g_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[35].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[3].GTYE4_CHANNEL_PRIM_INST}]
set_property LOC CMACE4_X0Y7 [get_cells -hierarchical -filter {NAME =~ GEN_MAC_INST[0].u_eth100g_inst/u_eth100g/u_x100g/inst/i_x100g_top/* && REF_NAME==CMACE4}]
set_property LOC GTYE4_CHANNEL_X1Y52 [get_cells {GEN_MAC_INST[1].u_eth100g_inst/u_eth100g/u_x100g/inst/x100g_gt_i/inst/gen_gtwizard_gtye4_top.x100g_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[35].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[0].GTYE4_CHANNEL_PRIM_INST}]
set_property LOC GTYE4_CHANNEL_X1Y53 [get_cells {GEN_MAC_INST[1].u_eth100g_inst/u_eth100g/u_x100g/inst/x100g_gt_i/inst/gen_gtwizard_gtye4_top.x100g_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[35].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[1].GTYE4_CHANNEL_PRIM_INST}]
set_property LOC GTYE4_CHANNEL_X1Y54 [get_cells {GEN_MAC_INST[1].u_eth100g_inst/u_eth100g/u_x100g/inst/x100g_gt_i/inst/gen_gtwizard_gtye4_top.x100g_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[35].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[2].GTYE4_CHANNEL_PRIM_INST}]
set_property LOC GTYE4_CHANNEL_X1Y55 [get_cells {GEN_MAC_INST[1].u_eth100g_inst/u_eth100g/u_x100g/inst/x100g_gt_i/inst/gen_gtwizard_gtye4_top.x100g_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[35].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[3].GTYE4_CHANNEL_PRIM_INST}]
set_property LOC CMACE4_X0Y8 [get_cells -hierarchical -filter {NAME =~ GEN_MAC_INST[1].u_eth100g_inst/u_eth100g/u_x100g/inst/i_x100g_top/* && REF_NAME==CMACE4}]
create_pblock pblock_U_100G_PB
resize_pblock pblock_U_100G_PB -add CLOCKREGION_X0Y10:CLOCKREGION_X5Y14
add_cells_to_pblock pblock_U_100G_PB [get_cells [list GEN_MAC_INST[0].u_eth100g_inst GEN_MAC_INST[0].u_pkt_gen_inst GEN_MAC_INST[1].u_eth100g_inst GEN_MAC_INST[0].u_pkt_gen_inst GEN_MAC_INST1[8].u_eth100g_inst GEN_MAC_INST1[8].u_pkt_gen_inst ] ]
remove_cells_from_pblock pblock_U_100G_PB [get_cells GEN_MAC_INST1[8].u_eth100g_inst/u_eth100g/u_x100g] 

create_clock -period 10.000 -name pin_sys_clk_p [get_ports pin_sys_clk_p]
create_clock -period 10.000 -name pin_pcie_ref_clk_p [get_ports pin_pcie_ref_clk_p]
create_clock -period 6.200 -name pin_caui4_ref_clk_p0 [get_ports pin_caui4_ref_clk_p[0]]
create_clock -period 6.200 -name pin_caui4_ref_clk_p1 [get_ports pin_caui4_ref_clk_p[1]]

create_generated_clock -name mmcm_sysclk_out0 [get_pins u_clkm_top/u_clkm/u_mmcm_sys/inst/mmcme3_adv_inst/CLKOUT0]
create_generated_clock -name eth100g_0_txoutclk [get_nets GEN_MAC_INST[0].u_eth100g_inst/u_eth100g/u_x100g/inst/txoutclk_out] 
create_generated_clock -name eth100g_1_txoutclk [get_nets GEN_MAC_INST[1].u_eth100g_inst/u_eth100g/u_x100g/inst/txoutclk_out] 
set_clock_groups -asynchronous -group [get_clocks pin_sys_clk_p]
set_clock_groups -asynchronous -group [get_clocks mmcm_sysclk_out0]
set_clock_groups -asynchronous -group [get_clocks pin_caui4_ref_clk_p0]
set_clock_groups -asynchronous -group [get_clocks pin_caui4_ref_clk_p1]
set_clock_groups -asynchronous -group [get_clocks eth100g_0_txoutclk ]
set_clock_groups -asynchronous -group [get_clocks eth100g_1_txoutclk ]

set_multicycle_path -from [get_pins {u_hf_hpi_top/u_hpi/bar_wdata_reg*/C} ] 2 
set_multicycle_path -from [get_pins {u_hf_hpi_top/u_hpi/cpu_addr_pre_reg*/C} ] 2 

set_multicycle_path -hold 1 -from  [get_pins {u_hf_hpi_top/u_hpi/bar_wdata_reg*/C} ]  
set_multicycle_path -hold 1 -from  [get_pins {u_hf_hpi_top/u_hpi/cpu_addr_pre_reg*/C} ]  
set_false_path -from [get_clocks pin_pcie_ref_clk_p] -to [get_clocks mmcm_sysclk_out0]
set_false_path -from [get_clocks -of_objects [get_pins u_pcie_top_x16/u_pcie_ep_top_16x/u_pcie_ep/inst/gt_top_i/diablo_gt.diablo_gt_phy_wrapper/phy_clk_i/bufg_gt_userclk/O]] -to [get_clocks mmcm_sysclk_out0]

set_false_path -from [get_clocks -of_objects [get_pins u_phy_ddrc_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT5]] -to [get_clocks -of_objects [get_pins u_phy_ddrc_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT0]]
set_false_path -from [get_clocks -of_objects [get_pins u_phy_ddrc_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT6]] -to [get_clocks -of_objects [get_pins u_phy_ddrc_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT0]]
set_false_path -from [get_clocks -of_objects [get_pins u_ul_pr_top/u_phy_ddra_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT6]] -to [get_clocks -of_objects [get_pins u_ul_pr_top/u_phy_ddra_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT0]]
set_false_path -from [get_clocks -of_objects [get_pins u_ul_pr_top/u_phy_ddrb_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT6]] -to [get_clocks -of_objects [get_pins u_ul_pr_top/u_phy_ddrb_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT0]]
set_false_path -from [get_clocks -of_objects [get_pins u_ul_pr_top/u_phy_ddrd_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT6]] -to [get_clocks -of_objects [get_pins u_ul_pr_top/u_phy_ddrd_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT0]]

set_false_path -from [get_clocks -of_objects [get_pins u_phy_ddrc_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT0]] -to [get_clocks -of_objects [get_pins u_phy_ddrc_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT5]]
set_false_path -from [get_clocks -of_objects [get_pins u_phy_ddrc_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT0]] -to [get_clocks -of_objects [get_pins u_phy_ddrc_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT6]]
set_false_path -from [get_clocks -of_objects [get_pins u_ul_pr_top/u_phy_ddra_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT0]] -to [get_clocks -of_objects [get_pins u_ul_pr_top/u_phy_ddra_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT6]]
set_false_path -from [get_clocks -of_objects [get_pins u_ul_pr_top/u_phy_ddrb_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT0]] -to [get_clocks -of_objects [get_pins u_ul_pr_top/u_phy_ddrb_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT6]]
set_false_path -from [get_clocks -of_objects [get_pins u_ul_pr_top/u_phy_ddrd_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT0]] -to [get_clocks -of_objects [get_pins u_ul_pr_top/u_phy_ddrd_72b/inst/u_ddr4_infrastructure/gen_mmcme4.u_mmcme_adv_inst/CLKOUT6]]

######################################################################################################################
#jitter constraints
set_input_jitter pin_pcie_ref_clk_p  0.100
set_input_jitter pin_caui4_ref_clk_p0  0.100
set_input_jitter pin_caui4_ref_clk_p1  0.100
######################################################################################################################
#bitstream gen cfg for spi interface

set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]
set_property BITSTREAM.CONFIG.CONFIGRATE 21.3 [current_design]
set_property CONFIG_VOLTAGE 1.8 [current_design]
set_property BITSTREAM.CONFIG.SPI_32BIT_ADDR YES [current_design]
set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [current_design]
set_property BITSTREAM.CONFIG.CONFIGFALLBACK ENABLE [current_design]
set_property BITSTREAM.CONFIG.TIMER_CFG 0x06000000 [current_design]
######################################################################################################################
set_max_delay -datapath_only -from [get_cells -hier *spi_sck_reg] -to [get_pins -hier *STARTUP*_inst/USRCCLKO] 2.000 
set_max_delay -datapath_only -from [get_cells -hier *spi_mosi_reg] -to [get_pins -hier *STARTUP*_inst/DO[0]] 2.000 
set_max_delay -datapath_only -from [get_pins -hier *STARTUP*_inst/DI[1]] -to [get_cells -hier *rx_data_buf_sft_reg[0]] 2.000 

