set_property PACKAGE_PIN V39 [get_ports {pin_caui4_ref_clk_n[0]}]
set_property PACKAGE_PIN V38 [get_ports {pin_caui4_ref_clk_p[0]}]
set_property PACKAGE_PIN R41 [get_ports {pin_caui4_ref_clk_n[1]}]
set_property PACKAGE_PIN R40 [get_ports {pin_caui4_ref_clk_p[1]}]




#timing constraints
#sys ref clk 156.25M
create_clock -period 10.000 -name pin_sys_clk_p [get_ports pin_sys_clk_p]
# pcie x8 ref clk 100M
create_clock -period 10.000 -name pin_pcie_ref_clk_p [get_ports pin_pcie_ref_clk_p]
# eth ref clk 156.25M
create_clock -period 6.200 -name pin_caui4_ref_clk_p0 [get_ports {pin_caui4_ref_clk_p[0]}]
create_clock -period 6.200 -name pin_caui4_ref_clk_p1 [get_ports {pin_caui4_ref_clk_p[1]}]

#create_generated_clock -name mmcm_drp_out0 [get_pins u_clkm_top/u_clkm/u_mmcm_drp/inst/mmcme3_adv_inst/CLKOUT0]
create_generated_clock -name mmcm_sysclk_out0 [get_pins u_clkm_top/u_clkm/u_mmcm_sys/inst/mmcme4_adv_inst/CLKOUT0]
#create_generated_clock -name mmcm_eth_out0 [get_pins u_clkm_top/u_clkm/u_mmcm_eth/inst/mmcme3_adv_inst/CLKOUT0]
create_generated_clock -name eth100g_0_txoutclk [get_nets {GEN_MAC_INST[0].u_eth100g_inst/u_eth100g/u_x100g/inst/txoutclk_out}]
create_generated_clock -name eth100g_1_txoutclk [get_nets {GEN_MAC_INST[1].u_eth100g_inst/u_eth100g/u_x100g/inst/txoutclk_out}]

######################################################################################################################
set_clock_groups -asynchronous -group [get_clocks pin_sys_clk_p]
#set_clock_groups -asynchronous -group [get_clocks mmcm_drp_out0]
set_clock_groups -asynchronous -group [get_clocks mmcm_sysclk_out0]
set_clock_groups -asynchronous -group [get_clocks pin_caui4_ref_clk_p0]
set_clock_groups -asynchronous -group [get_clocks pin_caui4_ref_clk_p1]
set_clock_groups -asynchronous -group [get_clocks eth100g_0_txoutclk]
set_clock_groups -asynchronous -group [get_clocks eth100g_1_txoutclk]




#clk to clk constraints

######################################################################################################################
#jitter constraints
set_input_jitter pin_caui4_ref_clk_p0 0.100

####################################################################################
# Constraints from file : 'hl18000100.xdc'
####################################################################################

set_input_jitter pin_caui4_ref_clk_p1 0.100
#loc
set_property LOC GTYE4_CHANNEL_X0Y29 [get_cells {GEN_MAC_INST[0].u_eth100g_inst/u_eth100g/u_x100g/inst/x100g_gt_i/inst/gen_gtwizard_gtye4_top.x100g_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[6].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[0].GTYE4_CHANNEL_PRIM_INST}]
set_property LOC GTYE4_CHANNEL_X0Y31 [get_cells {GEN_MAC_INST[0].u_eth100g_inst/u_eth100g/u_x100g/inst/x100g_gt_i/inst/gen_gtwizard_gtye4_top.x100g_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[6].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[1].GTYE4_CHANNEL_PRIM_INST}]
set_property LOC GTYE4_CHANNEL_X0Y28 [get_cells {GEN_MAC_INST[0].u_eth100g_inst/u_eth100g/u_x100g/inst/x100g_gt_i/inst/gen_gtwizard_gtye4_top.x100g_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[6].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[2].GTYE4_CHANNEL_PRIM_INST}]
set_property LOC GTYE4_CHANNEL_X0Y30 [get_cells {GEN_MAC_INST[0].u_eth100g_inst/u_eth100g/u_x100g/inst/x100g_gt_i/inst/gen_gtwizard_gtye4_top.x100g_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[6].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[3].GTYE4_CHANNEL_PRIM_INST}]
set_property LOC CMACE4_X0Y3 [get_cells -hierarchical -filter {NAME =~ GEN_MAC_INST[0].u_eth100g_inst/u_eth100g/u_x100g/inst/i_x100g_top/* && REF_NAME==CMACE4}]
#229
set_property LOC GTYE4_CHANNEL_X0Y33 [get_cells {GEN_MAC_INST[1].u_eth100g_inst/u_eth100g/u_x100g/inst/x100g_gt_i/inst/gen_gtwizard_gtye4_top.x100g_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[6].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[0].GTYE4_CHANNEL_PRIM_INST}]
set_property LOC GTYE4_CHANNEL_X0Y35 [get_cells {GEN_MAC_INST[1].u_eth100g_inst/u_eth100g/u_x100g/inst/x100g_gt_i/inst/gen_gtwizard_gtye4_top.x100g_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[6].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[1].GTYE4_CHANNEL_PRIM_INST}]
set_property LOC GTYE4_CHANNEL_X0Y32 [get_cells {GEN_MAC_INST[1].u_eth100g_inst/u_eth100g/u_x100g/inst/x100g_gt_i/inst/gen_gtwizard_gtye4_top.x100g_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[6].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[2].GTYE4_CHANNEL_PRIM_INST}]
set_property LOC GTYE4_CHANNEL_X0Y34 [get_cells {GEN_MAC_INST[1].u_eth100g_inst/u_eth100g/u_x100g/inst/x100g_gt_i/inst/gen_gtwizard_gtye4_top.x100g_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[6].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[3].GTYE4_CHANNEL_PRIM_INST}]
set_property LOC CMACE4_X0Y4 [get_cells -hierarchical -filter {NAME =~ GEN_MAC_INST[1].u_eth100g_inst/u_eth100g/u_x100g/inst/i_x100g_top/* && REF_NAME==CMACE4}]
#s

