//-------------------------------------------------------------------------------
//      Copyright (c) 2018 Huawei Technologies Co., Ltd. All Rights Reserved.
// 
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the Huawei Software License (the "License").
//      A copy of the License is located in the "LICENSE" file accompanying 
//      this file.
// 
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//      Huawei Software License for more details. 
//-------------------------------------------------------------------------------
`resetall
`timescale 1ns / 1ns
module fx300_top
                 # (
                  parameter VERNUM                            = 32'h18000100                    ,
                  parameter VERDATE                           = 32'h20181030                    ,
                  parameter VERSUB                            = 32'h06000006                    ,
                  parameter PR_VLD                            = 8'h00                           ,
                  parameter SEC0_VLD                          = 8'ha5                           ,
                  parameter SEC1_VLD                          = 8'ha5                           ,
                  parameter SEU_VLD                           = 8'h00                           ,
                  parameter WAR_VLD                           = 8'ha5                           ,
                  parameter CLKM_CM_ID                        = 12'h002                         ,
                  parameter BASE_CM_ID                        = 12'h003                         ,
                  parameter FEATURE_CM_ID                     = 12'h0d4                         ,
                  parameter PCIE_TOP_CM_ID                    = 12'h00c                         ,
                  parameter PCIE_TEST_CM_ID                   = 12'h00d                         ,
                  parameter ETH_100G0_CM_ID                   = 12'h0d0                         ,
                  parameter ETH_PKT_GEN_ID                    = 12'h090                         ,
                  parameter DRP_CM_ID                         = 12'h00f                         ,
                  parameter HPI_CM_ID                         = 12'h00B                         ,
                  parameter SPI_CM_ID                         = 12'h00E                         ,
                  parameter IIC_M0_ID                         = 12'h010                         ,
                  parameter IIC_M1_ID                         = 12'h011                         ,
                  parameter IIC_M2_ID                         = 12'h012                         ,
                  parameter IIC_M3_ID                         = 12'h013                         ,
                  parameter IIC_S0_ID                         = 12'h015                         ,
                  parameter DDRA_ID                           = 12'h060                         ,
                  parameter DDRB_ID                           = 12'h061                         ,
                  parameter VF_NUM                            = 8                               ,
                  parameter VFX_BARX_BASE_ADDR_WTH            = 12                              ,
                  parameter VFX_BAR0_BASE_ADDR                ={12'h00B                         ,//VF7_BAR0 base addr, 0x00B0_0000
                                                                12'h00A                         ,//VF6_BAR0 base addr, 0x00A0_0000
                                                                12'h009                         ,//VF5_BAR0 base addr, 0x0090_0000
                                                                12'h008                         ,//VF4_BAR0 base addr, 0x0080_0000
                                                                12'h007                         ,//VF3_BAR0 base addr, 0x0070_0000
                                                                12'h006                         ,//VF2_BAR0 base addr, 0x0060_0000
                                                                12'h005                         ,//VF1_BAR0 base addr, 0x0050_0000
                                                                12'h004}                        ,//VF0_BAR0 base addr, 0x0040_0000
                  parameter MMCM_RST_W                        = 17                              ,
                  parameter RST_DIV_W                         = 10                              ,
                  parameter A                                 = 14                              ,
                  parameter GTY_MAC_NUM                       = 2                               ,
                  parameter GTY_LANE_NUM                      = GTY_MAC_NUM*4                   ,
                  parameter HPI_A_WTH                         = 23                              ,
                  parameter A_WTH                             = 24                              ,
                  parameter D_WTH                             = 32                              ,
                  parameter CLK_ARRY_NUM                      = 1                               ,
                  parameter DATA_WIDTH                        = 512                             ,
                  parameter MODE_WIDTH                        = 6                               ,
                  parameter DATA_REV_WIDTH                    = 20                              ,
                  parameter PTRC_WIDTH                        = 144                             ,
                  parameter PTRD_WIDTH                        = 540                             ,
                  parameter DDRC_ECC_ENABLE                   = 1                               ,
                  parameter DDRC_DATA_MASK_ENABLE             = 0                               ,//"1"-enable data mask;"0"-disable data mask
                  parameter DDRC_GRP0_USER_NUM                = 2                               ,//group0 user num,write or read channel
                  parameter DDRC_GRP1_USER_NUM                = 2                               ,//group1 user num,read channel
                  parameter DDRC_USER_TYPE_BW                 = 4                               ,//user write or read command width,maximumn to 4
                  parameter DDRC_USER_LEN_BW                  = 8                               ,//user read/write length bit width at mbus side.
                  parameter DDRC_USER_ORDER_LEN               = 4                               ,//user order info len,maximumn to 8bit
                  parameter DDRC_USER_BP_ENABLE               = 1                               ,//dropping relative packages when user back press. "0"-not support; "1"-support
                  parameter DDRC_USER_CFIFO_DPH_BW            = 5                               ,//user command fifo depth bit width.
                  parameter DDRC_USER_DFIFO_DPH_BW            = DDRC_USER_LEN_BW + 1            ,//user data fifo depth bit width.
                  parameter DDRC_USER_CFIFO_AFULL_OVFL_THD    = 2**DDRC_USER_CFIFO_DPH_BW - 15  ,
                  parameter DDRC_USER_DFIFO_AFULL_OVFL_THD    = 2**DDRC_USER_LEN_BW             ,//user data fifo amost_full threshold.
                  parameter DDRC_COL_WIDTH                    = 10                              ,//column address width
                  parameter DDRC_ROW_WIDTH                    = 16                              ,//row address width
                  parameter DDRC_BA_WIDTH                     = 2                               ,//bank address width
                  parameter DDRC_BG_WIDTH                     = 2                               ,//bank group address width
                  parameter DDRC_ADDR_WIDTH                   = 17                              ,
                  parameter DDRC_CKE_WIDTH                    = 2                               ,
                  parameter DDRC_CK_WIDTH                     = 1                               ,
                  parameter DDRC_CS_WIDTH                     = 2                               ,
                  parameter DDRC_ODT_WIDTH                    = 2                               ,
                  parameter DDRC_DQ_WIDTH                     = 64                              ,
                  parameter DDRC_DM_WIDTH                     = DDRC_DQ_WIDTH/8                 ,
		          parameter DDRC_ECC_DQ_WIDTH                 = 8                               ,//when DATA_MASK_ENABLE=1,ECC_DQ_WIDTH do not use
		          parameter DDRC_ECC_DM_WIDTH                 = DDRC_ECC_DQ_WIDTH/8             ,//when DATA_MASK_ENABLE=1,ECC_DM_WIDTH do not use
                  parameter DDRC_USER_DATA_BW                 = DDRC_DQ_WIDTH*8                 ,//user data width
                  parameter DDRC_USER_MASK_BW                 = DDRC_DM_WIDTH*8                 ,//user data mask width
				  parameter PAR_DATA_WIDTH                    = (DATA_WIDTH +  MODE_WIDTH + DATA_REV_WIDTH + 2)                                         ,
                  parameter DDRC_USER_WDATA_BW                = (DDRC_DATA_MASK_ENABLE == 1) ? (DDRC_USER_MASK_BW+DDRC_USER_DATA_BW) : DDRC_USER_DATA_BW,
                  parameter DDRC_USER_RDATA_BW                = DDRC_USER_DATA_BW+DDRC_USER_ORDER_LEN+6                                                 ,
                  parameter DDRC_USER_ADDR_BW                 = DDRC_ROW_WIDTH+DDRC_BA_WIDTH+DDRC_COL_WIDTH-3+DDRC_BG_WIDTH                             ,
                  parameter DDRC_USER_CMD_BW                  = DDRC_USER_ORDER_LEN+DDRC_USER_ADDR_BW+DDRC_USER_LEN_BW+DDRC_USER_TYPE_BW                ,
                  parameter DDRC_GRP0_CMD_VECTOR_BW           = DDRC_USER_CMD_BW*DDRC_GRP0_USER_NUM                                                     ,
                  parameter DDRC_GRP1_CMD_VECTOR_BW           = DDRC_USER_CMD_BW*DDRC_GRP1_USER_NUM                                                     ,
                  parameter DDRC_GRP0_DATA_VECTOR_BW          = DDRC_USER_WDATA_BW*DDRC_GRP0_USER_NUM                                                   ,
                  parameter DDRC_TOTAL_DQ_WIDTH               = (DDRC_ECC_ENABLE == 1) ? (DDRC_DQ_WIDTH+DDRC_ECC_DQ_WIDTH) : DDRC_DQ_WIDTH              ,
                  parameter DDRC_TOTAL_DM_WIDTH               = (DDRC_ECC_ENABLE == 1) ? (DDRC_DM_WIDTH+DDRC_ECC_DM_WIDTH) : DDRC_DM_WIDTH
                  )
                  (
				  //clk rst
                  input                      pin_sys_clk_p            ,
                  input                      pin_sys_clk_n            ,
                  input                      pin_reset_n              ,
				  //led etc signal
                  output                     pin_fpga_act_led         ,
                  output                     pin_program_inband       ,
                  output                     pin_sta_secure_rst_n     ,
                  output                     pin_sta_secure_scl       ,
                  inout                      pin_sta_secure_sda       ,
				  //fpga2e2prom interface
                  output                     pin_fpga2e2p_i2c_wp      ,
                  output                     pin_fpga2e2p_i2c_scl     ,
                  inout                      pin_fpga2e2p_i2c_sda     ,				  
				  //fpga2mcu interface
                  output                     pin_fpga2mcu_i2c_scl     ,
                  inout                      pin_fpga2mcu_i2c_sda     ,
                  inout                      pin_mcu2fpga_i2c_scl     ,
                  inout                      pin_mcu2fpga_i2c_sda     ,
				  //interface with led
                  output                     pin_opt0_act_led         ,
                  output                     pin_opt0_lnk_led         ,
                  output                     pin_opt1_act_led         ,
                  output                     pin_opt1_lnk_led         ,
                  //interface with regbase
				  input                      pin_opt0_25g_rx_los0     ,
                  input                      pin_opt0_25g_mod_abs_def0,
                  output                     pin_opt0_25g_tx_dis0     ,
                  input                      pin_opt1_25g_rx_los0     ,
                  input                      pin_opt1_25g_mod_abs_def0,
                  output                     pin_opt1_25g_tx_dis0     ,
                  output                     pin_dync_secure_rst_n    ,
                  output                     pin_dync_secure_scl      ,
                  inout                      pin_dync_secure_sda      ,
                  input                      pin_fpga2mcu_rsv0        ,
                  output                     pin_fpga2mcu_rsv1        ,
                  input                      pin_pcie_ref_clk_p       ,
                  input                      pin_pcie_ref_clk_n       ,
                  input  [15:0]              pin_pcie_rxn_in          ,
                  input  [15:0]              pin_pcie_rxp_in          ,
                  output [15:0]              pin_pcie_txn_out         ,
                  output [15:0]              pin_pcie_txp_out         ,
				  //interface with ddr
                  input                      ddra_100m_ref_clk_p      ,
                  input                      ddra_100m_ref_clk_n      ,
                  input                      ddrb_100m_ref_clk_p      ,
                  input                      ddrb_100m_ref_clk_n      ,				  
                  output                     ddra_72b_act_n           ,
                  output [16:0]              ddra_72b_addr            ,
                  output [1:0]               ddra_72b_ba              ,
                  output [1:0]               ddra_72b_bg              ,
                  output [1:0]               ddra_72b_cke             ,
                  output [1:0]               ddra_72b_odt             ,
                  output [1:0]               ddra_72b_cs_n            ,
                  output                     ddra_72b_ck_t            ,
                  output                     ddra_72b_ck_c            ,
                  output                     ddra_72b_rst_n           ,
                  inout  [71:0]              ddra_72b_dq              ,
                  inout  [8:0]               ddra_72b_dqs_t           ,
                  inout  [8:0]               ddra_72b_dqs_c           ,
                  inout  [8:0]               ddra_72b_dm_dbi_n        ,                 
                  output                     ddrb_72b_act_n           ,
                  output [16:0]              ddrb_72b_addr            ,
                  output [1:0]               ddrb_72b_ba              ,
                  output [1:0]               ddrb_72b_bg              ,
                  output [1:0]               ddrb_72b_cke             ,
                  output [1:0]               ddrb_72b_odt             ,
                  output [1:0]               ddrb_72b_cs_n            ,
                  output                     ddrb_72b_ck_t            ,
                  output                     ddrb_72b_ck_c            ,
                  output                     ddrb_72b_rst_n           ,
                  inout  [71:0]              ddrb_72b_dq              ,
                  inout  [8:0]               ddrb_72b_dqs_t           ,
                  inout  [8:0]               ddrb_72b_dqs_c           ,
                  inout  [8:0]               ddrb_72b_dm_dbi_n        ,
				  //interface with eth100g
                  input  [GTY_MAC_NUM-1:0]   pin_caui4_ref_clk_p      ,
                  input  [GTY_MAC_NUM-1:0]   pin_caui4_ref_clk_n      ,
                  input  [GTY_MAC_NUM*4-1:0] pin_caui4_rx_n           ,
                  input  [GTY_MAC_NUM*4-1:0] pin_caui4_rx_p           ,
                  output [GTY_MAC_NUM*4-1:0] pin_caui4_tx_n           ,
                  output [GTY_MAC_NUM*4-1:0] pin_caui4_tx_p            
                  );

endmodule

