/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2020 MediaTek Inc.
 * Copyright (C) 2021 XiaoMi, Inc.
 */
/*! \file   soc7_0.c
*    \brief  Internal driver stack will export
*    the required procedures here for GLUE Layer.
*
*    This file contains all routines which are exported
     from MediaTek 802.11 Wireless LAN driver stack to GLUE Layer.
*/

#ifdef SOC7_0

#include "precomp.h"
#include "soc7_0.h"
#include "coda/soc7_0/wf_wfdma_host_dma0.h"
#include "coda/soc7_0/wf_pse_top.h"
#include "hal_dmashdl_soc7_0.h"

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
static void soc7_0_ConstructFirmwarePrio(struct GLUE_INFO *prGlueInfo,
	uint8_t **apucNameTable, uint8_t **apucName,
	uint8_t *pucNameIdx, uint8_t ucMaxNameIdx);

static uint8_t soc7_0SetRxRingHwAddr(struct RTMP_RX_RING *prRxRing,
		struct BUS_INFO *prBusInfo, uint32_t u4SwRingIdx);

static bool soc7_0WfdmaAllocRxRing(struct GLUE_INFO *prGlueInfo,
		bool fgAllocMem);

static void soc7_0asicConnac2xProcessTxInterrupt(
		struct ADAPTER *prAdapter);

static void soc7_0asicConnac2xProcessRxInterrupt(
	struct ADAPTER *prAdapter);

static void soc7_0asicConnac2xWfdmaManualPrefetch(
	struct GLUE_INFO *prGlueInfo);

static void soc7_0ReadIntStatus(struct ADAPTER *prAdapter,
		uint32_t *pu4IntStatus);

static void configIntMask(struct GLUE_INFO *prGlueInfo,
		u_int8_t enable);

static void soc7_0asicConnac2xWpdmaConfig(struct GLUE_INFO *prGlueInfo,
		u_int8_t enable, bool fgResetHif);

static void soc7_0EnableFwDlMode(struct ADAPTER *prAdapter);

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

struct ECO_INFO soc7_0_eco_table[] = {
	/* HW version,  ROM version,    Factory version */
	{0x00, 0x00, 0xA, 0x1},	/* E1 */
	{0x00, 0x00, 0x0, 0x0}	/* End of table */
};

uint8_t *apucsoc7_0FwName[] = {
	(uint8_t *) CFG_FW_FILENAME "_soc7_0",
	NULL
};

#if defined(_HIF_PCIE)
struct PCIE_CHIP_CR_MAPPING soc7_0_bus2chip_cr_mapping[] = {
	/* chip addr, bus addr, range */
	{0x830c0000, 0x00000, 0x1000}, /* WF_MCU_BUS_CR_REMAP */
	{0x54000000, 0x02000, 0x1000},  /* WFDMA PCIE0 MCU DMA0 */
	{0x55000000, 0x03000, 0x1000},  /* WFDMA PCIE0 MCU DMA1 */
	{0x56000000, 0x04000, 0x1000},  /* WFDMA reserved */
	{0x57000000, 0x05000, 0x1000},  /* WFDMA MCU wrap CR */
	{0x58000000, 0x06000, 0x1000},  /* WFDMA PCIE1 MCU DMA0 (MEM_DMA) */
	{0x59000000, 0x07000, 0x1000},  /* WFDMA PCIE1 MCU DMA1 */
	{0x820c0000, 0x08000, 0x4000},  /* WF_UMAC_TOP (PLE) */
	{0x820c8000, 0x0c000, 0x2000},  /* WF_UMAC_TOP (PSE) */
	{0x820cc000, 0x0e000, 0x2000},  /* WF_UMAC_TOP (PP) */
	{0x820e0000, 0x20000, 0x0400},  /* WF_LMAC_TOP BN0 (WF_CFG) */
	{0x820e1000, 0x20400, 0x0200},  /* WF_LMAC_TOP BN0 (WF_TRB) */
	{0x820e2000, 0x20800, 0x0400},  /* WF_LMAC_TOP BN0 (WF_AGG) */
	{0x820e3000, 0x20c00, 0x0400},  /* WF_LMAC_TOP BN0 (WF_ARB) */
	{0x820e4000, 0x21000, 0x0400},  /* WF_LMAC_TOP BN0 (WF_TMAC) */
	{0x820e5000, 0x21400, 0x0800},  /* WF_LMAC_TOP BN0 (WF_RMAC) */
	{0x820ce000, 0x21c00, 0x0200},  /* WF_LMAC_TOP (WF_SEC) */
	{0x820e7000, 0x21e00, 0x0200},  /* WF_LMAC_TOP BN0 (WF_DMA) */
	{0x820cf000, 0x22000, 0x1000},  /* WF_LMAC_TOP (WF_PF) */
	{0x820e9000, 0x23400, 0x0200},  /* WF_LMAC_TOP BN0 (WF_WTBLOFF) */
	{0x820ea000, 0x24000, 0x0200},  /* WF_LMAC_TOP BN0 (WF_ETBF) */
	{0x820eb000, 0x24200, 0x0400},  /* WF_LMAC_TOP BN0 (WF_LPON) */
	{0x820ec000, 0x24600, 0x0200},  /* WF_LMAC_TOP BN0 (WF_INT) */
	{0x820ed000, 0x24800, 0x0800},  /* WF_LMAC_TOP BN0 (WF_MIB) */
	{0x820ca000, 0x26000, 0x2000},  /* WF_LMAC_TOP BN0 (WF_MUCOP) */
	{0x820d0000, 0x30000, 0x10000}, /* WF_LMAC_TOP (WF_WTBLON) */
	{0x40000000, 0x70000, 0x10000}, /* WF_UMAC_SYSRAM */
	{0x00400000, 0x80000, 0x10000}, /* WF_MCU_SYSRAM */
	{0x00410000, 0x90000, 0x10000}, /* WF_MCU_SYSRAM (configure register) */
	{0x820f0000, 0xa0000, 0x0400},  /* WF_LMAC_TOP BN1 (WF_CFG) */
	{0x820f1000, 0xa0600, 0x0200},  /* WF_LMAC_TOP BN1 (WF_TRB) */
	{0x820f2000, 0xa0800, 0x0400},  /* WF_LMAC_TOP BN1 (WF_AGG) */
	{0x820f3000, 0xa0c00, 0x0400},  /* WF_LMAC_TOP BN1 (WF_ARB) */
	{0x820f4000, 0xa1000, 0x0400},  /* WF_LMAC_TOP BN1 (WF_TMAC) */
	{0x820f5000, 0xa1400, 0x0800},  /* WF_LMAC_TOP BN1 (WF_RMAC) */
	{0x820f7000, 0xa1e00, 0x0200},  /* WF_LMAC_TOP BN1 (WF_DMA) */
	{0x820f9000, 0xa3400, 0x0200},  /* WF_LMAC_TOP BN1 (WF_WTBLOFF) */
	{0x820fa000, 0xa4000, 0x0200},  /* WF_LMAC_TOP BN1 (WF_ETBF) */
	{0x820fb000, 0xa4200, 0x0400},  /* WF_LMAC_TOP BN1 (WF_LPON) */
	{0x820fc000, 0xa4600, 0x0200},  /* WF_LMAC_TOP BN1 (WF_INT) */
	{0x820fd000, 0xa4800, 0x0800},  /* WF_LMAC_TOP BN1 (WF_MIB) */
	{0x820cc000, 0xa5000, 0x2000},  /* WF_LMAC_TOP BN1 (WF_MUCOP) */
	{0x820c4000, 0xa8000, 0x4000},  /* WF_LMAC_TOP BN1 (WF_MUCOP) */
	{0x820b0000, 0xae000, 0x1000},  /* [APB2] WFSYS_ON */
	{0x80020000, 0xb0000, 0x10000}, /* WF_TOP_MISC_OFF */
	{0x81020000, 0xc0000, 0x10000}, /* WF_TOP_MISC_ON */
	{0x7c020000, 0xd0000, 0x10000}, /* CONN_INFRA, wfdma */
	{0x7c500000, 0x50000, 0x10000}, /* CONN_INFRA, dyn mem map */
	{0x7c060000, 0xe0000, 0x10000}, /* CONN_INFRA, conn_host_csr_top */
	{0x7c000000, 0xf0000, 0x10000}, /* CONN_INFRA */
};
#elif defined(_HIF_AXI)
struct PCIE_CHIP_CR_MAPPING soc7_0_bus2chip_cr_mapping[] = {
	/* chip addr, bus addr, range */
	{0x830c0000, 0x400000, 0x1000}, /* WF_MCU_BUS_CR_REMAP */
	{0x54000000, 0x402000, 0x1000},  /* WFDMA PCIE0 MCU DMA0 */
	{0x55000000, 0x403000, 0x1000},  /* WFDMA PCIE0 MCU DMA1 */
	{0x56000000, 0x404000, 0x1000},  /* WFDMA reserved */
	{0x57000000, 0x405000, 0x1000},  /* WFDMA MCU wrap CR */
	{0x58000000, 0x406000, 0x1000},  /* WFDMA PCIE1 MCU DMA0 (MEM_DMA) */
	{0x59000000, 0x407000, 0x1000},  /* WFDMA PCIE1 MCU DMA1 */
	{0x820c0000, 0x408000, 0x4000},  /* WF_UMAC_TOP (PLE) */
	{0x820c8000, 0x40c000, 0x2000},  /* WF_UMAC_TOP (PSE) */
	{0x820cc000, 0x40e000, 0x2000},  /* WF_UMAC_TOP (PP) */
	{0x820e0000, 0x420000, 0x0400},  /* WF_LMAC_TOP BN0 (WF_CFG) */
	{0x820e1000, 0x420400, 0x0200},  /* WF_LMAC_TOP BN0 (WF_TRB) */
	{0x820e2000, 0x420800, 0x0400},  /* WF_LMAC_TOP BN0 (WF_AGG) */
	{0x820e3000, 0x420c00, 0x0400},  /* WF_LMAC_TOP BN0 (WF_ARB) */
	{0x820e4000, 0x421000, 0x0400},  /* WF_LMAC_TOP BN0 (WF_TMAC) */
	{0x820e5000, 0x421400, 0x0800},  /* WF_LMAC_TOP BN0 (WF_RMAC) */
	{0x820ce000, 0x421c00, 0x0200},  /* WF_LMAC_TOP (WF_SEC) */
	{0x820e7000, 0x421e00, 0x0200},  /* WF_LMAC_TOP BN0 (WF_DMA) */
	{0x820cf000, 0x422000, 0x1000},  /* WF_LMAC_TOP (WF_PF) */
	{0x820e9000, 0x423400, 0x0200},  /* WF_LMAC_TOP BN0 (WF_WTBLOFF) */
	{0x820ea000, 0x424000, 0x0200},  /* WF_LMAC_TOP BN0 (WF_ETBF) */
	{0x820eb000, 0x424200, 0x0400},  /* WF_LMAC_TOP BN0 (WF_LPON) */
	{0x820ec000, 0x424600, 0x0200},  /* WF_LMAC_TOP BN0 (WF_INT) */
	{0x820ed000, 0x424800, 0x0800},  /* WF_LMAC_TOP BN0 (WF_MIB) */
	{0x820ca000, 0x426000, 0x2000},  /* WF_LMAC_TOP BN0 (WF_MUCOP) */
	{0x820d0000, 0x430000, 0x10000}, /* WF_LMAC_TOP (WF_WTBLON) */
	{0x40000000, 0x470000, 0x10000}, /* WF_UMAC_SYSRAM */
	{0x00400000, 0x480000, 0x10000}, /* WF_MCU_SYSRAM */
	{0x00410000, 0x490000, 0x10000}, /* WF_MCU_SYSRAM (configure register)*/
	{0x820f0000, 0x4a0000, 0x0400},  /* WF_LMAC_TOP BN1 (WF_CFG) */
	{0x820f1000, 0x4a0600, 0x0200},  /* WF_LMAC_TOP BN1 (WF_TRB) */
	{0x820f2000, 0x4a0800, 0x0400},  /* WF_LMAC_TOP BN1 (WF_AGG) */
	{0x820f3000, 0x4a0c00, 0x0400},  /* WF_LMAC_TOP BN1 (WF_ARB) */
	{0x820f4000, 0x4a1000, 0x0400},  /* WF_LMAC_TOP BN1 (WF_TMAC) */
	{0x820f5000, 0x4a1400, 0x0800},  /* WF_LMAC_TOP BN1 (WF_RMAC) */
	{0x820f7000, 0x4a1e00, 0x0200},  /* WF_LMAC_TOP BN1 (WF_DMA) */
	{0x820f9000, 0x4a3400, 0x0200},  /* WF_LMAC_TOP BN1 (WF_WTBLOFF) */
	{0x820fa000, 0x4a4000, 0x0200},  /* WF_LMAC_TOP BN1 (WF_ETBF) */
	{0x820fb000, 0x4a4200, 0x0400},  /* WF_LMAC_TOP BN1 (WF_LPON) */
	{0x820fc000, 0x4a4600, 0x0200},  /* WF_LMAC_TOP BN1 (WF_INT) */
	{0x820fd000, 0x4a4800, 0x0800},  /* WF_LMAC_TOP BN1 (WF_MIB) */
	{0x820cc000, 0x4a5000, 0x2000},  /* WF_LMAC_TOP BN1 (WF_MUCOP) */
	{0x820c4000, 0x4a8000, 0x4000},  /* WF_LMAC_TOP BN1 (WF_MUCOP) */
	{0x820b0000, 0x4ae000, 0x1000},  /* [APB2] WFSYS_ON */
	{0x80020000, 0x4b0000, 0x10000}, /* WF_TOP_MISC_OFF */
	{0x81020000, 0x4c0000, 0x10000}, /* WF_TOP_MISC_ON */
	{0x7c020000, 0x4d0000, 0x10000}, /* CONN_INFRA, wfdma */
	{0x7c500000, 0x500000, 0x10000}, /* CONN_INFRA, dyn mem map */
	{0x7c060000, 0x4e0000, 0x10000}, /* CONN_INFRA, conn_host_csr_top */
	{0x7c000000, 0x4f0000, 0x10000}, /* CONN_INFRA */
};
#endif

struct wfdma_group_info soc7_0_wfmda_host_tx_group[] = {
	{"P0T0:AP DATA0", WF_WFDMA_HOST_DMA0_WPDMA_TX_RING0_CTRL0_ADDR, true},
	{"P0T1:AP DATA1", WF_WFDMA_HOST_DMA0_WPDMA_TX_RING1_CTRL0_ADDR, true},
	{"P0T17:AP CMD", WF_WFDMA_HOST_DMA0_WPDMA_TX_RING17_CTRL0_ADDR, true},
	{"P0T16:FWDL", WF_WFDMA_HOST_DMA0_WPDMA_TX_RING16_CTRL0_ADDR, true},
	{"P0T8:MD DATA0", WF_WFDMA_HOST_DMA0_WPDMA_TX_RING8_CTRL0_ADDR},
	{"P0T9:MD DATA1", WF_WFDMA_HOST_DMA0_WPDMA_TX_RING9_CTRL0_ADDR},
	{"P0T18:MD CMD", WF_WFDMA_HOST_DMA0_WPDMA_TX_RING18_CTRL0_ADDR},
};

struct wfdma_group_info soc7_0_wfmda_host_rx_group[] = {
	{"P0R0:AP DATA0", WF_WFDMA_HOST_DMA0_WPDMA_RX_RING0_CTRL0_ADDR, true},
	{"P0R2:AP TDONE0", WF_WFDMA_HOST_DMA0_WPDMA_RX_RING2_CTRL0_ADDR, true},
	{"P0R1:AP DATA1", WF_WFDMA_HOST_DMA0_WPDMA_RX_RING1_CTRL0_ADDR, true},
	{"P0R3:AP TDONE1", WF_WFDMA_HOST_DMA0_WPDMA_RX_RING3_CTRL0_ADDR},
	{"P0R4:MD DATA0", WF_WFDMA_HOST_DMA0_WPDMA_RX_RING4_CTRL0_ADDR},
	{"P0R5:MD DATA1", WF_WFDMA_HOST_DMA0_WPDMA_RX_RING5_CTRL0_ADDR},
	{"P0R6:MD TDONE0", WF_WFDMA_HOST_DMA0_WPDMA_RX_RING6_CTRL0_ADDR},
	{"P0R7:MD TDONE1", WF_WFDMA_HOST_DMA0_WPDMA_RX_RING7_CTRL0_ADDR},
};

struct pse_group_info rSoc7_0_pse_group[] = {
	{"HIF0(TX data)", WF_PSE_TOP_PG_HIF0_GROUP_ADDR,
		WF_PSE_TOP_HIF0_PG_INFO_ADDR},
	{"HIF1(Talos CMD)", WF_PSE_TOP_PG_HIF1_GROUP_ADDR,
		WF_PSE_TOP_HIF1_PG_INFO_ADDR},
#if 0
	{"HIF2", WF_PSE_TOP_PG_HIF2_GROUP_ADDR,
		WF_PSE_TOP_HIF2_PG_INFO_ADDR},
#endif
	{"CPU(I/O r/w)",  WF_PSE_TOP_PG_CPU_GROUP_ADDR,
		WF_PSE_TOP_CPU_PG_INFO_ADDR},
	{"PLE(host report)",  WF_PSE_TOP_PG_PLE_GROUP_ADDR,
		WF_PSE_TOP_PLE_PG_INFO_ADDR},
	{"PLE1(SPL report)", WF_PSE_TOP_PG_PLE1_GROUP_ADDR,
		WF_PSE_TOP_PLE1_PG_INFO_ADDR},
	{"LMAC0(RX data)", WF_PSE_TOP_PG_LMAC0_GROUP_ADDR,
			WF_PSE_TOP_LMAC0_PG_INFO_ADDR},
	{"LMAC1(RX_VEC)", WF_PSE_TOP_PG_LMAC1_GROUP_ADDR,
			WF_PSE_TOP_LMAC1_PG_INFO_ADDR},
	{"LMAC2(TXS)", WF_PSE_TOP_PG_LMAC2_GROUP_ADDR,
			WF_PSE_TOP_LMAC2_PG_INFO_ADDR},
	{"LMAC3(TXCMD/RXRPT)", WF_PSE_TOP_PG_LMAC3_GROUP_ADDR,
			WF_PSE_TOP_LMAC3_PG_INFO_ADDR},
	{"MDP",  WF_PSE_TOP_PG_MDP_GROUP_ADDR,
			WF_PSE_TOP_MDP_PG_INFO_ADDR},
#if 0
	{"MDP1", WF_PSE_TOP_PG_MDP1_GROUP_ADDR,
		WF_PSE_TOP_MDP1_PG_INFO_ADDR},
	{"MDP2", WF_PSE_TOP_PG_MDP2_GROUP_ADDR,
		WF_PSE_TOP_MDP2_PG_INFO_ADDR},
#endif
};

struct BUS_INFO soc7_0_bus_info = {
#if defined(_HIF_PCIE) || defined(_HIF_AXI)
	.top_cfg_base = SOC7_0_TOP_CFG_BASE,

	/* host_dma0 for TXP */
	.host_dma0_base = WF_WFDMA_HOST_DMA0_BASE,
	.host_int_status_addr = WF_WFDMA_HOST_DMA0_HOST_INT_STA_ADDR,

	.host_int_txdone_bits =
		(CONNAC2X_WFDMA_TX_DONE_INT0 | CONNAC2X_WFDMA_TX_DONE_INT1 |
		CONNAC2X_WFDMA_TX_DONE_INT2 | CONNAC2X_WFDMA_TX_DONE_INT3 |
		CONNAC2X_WFDMA_TX_DONE_INT16 | CONNAC2X_WFDMA_TX_DONE_INT17),
	.host_int_rxdone_bits =
		(CONNAC2X_WFDMA_RX_DONE_INT0 | CONNAC2X_WFDMA_RX_DONE_INT1 |
		 CONNAC2X_WFDMA_RX_DONE_INT2 | CONNAC2X_WFDMA_RX_DONE_INT3),

	.host_tx_ring_base = WF_WFDMA_HOST_DMA0_WPDMA_TX_RING0_CTRL0_ADDR,
	.host_tx_ring_ext_ctrl_base =
		WF_WFDMA_HOST_DMA0_WPDMA_TX_RING0_EXT_CTRL_ADDR,
	.host_tx_ring_cidx_addr = WF_WFDMA_HOST_DMA0_WPDMA_TX_RING0_CTRL2_ADDR,
	.host_tx_ring_didx_addr = WF_WFDMA_HOST_DMA0_WPDMA_TX_RING0_CTRL3_ADDR,
	.host_tx_ring_cnt_addr = WF_WFDMA_HOST_DMA0_WPDMA_TX_RING0_CTRL1_ADDR,

	.host_rx_ring_base = WF_WFDMA_HOST_DMA0_WPDMA_RX_RING0_CTRL0_ADDR,
	.host_rx_ring_ext_ctrl_base =
		WF_WFDMA_HOST_DMA0_WPDMA_TX_RING0_EXT_CTRL_ADDR,
	.host_rx_ring_cidx_addr = WF_WFDMA_HOST_DMA0_WPDMA_RX_RING0_CTRL2_ADDR,
	.host_rx_ring_didx_addr = WF_WFDMA_HOST_DMA0_WPDMA_RX_RING0_CTRL3_ADDR,
	.host_rx_ring_cnt_addr = WF_WFDMA_HOST_DMA0_WPDMA_RX_RING0_CTRL1_ADDR,

	.bus2chip = soc7_0_bus2chip_cr_mapping,
	.max_static_map_addr = 0x000f0000,

	.tx_ring_fwdl_idx = CONNAC2X_FWDL_TX_RING_IDX,
	.tx_ring_cmd_idx = 15,
	.tx_ring0_data_idx = 0,
	.tx_ring1_data_idx = 1,
	.fw_own_clear_addr = CONNAC2X_BN0_IRQ_STAT_ADDR,
	.fw_own_clear_bit = PCIE_LPCR_FW_CLR_OWN,
	.fgCheckDriverOwnInt = FALSE,
	.u4DmaMask = 32,
#if defined(_HIF_PCIE)
	.pcie2ap_remap_2 = CONN_INFRA_CFG_PCIE2AP_REMAP_2_ADDR,
#endif
	.ap2wf_remap_1 = CONN_INFRA_CFG_AP2WF_REMAP_1_ADDR,
	.wfmda_host_tx_group = soc7_0_wfmda_host_tx_group,
	.wfmda_host_tx_group_len = ARRAY_SIZE(soc7_0_wfmda_host_tx_group),
	.wfmda_host_rx_group = soc7_0_wfmda_host_rx_group,
	.wfmda_host_rx_group_len = ARRAY_SIZE(soc7_0_wfmda_host_rx_group),
	.wfmda_wm_tx_group = NULL,
	.wfmda_wm_tx_group_len = 0,
	.wfmda_wm_rx_group = NULL,
	.wfmda_wm_rx_group_len = 0,
	.prDmashdlCfg = &rSOC7_0_DmashdlCfg,
	.prPleTopCr = &rSoc7_0_PleTopCr,
	.prPseTopCr = &rSoc7_0_PseTopCr,
	.prPseGroup = rSoc7_0_pse_group,
	.u4PseGroupLen = ARRAY_SIZE(rSoc7_0_pse_group),
	.pdmaSetup = soc7_0asicConnac2xWpdmaConfig,
	.enableInterrupt = asicConnac2xEnablePlatformIRQ,
	.disableInterrupt = asicConnac2xDisablePlatformIRQ,
	.disableSwInterrupt = asicConnac2xDisablePlatformSwIRQ,
	.processTxInterrupt = soc7_0asicConnac2xProcessTxInterrupt,
	.processRxInterrupt = soc7_0asicConnac2xProcessRxInterrupt,
	.tx_ring_ext_ctrl = asicConnac2xWfdmaTxRingExtCtrl,
	.rx_ring_ext_ctrl = asicConnac2xWfdmaRxRingExtCtrl,
	/* null wfdmaManualPrefetch if want to disable manual mode */
	.wfdmaManualPrefetch = soc7_0asicConnac2xWfdmaManualPrefetch,
	.lowPowerOwnRead = asicConnac2xLowPowerOwnRead,
	.lowPowerOwnSet = asicConnac2xLowPowerOwnSet,
	.lowPowerOwnClear = asicConnac2xLowPowerOwnClear,
	.wakeUpWiFi = asicWakeUpWiFi,
	.processSoftwareInterrupt = asicConnac2xProcessSoftwareInterrupt,
	.softwareInterruptMcu = asicConnac2xSoftwareInterruptMcu,
	.hifRst = asicConnac2xHifRst,
	.initPcieInt = NULL,
	.devReadIntStatus = soc7_0ReadIntStatus,
	.DmaShdlInit = soc7_0DmashdlInit,
	.setRxRingHwAddr = soc7_0SetRxRingHwAddr,
	.wfdmaAllocRxRing = soc7_0WfdmaAllocRxRing,
	.enableFwDlMode = soc7_0EnableFwDlMode,
#endif /*_HIF_PCIE || _HIF_AXI */
};

#if CFG_ENABLE_FW_DOWNLOAD
struct FWDL_OPS_T soc7_0_fw_dl_ops = {
	.constructFirmwarePrio = soc7_0_ConstructFirmwarePrio,
	.constructPatchName = NULL,
	.downloadPatch = NULL,
	.downloadFirmware = wlanConnacFormatDownload,
#if (CFG_DOWNLOAD_DYN_MEMORY_MAP == 1)
	.downloadByDynMemMap = downloadImgByDynMemMap,
#else
	.downloadByDynMemMap = NULL,
#endif
	.getFwInfo = wlanGetConnacFwInfo,
	.getFwDlInfo = asicGetFwDlInfo,
};
#endif /* CFG_ENABLE_FW_DOWNLOAD */

struct TX_DESC_OPS_T soc7_0_TxDescOps = {
	.fillNicAppend = fillNicTxDescAppend,
	.fillHifAppend = fillTxDescAppendByHostV2,
	.fillTxByteCount = fillConnac2xTxDescTxByteCount,
};

struct RX_DESC_OPS_T soc7_0_RxDescOps = {};

struct CHIP_DBG_OPS soc7_0_DebugOps = {
	.showPdmaInfo = connac2x_show_wfdma_info,
	.showPseInfo = connac2x_show_pse_info,
	.showPleInfo = connac2x_show_ple_info,
	.showTxdInfo = connac2x_show_txd_Info,
	.showWtblInfo = connac2x_show_wtbl_info,
	.showUmacFwtblInfo = connac2x_show_umac_wtbl_info,
	.showCsrInfo = NULL,
	.showDmaschInfo = connac2x_show_dmashdl_info,
	.getFwDebug = connac2x_get_ple_int,
	.setFwDebug = connac2x_set_ple_int,
	.showHifInfo = NULL,
	.printHifDbgInfo = NULL,
	.show_rx_rate_info = connac2x_show_rx_rate_info,
	.show_rx_rssi_info = connac2x_show_rx_rssi_info,
	.show_stat_info = connac2x_show_stat_info,
	.show_wfdma_dbg_probe_info = soc7_0_show_wfdma_dbg_probe_info,
	.show_wfdma_wrapper_info = soc7_0_show_wfdma_wrapper_info,
};

struct mt66xx_chip_info mt66xx_chip_info_soc7_0 = {
	.bus_info = &soc7_0_bus_info,
#if CFG_ENABLE_FW_DOWNLOAD
	.fw_dl_ops = &soc7_0_fw_dl_ops,
#endif /* CFG_ENABLE_FW_DOWNLOAD */
	.prTxDescOps = &soc7_0_TxDescOps,
	.prRxDescOps = &soc7_0_RxDescOps,
	.prDebugOps = &soc7_0_DebugOps,
	.chip_id = SOC7_0_CHIP_ID,
	.should_verify_chip_id = FALSE,
	.sw_sync0 = CONNAC2x_CONN_CFG_ON_CONN_ON_MISC_ADDR,
	.sw_ready_bits = WIFI_FUNC_NO_CR4_READY_BITS,
	.sw_ready_bit_offset =
		CONNAC2x_CONN_CFG_ON_CONN_ON_MISC_DRV_FM_STAT_SYNC_SHFT,
	.patch_addr = SOC7_0_PATCH_START_ADDR,
	.is_support_cr4 = FALSE,
	.is_support_wacpu = FALSE,
	.txd_append_size = SOC7_0_TX_DESC_APPEND_LENGTH,
	.rxd_size = SOC7_0_RX_DESC_LENGTH,
	.pse_header_length = CONNAC2X_NIC_TX_PSE_HEADER_LENGTH,
	.init_event_size = CONNAC2X_RX_INIT_EVENT_LENGTH,
	.eco_info = soc7_0_eco_table,
	.isNicCapV1 = FALSE,
	.top_hcr = CONNAC2X_TOP_HCR,
	.top_hvr = CONNAC2X_TOP_HVR,
	.top_fvr = CONNAC2X_TOP_FVR,
	.arb_ac_mode_addr = SOC7_0_ARB_AC_MODE_ADDR,
	.asicCapInit = asicConnac2xCapInit,
#if CFG_ENABLE_FW_DOWNLOAD
	.asicEnableFWDownload = NULL,
#endif /* CFG_ENABLE_FW_DOWNLOAD */
	.downloadBufferBin = NULL,
	.is_support_hw_amsdu = TRUE,
	.is_support_asic_lp = TRUE,
	.is_support_wfdma1 = FALSE,
	.asicWfdmaReInit = asicConnac2xWfdmaReInit,
	.asicWfdmaReInit_handshakeInit = asicConnac2xWfdmaDummyCrWrite,
	.group5_size = sizeof(struct HW_MAC_RX_STS_GROUP_5),
	.u4LmacWtblDUAddr = CONNAC2X_WIFI_LWTBL_BASE,
	.u4UmacWtblDUAddr = CONNAC2X_WIFI_UWTBL_BASE,
};

struct mt66xx_hif_driver_data mt66xx_driver_data_soc7_0 = {
	.chip_info = &mt66xx_chip_info_soc7_0,
};

static void soc7_0_ConstructFirmwarePrio(struct GLUE_INFO *prGlueInfo,
	uint8_t **apucNameTable, uint8_t **apucName,
	uint8_t *pucNameIdx, uint8_t ucMaxNameIdx)
{
	int ret = 0;
	uint8_t ucIdx = 0;

	for (ucIdx = 0; apucsoc7_0FwName[ucIdx]; ucIdx++) {
		if ((*pucNameIdx + 3) >= ucMaxNameIdx) {
			/* the table is not large enough */
			DBGLOG(INIT, ERROR,
				"kalFirmwareImageMapping >> file name array is not enough.\n");
			ASSERT(0);
			continue;
		}

		/* Type 1. WIFI_RAM_CODE_soc7_0_1_1.bin */
		ret = kalSnprintf(*(apucName + (*pucNameIdx)),
				CFG_FW_NAME_MAX_LEN, "%s_%u_%u.bin",
				apucsoc7_0FwName[ucIdx],
				CFG_WIFI_IP_SET,
				wlanGetEcoVersion(
					prGlueInfo->prAdapter));
		if (ret >= 0 && ret < CFG_FW_NAME_MAX_LEN)
			(*pucNameIdx) += 1;
		else
			DBGLOG(INIT, ERROR,
					"[%u] kalSnprintf failed, ret: %d\n",
					__LINE__, ret);
	}
}

static uint8_t soc7_0SetRxRingHwAddr(struct RTMP_RX_RING *prRxRing,
		struct BUS_INFO *prBusInfo, uint32_t u4SwRingIdx)
{
	uint32_t offset = 0;

	/*
	 * RX_RING_DATA_IDX_0   (RX_Ring0) - Band0 Rx Data
	 * RX_RING_DATA1_IDX_2 (RX_Ring1) - Band1 Rx Data
	 * RX_RING_EVT_IDX_1    (RX_Ring2) - Band0 Tx Free Done Event / Rx Event
	 * RX_RING_TXDONE0_IDX_3 (RX_Ring3) - Band1 Tx Free Done Event
	*/
	switch (u4SwRingIdx) {
	case RX_RING_EVT_IDX_1:
		offset = 2 * MT_RINGREG_DIFF;
		break;
	case RX_RING_DATA1_IDX_2:
		offset = 1 * MT_RINGREG_DIFF;
		break;
	case RX_RING_DATA_IDX_0:
	case RX_RING_TXDONE0_IDX_3:
		offset = u4SwRingIdx * MT_RINGREG_DIFF;
		break;
	default:
		return FALSE;
	}

	prRxRing->hw_desc_base = prBusInfo->host_rx_ring_base + offset;
	prRxRing->hw_cidx_addr = prBusInfo->host_rx_ring_cidx_addr + offset;
	prRxRing->hw_didx_addr = prBusInfo->host_rx_ring_didx_addr + offset;
	prRxRing->hw_cnt_addr = prBusInfo->host_rx_ring_cnt_addr + offset;

	return TRUE;
}

static bool soc7_0WfdmaAllocRxRing(struct GLUE_INFO *prGlueInfo,
		bool fgAllocMem)
{
	/* Band1 Data Rx path */
	if (!halWpdmaAllocRxRing(prGlueInfo,
			RX_RING_DATA1_IDX_2, RX_RING0_SIZE,
			RXD_SIZE, CFG_RX_MAX_PKT_SIZE, fgAllocMem)) {
		DBGLOG(HAL, ERROR, "AllocRxRing[2] fail\n");
		return false;
	}
	/* Band0 Tx Free Done Event */
	if (!halWpdmaAllocRxRing(prGlueInfo,
			RX_RING_TXDONE0_IDX_3, RX_RING1_SIZE,
			RXD_SIZE, RX_BUFFER_AGGRESIZE, fgAllocMem)) {
		DBGLOG(HAL, ERROR, "AllocRxRing[3] fail\n");
		return false;
	}
	return true;
}

static void soc7_0asicConnac2xProcessTxInterrupt(
		struct ADAPTER *prAdapter)
{
	struct GL_HIF_INFO *prHifInfo = &prAdapter->prGlueInfo->rHifInfo;
	union WPDMA_INT_STA_STRUCT rIntrStatus;

	rIntrStatus = (union WPDMA_INT_STA_STRUCT)prHifInfo->u4IntStatus;
	if (rIntrStatus.field_conn2x_single.wfdma0_tx_done_16)
		halWpdmaProcessCmdDmaDone(
			prAdapter->prGlueInfo, TX_RING_FWDL_IDX_3);

	if (rIntrStatus.field_conn2x_single.wfdma0_tx_done_17)
		halWpdmaProcessCmdDmaDone(
			prAdapter->prGlueInfo, TX_RING_CMD_IDX_2);

	if (rIntrStatus.field_conn2x_single.wfdma0_tx_done_0) {
		halWpdmaProcessDataDmaDone(
			prAdapter->prGlueInfo, TX_RING_DATA0_IDX_0);
		kalSetTxEvent2Hif(prAdapter->prGlueInfo);
	}

	if (rIntrStatus.field_conn2x_single.wfdma0_tx_done_1) {
		halWpdmaProcessDataDmaDone(
			prAdapter->prGlueInfo, TX_RING_DATA1_IDX_1);
		kalSetTxEvent2Hif(prAdapter->prGlueInfo);
	}
}

static void soc7_0asicConnac2xProcessRxInterrupt(
	struct ADAPTER *prAdapter)
{
	struct GL_HIF_INFO *prHifInfo = &prAdapter->prGlueInfo->rHifInfo;
	union WPDMA_INT_STA_STRUCT rIntrStatus;

	rIntrStatus = (union WPDMA_INT_STA_STRUCT)prHifInfo->u4IntStatus;

	if (rIntrStatus.field_conn2x_single.wfdma0_rx_done_0)
		halRxReceiveRFBs(prAdapter, RX_RING_DATA_IDX_0, TRUE);

	if (rIntrStatus.field_conn2x_single.wfdma0_rx_done_1)
		halRxReceiveRFBs(prAdapter, RX_RING_DATA1_IDX_2, TRUE);

	if (rIntrStatus.field_conn2x_single.wfdma0_rx_done_2)
		halRxReceiveRFBs(prAdapter, RX_RING_EVT_IDX_1, FALSE);

	if (rIntrStatus.field_conn2x_single.wfdma0_rx_done_3)
		halRxReceiveRFBs(prAdapter, RX_RING_TXDONE0_IDX_3, FALSE);
}

#if CFG_MTK_MCIF_WIFI_SUPPORT
static void soc7_0SetMDRXRingPriorityInterrupt(struct ADAPTER *prAdapter)
{
	u_int32_t val = 0;

	HAL_MCR_RD(prAdapter,
		WF_WFDMA_HOST_DMA0_WPDMA_INT_RX_PRI_SEL_ADDR, &val);
	val |= BITS(4, 7);
	HAL_MCR_WR(prAdapter,
		WF_WFDMA_HOST_DMA0_WPDMA_INT_RX_PRI_SEL_ADDR, val);
}
#endif /* CFG_MTK_MCIF_WIFI_SUPPORT */

static void soc7_0asicConnac2xWfdmaManualPrefetch(
	struct GLUE_INFO *prGlueInfo)
{
	struct ADAPTER *prAdapter = prGlueInfo->prAdapter;
	u_int32_t val = 0;
	uint32_t u4WrVal = 0x00000004, u4Addr = 0;

	HAL_MCR_RD(prAdapter, WF_WFDMA_HOST_DMA0_WPDMA_GLO_CFG_ADDR, &val);
	/* disable prefetch offset calculation auto-mode */
	val &=
	~WF_WFDMA_HOST_DMA0_WPDMA_GLO_CFG_CSR_DISP_BASE_PTR_CHAIN_EN_MASK;
	HAL_MCR_WR(prAdapter, WF_WFDMA_HOST_DMA0_WPDMA_GLO_CFG_ADDR, val);

	/* Rx ring */
	for (u4Addr = WF_WFDMA_HOST_DMA0_WPDMA_RX_RING0_EXT_CTRL_ADDR;
	     u4Addr <= WF_WFDMA_HOST_DMA0_WPDMA_RX_RING3_EXT_CTRL_ADDR;
	     u4Addr += 0x4) {
		HAL_MCR_WR(prAdapter, u4Addr, u4WrVal);
		u4WrVal += 0x00400000;
	}

#if CFG_MTK_MCIF_WIFI_SUPPORT
	for (u4Addr = WF_WFDMA_HOST_DMA0_WPDMA_RX_RING4_EXT_CTRL_ADDR;
	     u4Addr <= WF_WFDMA_HOST_DMA0_WPDMA_RX_RING7_EXT_CTRL_ADDR;
	     u4Addr += 0x4) {
		HAL_MCR_WR(prAdapter, u4Addr, u4WrVal);
		u4WrVal += 0x00400000;
	}
#endif

	/* Tx ring */
	for (u4Addr = WF_WFDMA_HOST_DMA0_WPDMA_TX_RING0_EXT_CTRL_ADDR;
	     u4Addr <= WF_WFDMA_HOST_DMA0_WPDMA_TX_RING1_EXT_CTRL_ADDR;
	     u4Addr += 0x4) {
		HAL_MCR_WR(prAdapter, u4Addr, u4WrVal);
		u4WrVal += 0x00400000;
	}

	for (u4Addr = WF_WFDMA_HOST_DMA0_WPDMA_TX_RING16_EXT_CTRL_ADDR;
	     u4Addr <= WF_WFDMA_HOST_DMA0_WPDMA_TX_RING17_EXT_CTRL_ADDR;
	     u4Addr += 0x4) {
		HAL_MCR_WR(prAdapter, u4Addr, u4WrVal);
		u4WrVal += 0x00400000;
	}

#if CFG_MTK_MCIF_WIFI_SUPPORT
	for (u4Addr = WF_WFDMA_HOST_DMA0_WPDMA_TX_RING8_EXT_CTRL_ADDR;
	     u4Addr <= WF_WFDMA_HOST_DMA0_WPDMA_TX_RING9_EXT_CTRL_ADDR;
	     u4Addr += 0x4) {
		HAL_MCR_WR(prAdapter, u4Addr, u4WrVal);
		u4WrVal += 0x00400000;
	}
	HAL_MCR_WR(prAdapter, WF_WFDMA_HOST_DMA0_WPDMA_TX_RING18_EXT_CTRL_ADDR,
	     0x04000004);
	u4WrVal += 0x00400000;
#endif
	/* fill last dummy ring */
	HAL_MCR_WR(prAdapter, WF_WFDMA_HOST_DMA0_WPDMA_TX_RING19_EXT_CTRL_ADDR,
		   u4WrVal);

#if CFG_MTK_MCIF_WIFI_SUPPORT
	soc7_0SetMDRXRingPriorityInterrupt(prAdapter);
#endif /* CFG_MTK_MCIF_WIFI_SUPPORT */

	/* reset dma TRX idx */
	HAL_MCR_WR(prAdapter,
		WF_WFDMA_HOST_DMA0_WPDMA_RST_DTX_PTR_ADDR, 0xFFFFFFFF);
	HAL_MCR_WR(prAdapter,
		WF_WFDMA_HOST_DMA0_WPDMA_RST_DRX_PTR_ADDR, 0xFFFFFFFF);
}

static void soc7_0ReadIntStatus(struct ADAPTER *prAdapter,
		uint32_t *pu4IntStatus)
{
	uint32_t u4RegValue, u4WrValue = 0;
	struct GL_HIF_INFO *prHifInfo = &prAdapter->prGlueInfo->rHifInfo;
	struct BUS_INFO *prBusInfo = prAdapter->chip_info->bus_info;

	*pu4IntStatus = 0;

	HAL_MCR_RD(prAdapter,
		WF_WFDMA_HOST_DMA0_HOST_INT_STA_ADDR, &u4RegValue);

	if (HAL_IS_CONNAC2X_EXT_RX_DONE_INTR(
		    u4RegValue, prBusInfo->host_int_rxdone_bits)) {
		*pu4IntStatus |= WHISR_RX0_DONE_INT;
		u4WrValue |= (u4RegValue & prBusInfo->host_int_rxdone_bits);
	}

	if (HAL_IS_CONNAC2X_EXT_TX_DONE_INTR(
		    u4RegValue, prBusInfo->host_int_txdone_bits)) {
		*pu4IntStatus |= WHISR_TX_DONE_INT;
		u4WrValue |= (u4RegValue & prBusInfo->host_int_txdone_bits);
	}

	if (u4RegValue & CONNAC_MCU_SW_INT) {
		*pu4IntStatus |= WHISR_D2H_SW_INT;
		u4WrValue |= (u4RegValue & CONNAC_MCU_SW_INT);
	}

	prHifInfo->u4IntStatus = u4RegValue;

	/* clear interrupt */
	HAL_MCR_WR(prAdapter,
		WF_WFDMA_HOST_DMA0_HOST_INT_STA_ADDR, u4WrValue);
}

static void configIntMask(struct GLUE_INFO *prGlueInfo,
		u_int8_t enable)
{
	union WPDMA_INT_MASK IntMask;
	uint32_t u4Addr = 0, u4Val = 0;

	u4Addr = enable ? WF_WFDMA_HOST_DMA0_HOST_INT_ENA_SET_ADDR :
		WF_WFDMA_HOST_DMA0_HOST_INT_ENA_CLR_ADDR;

	IntMask.word = 0;
	IntMask.field_conn2x_single.wfdma0_rx_done_0 = 1;
	IntMask.field_conn2x_single.wfdma0_rx_done_1 = 1;
	IntMask.field_conn2x_single.wfdma0_rx_done_2 = 1;
	IntMask.field_conn2x_single.wfdma0_rx_done_3 = 1;
	IntMask.field_conn2x_single.wfdma0_tx_done_0 = 1;
	IntMask.field_conn2x_single.wfdma0_tx_done_1 = 1;
	IntMask.field_conn2x_single.wfdma0_tx_done_17 = 1;
	IntMask.field_conn2x_single.wfdma0_tx_done_16 = 1;
	IntMask.field_conn2x_single.wfdma0_mcu2host_sw_int_en = 1;

	HAL_MCR_WR(prGlueInfo->prAdapter, u4Addr, IntMask.word);

	HAL_MCR_RD(prGlueInfo->prAdapter,
		   WF_WFDMA_HOST_DMA0_HOST_INT_ENA_ADDR, &u4Val);

	DBGLOG(HAL, TRACE,
	       "HOST_INT_STA(0x%08x):0x%08x, En:%u, Word:0x%08x\n",
	       WF_WFDMA_HOST_DMA0_HOST_INT_ENA_ADDR,
	       u4Val,
	       enable,
	       IntMask.word);
}

static void soc7_0asicConnac2xWpdmaConfig(struct GLUE_INFO *prGlueInfo,
		u_int8_t enable, bool fgResetHif)
{
	struct ADAPTER *prAdapter = prGlueInfo->prAdapter;
	union WPDMA_GLO_CFG_STRUCT GloCfg;
	uint32_t u4DmaCfgCr;
	uint32_t idx;

	asicConnac2xWfdmaControl(prGlueInfo, 0, enable);
	u4DmaCfgCr = asicConnac2xWfdmaCfgAddrGet(prGlueInfo, 0);
	HAL_MCR_RD(prAdapter, u4DmaCfgCr, &GloCfg.word);

	configIntMask(prGlueInfo, enable);

	if (enable) {
		u4DmaCfgCr = asicConnac2xWfdmaCfgAddrGet(prGlueInfo, idx);
		GloCfg.field_conn2x.tx_dma_en = 1;
		GloCfg.field_conn2x.rx_dma_en = 1;
		HAL_MCR_WR(prAdapter, u4DmaCfgCr, GloCfg.word);
	}
}

static void soc7_0EnableFwDlMode(struct ADAPTER *prAdapter)
{
	u_int32_t val = 0;

	HAL_MCR_RD(prAdapter, WF_WFDMA_HOST_DMA0_PDA_CONFG_ADDR, &val);
	val |= (BIT(0) << 31);
	HAL_MCR_WR(prAdapter, WF_WFDMA_HOST_DMA0_PDA_CONFG_ADDR, val);
}

#endif  /* soc7_0 */
