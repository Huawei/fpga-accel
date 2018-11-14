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
module fx600_top
                 # (
                  parameter VERNUM                 = 32'h18000100       ,
                  parameter VERDATE                = 32'h20180827       ,
                  parameter VERSUB                 = 32'h07000103       ,
                  parameter CFG_VEND_ID            = 16'h19e5           ,
                  parameter CFG_DEV_ID             = 16'hd51e           ,                          
                  parameter PR_VLD                 = 8'h00              ,
                  parameter SEC0_VLD               = 8'ha5              ,
                  parameter SEC1_VLD               = 8'ha5              ,
                  parameter SEU_VLD                = 8'h00              ,
                  parameter WAR_VLD                = 8'ha5              ,
                  parameter CLKM_CM_ID             = 12'h002            , //0x00_2000~0x00_2FFF
                  parameter BASE_CM_ID             = 12'h003            , //0x00_3000~0x00_3FFF
                  parameter PCIE_CM_ID             = 12'h004            , //0x00_4000~0x00_4FFF
                  parameter ROUTE_CM_ID            = 12'h006            , //0x00_5000~0x00_5FFF
                  parameter DRP_CM_ID              = 12'h00A            , //0x00_A000~0x00_AFFF
                  parameter HPI_CM_ID              = 12'h00B            , //0x00_B000~0x00_BFFF
                  parameter SPI_CM_ID              = 12'h00E            , //0x00_E000~0x00_EFFF
                  parameter SA_ID                  = 12'h010            , //0x00_0000~0x00_0FFF
                  parameter IIC_M0_ID              = 12'h011            , //0x00_f000~0x00_fFFF
                  parameter IIC_M1_ID              = 12'h012            ,
                  parameter IIC_M2_ID              = 12'h013            ,
                  parameter IIC_M3_ID              = 12'h014            ,
                  parameter IIC_S0_ID              = 12'h015            ,
                  parameter DDRA_CM_ID             = 12'h060            , //0x01_7000~0x01_7FFF
                  parameter DDRB_CM_ID             = 12'h061            , //0x01_7000~0x01_7FFF
                  parameter DDRC_CM_ID             = 12'h062            , //0x01_7000~0x01_7FFF
                  parameter DDRD_CM_ID             = 12'h063            , //0x01_7000~0x01_7FFF
                  parameter DMAE_CM_ID             = 12'h040            ,
                  parameter TXQM_CM_ID             = 12'h041            ,
                  parameter TXM_CM_ID              = 12'h042            ,
                  parameter RXQM_CM_ID             = 12'h043            ,
                  parameter RXM_CM_ID              = 12'h044            ,
                  parameter FEATURE_CM_ID          = 12'h0d4            ,
                  parameter PKT_SCH_ID             = 12'h090            , //0x00_0000~0x00_0FFF
                  parameter MBX_CM_ID              = 12'h000            , //0x00_0000~0x00_0FFF
                  parameter MBX_VF0_ID             = 12'h100            , //0x10_0000~0x10_0FFF
                  parameter MBX_VF1_ID             = 12'h140            , //0x14_0000~0x14_0FFF
                  parameter MBX_VF2_ID             = 12'h180            , //0x18_0000~0x18_0FFF
                  parameter MBX_VF3_ID             = 12'h1C0            , //0x1C_0000~0x1C_0FFF
                  parameter MBX_VF4_ID             = 12'h200            , //0x20_0000~0x20_0FFF
                  parameter MBX_VF5_ID             = 12'h240            , //0x24_0000~0x24_0FFF
                  parameter MBX_VF6_ID             = 12'h280            , //0x28_0000~0x28_0FFF
                  parameter MBX_VF7_ID             = 12'h2C0            , //0x2C_0000~0x2C_0FFF
                  parameter ETH_100G0_CM_ID        = 12'h0d0            ,
                  parameter TXQM_FN0_ID            = 12'h0C6            ,
                  parameter TXQM_FN1_ID            = 12'h106            ,
                  parameter TXM_FN1_ID             = 12'h107            ,
                  parameter RXQM_FN0_ID            = 12'h0C8            ,
                  parameter RXQM_FN1_ID            = 12'h108            ,
                  parameter  RXM_FN1_ID            = 12'h109            ,
                  parameter VF_NUM                 = 8                  ,
                  parameter VFX_BARX_BASE_ADDR_WTH = 12                 ,
                  parameter VFX_BAR0_BASE_ADDR     ={12'h00B            ,//VF7_BAR0 base addr, 0x00B0_0000
                                                     12'h00A            ,//VF6_BAR0 base addr, 0x00A0_0000
                                                     12'h009            ,//VF5_BAR0 base addr, 0x0090_0000
                                                     12'h008            ,//VF4_BAR0 base addr, 0x0080_0000
                                                     12'h007            ,//VF3_BAR0 base addr, 0x0070_0000
                                                     12'h006            ,//VF2_BAR0 base addr, 0x0060_0000
                                                     12'h005            ,//VF1_BAR0 base addr, 0x0050_0000
                                                     12'h004}           ,//VF0_BAR0 base addr, 0x0040_0000
                  parameter MMCM_RST_W             = 17                 ,
                  parameter RST_DIV_W              = 10                 ,
                  parameter A                      = 14                 ,
                  parameter GTY_MAC_NUM            = 2                  ,
                  parameter GTY_LANE_NUM           = GTY_MAC_NUM*4      ,
                  parameter A_WTH                  = 24                 ,
                  parameter D_WTH                  = 32                 ,
                  parameter CLK_ARRY_NUM           = 1                  ,
                  parameter DDRC_ECC_ENABLE        = 1                  ,
                  parameter DDRC_BA_WIDTH          = 2                  ,
                  parameter DDRC_BG_WIDTH          = 2                  ,
                  parameter DDRC_ADDR_WIDTH        = 17                 ,
                  parameter DDRC_CKE_WIDTH         = 2                  ,
                  parameter DDRC_CS_WIDTH          = 2                  ,
                  parameter DDRC_ODT_WIDTH         = 2                  ,
                  parameter DDRC_DQ_WIDTH          = 64                 ,
                  parameter DDRC_DM_WIDTH          = DDRC_DQ_WIDTH/8    ,
		          parameter DDRC_ECC_DQ_WIDTH      = 8                  ,
		          parameter DDRC_ECC_DM_WIDTH      = DDRC_ECC_DQ_WIDTH/8,
				  parameter DDRA_ECC_ENABLE        = 1                  , 
                  parameter DDRA_BA_WIDTH          = 2                  ,//bank address width
                  parameter DDRA_BG_WIDTH          = 2                  ,//bank group address width
                  parameter DDRA_ADDR_WIDTH        = 17                 ,
                  parameter DDRA_CKE_WIDTH         = 2                  ,
                  parameter DDRA_CS_WIDTH          = 2                  ,
                  parameter DDRA_ODT_WIDTH         = 2                  ,
                  parameter DDRA_DQ_WIDTH          = 64                 ,
                  parameter DDRA_DM_WIDTH          = DDRA_DQ_WIDTH/8    ,
                  parameter	DDRA_ECC_DQ_WIDTH      = 8                  ,
                  parameter	DDRA_ECC_DM_WIDTH      = DDRA_ECC_DQ_WIDTH/8,
                  parameter DDRB_ECC_ENABLE        = 1                  ,
                  parameter DDRB_BA_WIDTH          = 2                  ,
                  parameter DDRB_BG_WIDTH          = 2                  ,
                  parameter DDRB_ADDR_WIDTH        = 17                 ,
                  parameter DDRB_CKE_WIDTH         = 2                  ,
                  parameter DDRB_CS_WIDTH          = 2                  ,
                  parameter DDRB_ODT_WIDTH         = 2                  ,
                  parameter DDRB_DQ_WIDTH          = 64                 ,
                  parameter DDRB_DM_WIDTH          = DDRB_DQ_WIDTH/8    ,
				  parameter DDRB_ECC_DM_WIDTH      = DDRB_ECC_DQ_WIDTH/8,
                  parameter DDRB_ECC_DQ_WIDTH      = 8                  ,	
                  parameter DDRD_ECC_ENABLE        = 1                  ,
                  parameter DDRD_BA_WIDTH          = 2                  ,
                  parameter DDRD_BG_WIDTH          = 2                  ,
                  parameter DDRD_ADDR_WIDTH        = 17                 ,
                  parameter DDRD_CKE_WIDTH         = 2                  ,
                  parameter DDRD_CS_WIDTH          = 2                  ,
                  parameter DDRD_ODT_WIDTH         = 2                  ,
                  parameter DDRD_DQ_WIDTH          = 64                 ,
                  parameter DDRD_DM_WIDTH          = DDRD_DQ_WIDTH/8    ,
                  parameter DDRD_ECC_DQ_WIDTH      = 8                  ,
                  parameter DDRD_ECC_DM_WIDTH      = DDRD_ECC_DQ_WIDTH/8,				  
                  parameter DDRC_TOTAL_DQ_WIDTH    = (DDRC_ECC_ENABLE == 1) ? (DDRC_DQ_WIDTH+DDRC_ECC_DQ_WIDTH) : DDRC_DQ_WIDTH,
                  parameter DDRC_TOTAL_DM_WIDTH    = (DDRC_ECC_ENABLE == 1) ? (DDRC_DM_WIDTH+DDRC_ECC_DM_WIDTH) : DDRC_DM_WIDTH,
                  parameter DDRA_TOTAL_DQ_WIDTH    = (DDRA_ECC_ENABLE == 1) ? (DDRA_DQ_WIDTH+DDRA_ECC_DQ_WIDTH) : DDRA_DQ_WIDTH,
                  parameter DDRA_TOTAL_DM_WIDTH    = (DDRA_ECC_ENABLE == 1) ? (DDRA_DM_WIDTH+DDRA_ECC_DM_WIDTH) : DDRA_DM_WIDTH,                
                  parameter DDRB_TOTAL_DQ_WIDTH    = (DDRB_ECC_ENABLE == 1) ? (DDRB_DQ_WIDTH+DDRB_ECC_DQ_WIDTH) : DDRB_DQ_WIDTH,
                  parameter DDRB_TOTAL_DM_WIDTH    = (DDRB_ECC_ENABLE == 1) ? (DDRB_DM_WIDTH+DDRB_ECC_DM_WIDTH) : DDRB_DM_WIDTH,               
                  parameter DDRD_TOTAL_DQ_WIDTH    = (DDRD_ECC_ENABLE == 1) ? (DDRD_DQ_WIDTH+DDRD_ECC_DQ_WIDTH) : DDRD_DQ_WIDTH,
                  parameter DDRD_TOTAL_DM_WIDTH    = (DDRD_ECC_ENABLE == 1) ? (DDRD_DM_WIDTH+DDRD_ECC_DM_WIDTH) : DDRD_DM_WIDTH
                  )
                  (
				  //clk and rst signal
                  input                           pin_sys_clk_p          ,
                  input                           pin_sys_clk_n          ,
                  input                           pin_reset_n            ,
				  //interface with pcie
                  input                           pin_pcie_ref_clk_p     ,
                  input                           pin_pcie_ref_clk_n     ,
                  input       [15:0]              pin_pcie_rxn_in        ,
                  input       [15:0]              pin_pcie_rxp_in        ,
                  output      [15:0]              pin_pcie_txn_out       ,
                  output      [15:0]              pin_pcie_txp_out       ,  
				  //interface with ddr			  
                  input                           ddra_100m_ref_clk_p    ,
                  input                           ddra_100m_ref_clk_n    ,
                  input                           ddrb_100m_ref_clk_p    ,
                  input                           ddrb_100m_ref_clk_n    ,
                  input                           ddrc_100m_ref_clk_p    ,
                  input                           ddrc_100m_ref_clk_n    ,
                  input                           ddrd_100m_ref_clk_p    ,
                  input                           ddrd_100m_ref_clk_n    ,        
                  output                          ddra_72b_act_n         ,
                  output      [16:0]              ddra_72b_addr          ,
                  output      [1:0]               ddra_72b_ba            ,
                  output      [1:0]               ddra_72b_bg            ,
                  output      [1:0]               ddra_72b_cke           ,
                  output      [1:0]               ddra_72b_odt           ,
                  output      [1:0]               ddra_72b_cs_n          ,
                  output                          ddra_72b_ck_t          ,
                  output                          ddra_72b_ck_c          ,
                  output                          ddra_72b_rst_n         ,
                  output                          ddra_72b_par           ,
                  inout       [71:0]              ddra_72b_dq            ,
                  inout       [8:0]               ddra_72b_dqs_t         ,
                  inout       [8:0]               ddra_72b_dqs_c         ,
                  inout       [8:0]               ddra_72b_dm_dbi_n      ,                
                  output                          ddrb_72b_act_n         ,
                  output      [16:0]              ddrb_72b_addr          ,
                  output      [1:0]               ddrb_72b_ba            ,
                  output      [1:0]               ddrb_72b_bg            ,
                  output      [1:0]               ddrb_72b_cke           ,
                  output      [1:0]               ddrb_72b_odt           ,
                  output      [1:0]               ddrb_72b_cs_n          ,
                  output                          ddrb_72b_ck_t          ,
                  output                          ddrb_72b_ck_c          ,
                  output                          ddrb_72b_rst_n         ,
                  output                          ddrb_72b_par           ,
                  inout       [71:0]              ddrb_72b_dq            ,
                  inout       [8:0]               ddrb_72b_dqs_t         ,
                  inout       [8:0]               ddrb_72b_dqs_c         ,
                  inout       [8:0]               ddrb_72b_dm_dbi_n      ,                 
                  output                          ddrc_72b_act_n         ,
                  output      [16:0]              ddrc_72b_addr          ,
                  output      [1:0]               ddrc_72b_ba            ,
                  output      [1:0]               ddrc_72b_bg            ,
                  output      [1:0]               ddrc_72b_cke           ,
                  output      [1:0]               ddrc_72b_odt           ,
                  output      [1:0]               ddrc_72b_cs_n          ,
                  output                          ddrc_72b_ck_t          ,
                  output                          ddrc_72b_ck_c          ,
                  output                          ddrc_72b_rst_n         ,
                  output                          ddrc_72b_par           ,
                  inout       [71:0]              ddrc_72b_dq            ,
                  inout       [8:0]               ddrc_72b_dqs_t         ,
                  inout       [8:0]               ddrc_72b_dqs_c         ,
                  inout       [8:0]               ddrc_72b_dm_dbi_n      ,                  
                  output                          ddrd_72b_act_n         ,
                  output      [16:0]              ddrd_72b_addr          ,
                  output      [1:0]               ddrd_72b_ba            ,
                  output      [1:0]               ddrd_72b_bg            ,
                  output      [1:0]               ddrd_72b_cke           ,
                  output      [1:0]               ddrd_72b_odt           ,
                  output      [1:0]               ddrd_72b_cs_n          ,
                  output                          ddrd_72b_ck_t          ,
                  output                          ddrd_72b_ck_c          ,
                  output                          ddrd_72b_rst_n         ,
                  output                          ddrd_72b_par           ,
                  inout       [71:0]              ddrd_72b_dq            ,
                  inout       [8:0]               ddrd_72b_dqs_t         ,
                  inout       [8:0]               ddrd_72b_dqs_c         ,
                  inout       [8:0]               ddrd_72b_dm_dbi_n      ,
				  //interface with eth_40g
                  input       [GTY_MAC_NUM-1:0]   pin_caui4_ref_clk_p    ,
                  input       [GTY_MAC_NUM-1:0]   pin_caui4_ref_clk_n    ,
                  input       [GTY_MAC_NUM*4-1:0] pin_caui4_rx_n         ,
                  input       [GTY_MAC_NUM*4-1:0] pin_caui4_rx_p         ,
                  output      [GTY_MAC_NUM*4-1:0] pin_caui4_tx_n         ,
                  output      [GTY_MAC_NUM*4-1:0] pin_caui4_tx_p         ,
				  //interface with i2c
                  output                          i2c_fpga_m_scl         ,
                  inout                           i2c_fpga_m_sda         ,
                  inout                           bmc2fpga_i2c_sda       ,
                  input                           bmc2fpga_i2c_scl       ,       		   
                  output                          mac_scl                ,
                  inout                           mac_sda                ,
                  output                          mac_wp                 ,
                  output                          sta_secure_scl         ,
                  inout                           sta_secure_sda         ,
                  output                          rst_sec_sta            ,
                  output                          dync_secure_scl        ,
                  inout                           dync_secure_sda        ,
                  output                          rst_sec_dync           ,
                  output                          qsfp28a_lpmode         ,
                  output                          qsfp28b_lpmode         ,
                  output                          qsfp28a_rst            ,
                  output                          qsfp28b_rst            ,
                  output                          qsfp28a_act_link       ,
                  output                          qsfp28b_act_link       ,
                  output                          fpga_mcu_rsv_dri       ,
                  output                          program_inband         ,
                  output                          fpga_act                 
                  );

endmodule

