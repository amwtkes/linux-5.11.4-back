/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
 */

#ifndef QCOM_PHY_QMP_H_
#define QCOM_PHY_QMP_H_

/* Only for QMP V2 PHY - QSERDES COM registers */
#define QSERDES_COM_BG_TIMER				0x00c
#define QSERDES_COM_SSC_EN_CENTER			0x010
#define QSERDES_COM_SSC_ADJ_PER1			0x014
#define QSERDES_COM_SSC_ADJ_PER2			0x018
#define QSERDES_COM_SSC_PER1				0x01c
#define QSERDES_COM_SSC_PER2				0x020
#define QSERDES_COM_SSC_STEP_SIZE1			0x024
#define QSERDES_COM_SSC_STEP_SIZE2			0x028
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN			0x034
#define QSERDES_COM_CLK_ENABLE1				0x038
#define QSERDES_COM_SYS_CLK_CTRL			0x03c
#define QSERDES_COM_SYSCLK_BUF_ENABLE			0x040
#define QSERDES_COM_PLL_IVCO				0x048
#define QSERDES_COM_LOCK_CMP1_MODE0			0x04c
#define QSERDES_COM_LOCK_CMP2_MODE0			0x050
#define QSERDES_COM_LOCK_CMP3_MODE0			0x054
#define QSERDES_COM_LOCK_CMP1_MODE1			0x058
#define QSERDES_COM_LOCK_CMP2_MODE1			0x05c
#define QSERDES_COM_LOCK_CMP3_MODE1			0x060
#define QSERDES_COM_BG_TRIM				0x070
#define QSERDES_COM_CLK_EP_DIV				0x074
#define QSERDES_COM_CP_CTRL_MODE0			0x078
#define QSERDES_COM_CP_CTRL_MODE1			0x07c
#define QSERDES_COM_PLL_RCTRL_MODE0			0x084
#define QSERDES_COM_PLL_RCTRL_MODE1			0x088
#define QSERDES_COM_PLL_CCTRL_MODE0			0x090
#define QSERDES_COM_PLL_CCTRL_MODE1			0x094
#define QSERDES_COM_BIAS_EN_CTRL_BY_PSM			0x0a8
#define QSERDES_COM_SYSCLK_EN_SEL			0x0ac
#define QSERDES_COM_RESETSM_CNTRL			0x0b4
#define QSERDES_COM_RESTRIM_CTRL			0x0bc
#define QSERDES_COM_RESCODE_DIV_NUM			0x0c4
#define QSERDES_COM_LOCK_CMP_EN				0x0c8
#define QSERDES_COM_LOCK_CMP_CFG			0x0cc
#define QSERDES_COM_DEC_START_MODE0			0x0d0
#define QSERDES_COM_DEC_START_MODE1			0x0d4
#define QSERDES_COM_DIV_FRAC_START1_MODE0		0x0dc
#define QSERDES_COM_DIV_FRAC_START2_MODE0		0x0e0
#define QSERDES_COM_DIV_FRAC_START3_MODE0		0x0e4
#define QSERDES_COM_DIV_FRAC_START1_MODE1		0x0e8
#define QSERDES_COM_DIV_FRAC_START2_MODE1		0x0ec
#define QSERDES_COM_DIV_FRAC_START3_MODE1		0x0f0
#define QSERDES_COM_INTEGLOOP_GAIN0_MODE0		0x108
#define QSERDES_COM_INTEGLOOP_GAIN1_MODE0		0x10c
#define QSERDES_COM_INTEGLOOP_GAIN0_MODE1		0x110
#define QSERDES_COM_INTEGLOOP_GAIN1_MODE1		0x114
#define QSERDES_COM_VCO_TUNE_CTRL			0x124
#define QSERDES_COM_VCO_TUNE_MAP			0x128
#define QSERDES_COM_VCO_TUNE1_MODE0			0x12c
#define QSERDES_COM_VCO_TUNE2_MODE0			0x130
#define QSERDES_COM_VCO_TUNE1_MODE1			0x134
#define QSERDES_COM_VCO_TUNE2_MODE1			0x138
#define QSERDES_COM_VCO_TUNE_TIMER1			0x144
#define QSERDES_COM_VCO_TUNE_TIMER2			0x148
#define QSERDES_COM_BG_CTRL				0x170
#define QSERDES_COM_CLK_SELECT				0x174
#define QSERDES_COM_HSCLK_SEL				0x178
#define QSERDES_COM_CORECLK_DIV				0x184
#define QSERDES_COM_CORE_CLK_EN				0x18c
#define QSERDES_COM_C_READY_STATUS			0x190
#define QSERDES_COM_CMN_CONFIG				0x194
#define QSERDES_COM_SVS_MODE_CLK_SEL			0x19c
#define QSERDES_COM_DEBUG_BUS0				0x1a0
#define QSERDES_COM_DEBUG_BUS1				0x1a4
#define QSERDES_COM_DEBUG_BUS2				0x1a8
#define QSERDES_COM_DEBUG_BUS3				0x1ac
#define QSERDES_COM_DEBUG_BUS_SEL			0x1b0
#define QSERDES_COM_CORECLK_DIV_MODE1			0x1bc

/* Only for QMP V2 PHY - TX registers */
#define QSERDES_TX_EMP_POST1_LVL			0x018
#define QSERDES_TX_SLEW_CNTL				0x040
#define QSERDES_TX_RES_CODE_LANE_OFFSET			0x054
#define QSERDES_TX_DEBUG_BUS_SEL			0x064
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN	0x068
#define QSERDES_TX_LANE_MODE				0x094
#define QSERDES_TX_RCV_DETECT_LVL_2			0x0ac

/* Only for QMP V2 PHY - RX registers */
#define QSERDES_RX_UCDR_SO_GAIN_HALF			0x010
#define QSERDES_RX_UCDR_SO_GAIN				0x01c
#define QSERDES_RX_UCDR_FASTLOCK_FO_GAIN		0x040
#define QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE	0x048
#define QSERDES_RX_RX_TERM_BW				0x090
#define QSERDES_RX_RX_EQ_GAIN1_LSB			0x0c4
#define QSERDES_RX_RX_EQ_GAIN1_MSB			0x0c8
#define QSERDES_RX_RX_EQ_GAIN2_LSB			0x0cc
#define QSERDES_RX_RX_EQ_GAIN2_MSB			0x0d0
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2		0x0d8
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3		0x0dc
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4		0x0e0
#define QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1		0x108
#define QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2		0x10c
#define QSERDES_RX_SIGDET_ENABLES			0x110
#define QSERDES_RX_SIGDET_CNTRL				0x114
#define QSERDES_RX_SIGDET_LVL				0x118
#define QSERDES_RX_SIGDET_DEGLITCH_CNTRL		0x11c
#define QSERDES_RX_RX_BAND				0x120
#define QSERDES_RX_RX_INTERFACE_MODE			0x12c

/* Only for QMP V2 PHY - PCS registers */
#define QPHY_POWER_DOWN_CONTROL				0x04
#define QPHY_TXDEEMPH_M6DB_V0				0x24
#define QPHY_TXDEEMPH_M3P5DB_V0				0x28
#define QPHY_ENDPOINT_REFCLK_DRIVE			0x54
#define QPHY_RX_IDLE_DTCT_CNTRL				0x58
#define QPHY_POWER_STATE_CONFIG1			0x60
#define QPHY_POWER_STATE_CONFIG2			0x64
#define QPHY_POWER_STATE_CONFIG4			0x6c
#define QPHY_LOCK_DETECT_CONFIG1			0x80
#define QPHY_LOCK_DETECT_CONFIG2			0x84
#define QPHY_LOCK_DETECT_CONFIG3			0x88
#define QPHY_PWRUP_RESET_DLY_TIME_AUXCLK		0xa0
#define QPHY_LP_WAKEUP_DLY_TIME_AUXCLK			0xa4
#define QPHY_PLL_LOCK_CHK_DLY_TIME_AUXCLK_LSB		0x1A8
#define QPHY_OSC_DTCT_ACTIONS				0x1AC
#define QPHY_RX_SIGDET_LVL				0x1D8
#define QPHY_L1SS_WAKEUP_DLY_TIME_AUXCLK_LSB		0x1DC
#define QPHY_L1SS_WAKEUP_DLY_TIME_AUXCLK_MSB		0x1E0

/* Only for QMP V3 & V4 PHY - DP COM registers */
#define QPHY_V3_DP_COM_PHY_MODE_CTRL			0x00
#define QPHY_V3_DP_COM_SW_RESET				0x04
#define QPHY_V3_DP_COM_POWER_DOWN_CTRL			0x08
#define QPHY_V3_DP_COM_SWI_CTRL				0x0c
#define QPHY_V3_DP_COM_TYPEC_CTRL			0x10
#define QPHY_V3_DP_COM_TYPEC_PWRDN_CTRL			0x14
#define QPHY_V3_DP_COM_RESET_OVRD_CTRL			0x1c

/* Only for QMP V3 PHY - QSERDES COM registers */
#define QSERDES_V3_COM_ATB_SEL1				0x000
#define QSERDES_V3_COM_ATB_SEL2				0x004
#define QSERDES_V3_COM_FREQ_UPDATE			0x008
#define QSERDES_V3_COM_BG_TIMER				0x00c
#define QSERDES_V3_COM_SSC_EN_CENTER			0x010
#define QSERDES_V3_COM_SSC_ADJ_PER1			0x014
#define QSERDES_V3_COM_SSC_ADJ_PER2			0x018
#define QSERDES_V3_COM_SSC_PER1				0x01c
#define QSERDES_V3_COM_SSC_PER2				0x020
#define QSERDES_V3_COM_SSC_STEP_SIZE1			0x024
#define QSERDES_V3_COM_SSC_STEP_SIZE2			0x028
#define QSERDES_V3_COM_BIAS_EN_CLKBUFLR_EN		0x034
# define QSERDES_V3_COM_BIAS_EN				0x0001
# define QSERDES_V3_COM_BIAS_EN_MUX			0x0002
# define QSERDES_V3_COM_CLKBUF_R_EN			0x0004
# define QSERDES_V3_COM_CLKBUF_L_EN			0x0008
# define QSERDES_V3_COM_EN_SYSCLK_TX_SEL		0x0010
# define QSERDES_V3_COM_CLKBUF_RX_DRIVE_L		0x0020
# define QSERDES_V3_COM_CLKBUF_RX_DRIVE_R		0x0040
#define QSERDES_V3_COM_CLK_ENABLE1			0x038
#define QSERDES_V3_COM_SYS_CLK_CTRL			0x03c
#define QSERDES_V3_COM_SYSCLK_BUF_ENABLE		0x040
#define QSERDES_V3_COM_PLL_IVCO				0x048
#define QSERDES_V3_COM_LOCK_CMP1_MODE0			0x098
#define QSERDES_V3_COM_LOCK_CMP2_MODE0			0x09c
#define QSERDES_V3_COM_LOCK_CMP3_MODE0			0x0a0
#define QSERDES_V3_COM_LOCK_CMP1_MODE1			0x0a4
#define QSERDES_V3_COM_LOCK_CMP2_MODE1			0x0a8
#define QSERDES_V3_COM_LOCK_CMP3_MODE1			0x0ac
#define QSERDES_V3_COM_CLK_EP_DIV			0x05c
#define QSERDES_V3_COM_CP_CTRL_MODE0			0x060
#define QSERDES_V3_COM_CP_CTRL_MODE1			0x064
#define QSERDES_V3_COM_PLL_RCTRL_MODE0			0x068
#define QSERDES_V3_COM_PLL_RCTRL_MODE1			0x06c
#define QSERDES_V3_COM_PLL_CCTRL_MODE0			0x070
#define QSERDES_V3_COM_PLL_CCTRL_MODE1			0x074
#define QSERDES_V3_COM_SYSCLK_EN_SEL			0x080
#define QSERDES_V3_COM_RESETSM_CNTRL			0x088
#define QSERDES_V3_COM_RESETSM_CNTRL2			0x08c
#define QSERDES_V3_COM_LOCK_CMP_EN			0x090
#define QSERDES_V3_COM_LOCK_CMP_CFG			0x094
#define QSERDES_V3_COM_DEC_START_MODE0			0x0b0
#define QSERDES_V3_COM_DEC_START_MODE1			0x0b4
#define QSERDES_V3_COM_DIV_FRAC_START1_MODE0		0x0b8
#define QSERDES_V3_COM_DIV_FRAC_START2_MODE0		0x0bc
#define QSERDES_V3_COM_DIV_FRAC_START3_MODE0		0x0c0
#define QSERDES_V3_COM_DIV_FRAC_START1_MODE1		0x0c4
#define QSERDES_V3_COM_DIV_FRAC_START2_MODE1		0x0c8
#define QSERDES_V3_COM_DIV_FRAC_START3_MODE1		0x0cc
#define QSERDES_V3_COM_INTEGLOOP_INITVAL		0x0d0
#define QSERDES_V3_COM_INTEGLOOP_GAIN0_MODE0		0x0d8
#define QSERDES_V3_COM_INTEGLOOP_GAIN1_MODE0		0x0dc
#define QSERDES_V3_COM_INTEGLOOP_GAIN0_MODE1		0x0e0
#define QSERDES_V3_COM_INTEGLOOP_GAIN1_MODE1		0x0e4
#define QSERDES_V3_COM_VCO_TUNE_CTRL			0x0ec
#define QSERDES_V3_COM_VCO_TUNE_MAP			0x0f0
#define QSERDES_V3_COM_VCO_TUNE1_MODE0			0x0f4
#define QSERDES_V3_COM_VCO_TUNE2_MODE0			0x0f8
#define QSERDES_V3_COM_VCO_TUNE1_MODE1			0x0fc
#define QSERDES_V3_COM_VCO_TUNE2_MODE1			0x100
#define QSERDES_V3_COM_VCO_TUNE_INITVAL1		0x104
#define QSERDES_V3_COM_VCO_TUNE_INITVAL2		0x108
#define QSERDES_V3_COM_VCO_TUNE_TIMER1			0x11c
#define QSERDES_V3_COM_VCO_TUNE_TIMER2			0x120
#define QSERDES_V3_COM_CLK_SELECT			0x138
#define QSERDES_V3_COM_HSCLK_SEL			0x13c
#define QSERDES_V3_COM_CORECLK_DIV_MODE0		0x148
#define QSERDES_V3_COM_CORECLK_DIV_MODE1		0x14c
#define QSERDES_V3_COM_CORE_CLK_EN			0x154
#define QSERDES_V3_COM_C_READY_STATUS			0x158
#define QSERDES_V3_COM_CMN_CONFIG			0x15c
#define QSERDES_V3_COM_SVS_MODE_CLK_SEL			0x164
#define QSERDES_V3_COM_DEBUG_BUS0			0x168
#define QSERDES_V3_COM_DEBUG_BUS1			0x16c
#define QSERDES_V3_COM_DEBUG_BUS2			0x170
#define QSERDES_V3_COM_DEBUG_BUS3			0x174
#define QSERDES_V3_COM_DEBUG_BUS_SEL			0x178
#define QSERDES_V3_COM_CMN_MODE				0x184

/* Only for QMP V3 PHY - TX registers */
#define QSERDES_V3_TX_BIST_MODE_LANENO			0x000
#define QSERDES_V3_TX_CLKBUF_ENABLE			0x008
#define QSERDES_V3_TX_TX_EMP_POST1_LVL			0x00c
# define DP_PHY_TXn_TX_EMP_POST1_LVL_MASK		0x001f
# define DP_PHY_TXn_TX_EMP_POST1_LVL_MUX_EN		0x0020

#define QSERDES_V3_TX_TX_DRV_LVL			0x01c
# define DP_PHY_TXn_TX_DRV_LVL_MASK			0x001f
# define DP_PHY_TXn_TX_DRV_LVL_MUX_EN			0x0020

#define QSERDES_V3_TX_RESET_TSYNC_EN			0x024
#define QSERDES_V3_TX_PRE_STALL_LDO_BOOST_EN		0x028

#define QSERDES_V3_TX_TX_BAND				0x02c
#define QSERDES_V3_TX_SLEW_CNTL				0x030
#define QSERDES_V3_TX_INTERFACE_SELECT			0x034
#define QSERDES_V3_TX_RES_CODE_LANE_TX			0x03c
#define QSERDES_V3_TX_RES_CODE_LANE_RX			0x040
#define QSERDES_V3_TX_RES_CODE_LANE_OFFSET_TX		0x044
#define QSERDES_V3_TX_RES_CODE_LANE_OFFSET_RX		0x048
#define QSERDES_V3_TX_DEBUG_BUS_SEL			0x058
#define QSERDES_V3_TX_TRANSCEIVER_BIAS_EN		0x05c
#define QSERDES_V3_TX_HIGHZ_DRVR_EN			0x060
#define QSERDES_V3_TX_TX_POL_INV			0x064
#define QSERDES_V3_TX_PARRATE_REC_DETECT_IDLE_EN	0x068
#define QSERDES_V3_TX_LANE_MODE_1			0x08c
#define QSERDES_V3_TX_RCV_DETECT_LVL_2			0x0a4
#define QSERDES_V3_TX_TRAN_DRVR_EMP_EN			0x0c0
#define QSERDES_V3_TX_TX_INTERFACE_MODE			0x0c4
#define QSERDES_V3_TX_VMODE_CTRL1			0x0f0

/* Only for QMP V3 PHY - RX registers */
#define QSERDES_V3_RX_UCDR_FO_GAIN			0x008
#define QSERDES_V3_RX_UCDR_SO_GAIN_HALF			0x00c
#define QSERDES_V3_RX_UCDR_SO_GAIN			0x014
#define QSERDES_V3_RX_UCDR_SVS_SO_GAIN_HALF		0x024
#define QSERDES_V3_RX_UCDR_SVS_SO_GAIN_QUARTER		0x028
#define QSERDES_V3_RX_UCDR_SVS_SO_GAIN			0x02c
#define QSERDES_V3_RX_UCDR_FASTLOCK_FO_GAIN		0x030
#define QSERDES_V3_RX_UCDR_SO_SATURATION_AND_ENABLE	0x034
#define QSERDES_V3_RX_UCDR_FASTLOCK_COUNT_LOW		0x03c
#define QSERDES_V3_RX_UCDR_FASTLOCK_COUNT_HIGH		0x040
#define QSERDES_V3_RX_UCDR_PI_CONTROLS			0x044
#define QSERDES_V3_RX_RX_TERM_BW			0x07c
#define QSERDES_V3_RX_VGA_CAL_CNTRL1			0x0bc
#define QSERDES_V3_RX_VGA_CAL_CNTRL2			0x0c0
#define QSERDES_V3_RX_RX_EQ_GAIN2_LSB			0x0c8
#define QSERDES_V3_RX_RX_EQ_GAIN2_MSB			0x0cc
#define QSERDES_V3_RX_RX_EQU_ADAPTOR_CNTRL2		0x0d4
#define QSERDES_V3_RX_RX_EQU_ADAPTOR_CNTRL3		0x0d8
#define QSERDES_V3_RX_RX_EQU_ADAPTOR_CNTRL4		0x0dc
#define QSERDES_V3_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1	0x0f8
#define QSERDES_V3_RX_RX_OFFSET_ADAPTOR_CNTRL2		0x0fc
#define QSERDES_V3_RX_SIGDET_ENABLES			0x100
#define QSERDES_V3_RX_SIGDET_CNTRL			0x104
#define QSERDES_V3_RX_SIGDET_LVL			0x108
#define QSERDES_V3_RX_SIGDET_DEGLITCH_CNTRL		0x10c
#define QSERDES_V3_RX_RX_BAND				0x110
#define QSERDES_V3_RX_RX_INTERFACE_MODE			0x11c
#define QSERDES_V3_RX_RX_MODE_00			0x164
#define QSERDES_V3_RX_RX_MODE_01			0x168

/* Only for QMP V3 PHY - PCS registers */
#define QPHY_V3_PCS_POWER_DOWN_CONTROL			0x004
#define QPHY_V3_PCS_TXMGN_V0				0x00c
#define QPHY_V3_PCS_TXMGN_V1				0x010
#define QPHY_V3_PCS_TXMGN_V2				0x014
#define QPHY_V3_PCS_TXMGN_V3				0x018
#define QPHY_V3_PCS_TXMGN_V4				0x01c
#define QPHY_V3_PCS_TXMGN_LS				0x020
#define QPHY_V3_PCS_TX_LARGE_AMP_DRV_LVL		0x02c
#define QPHY_V3_PCS_TX_SMALL_AMP_DRV_LVL		0x034
#define QPHY_V3_PCS_TXDEEMPH_M6DB_V0			0x024
#define QPHY_V3_PCS_TXDEEMPH_M3P5DB_V0			0x028
#define QPHY_V3_PCS_TXDEEMPH_M6DB_V1			0x02c
#define QPHY_V3_PCS_TXDEEMPH_M3P5DB_V1			0x030
#define QPHY_V3_PCS_TXDEEMPH_M6DB_V2			0x034
#define QPHY_V3_PCS_TXDEEMPH_M3P5DB_V2			0x038
#define QPHY_V3_PCS_TXDEEMPH_M6DB_V3			0x03c
#define QPHY_V3_PCS_TXDEEMPH_M3P5DB_V3			0x040
#define QPHY_V3_PCS_TXDEEMPH_M6DB_V4			0x044
#define QPHY_V3_PCS_TXDEEMPH_M3P5DB_V4			0x048
#define QPHY_V3_PCS_TXDEEMPH_M6DB_LS			0x04c
#define QPHY_V3_PCS_TXDEEMPH_M3P5DB_LS			0x050
#define QPHY_V3_PCS_ENDPOINT_REFCLK_DRIVE		0x054
#define QPHY_V3_PCS_RX_IDLE_DTCT_CNTRL			0x058
#define QPHY_V3_PCS_RATE_SLEW_CNTRL			0x05c
#define QPHY_V3_PCS_POWER_STATE_CONFIG1			0x060
#define QPHY_V3_PCS_POWER_STATE_CONFIG2			0x064
#define QPHY_V3_PCS_POWER_STATE_CONFIG4			0x06c
#define QPHY_V3_PCS_RCVR_DTCT_DLY_P1U2_L		0x070
#define QPHY_V3_PCS_RCVR_DTCT_DLY_P1U2_H		0x074
#define QPHY_V3_PCS_RCVR_DTCT_DLY_U3_L			0x078
#define QPHY_V3_PCS_RCVR_DTCT_DLY_U3_H			0x07c
#define QPHY_V3_PCS_LOCK_DETECT_CONFIG1			0x080
#define QPHY_V3_PCS_LOCK_DETECT_CONFIG2			0x084
#define QPHY_V3_PCS_LOCK_DETECT_CONFIG3			0x088
#define QPHY_V3_PCS_TSYNC_RSYNC_TIME			0x08c
#define QPHY_V3_PCS_PWRUP_RESET_DLY_TIME_AUXCLK		0x0a0
#define QPHY_V3_PCS_LP_WAKEUP_DLY_TIME_AUXCLK		0x0a4
#define QPHY_V3_PCS_PLL_LOCK_CHK_DLY_TIME		0x0a8
#define QPHY_V3_PCS_LFPS_TX_ECSTART_EQTLOCK		0x0b0
#define QPHY_V3_PCS_RXEQTRAINING_WAIT_TIME		0x0b8
#define QPHY_V3_PCS_RXEQTRAINING_RUN_TIME		0x0bc
#define QPHY_V3_PCS_FLL_CNTRL1				0x0c4
#define QPHY_V3_PCS_FLL_CNTRL2				0x0c8
#define QPHY_V3_PCS_FLL_CNT_VAL_L			0x0cc
#define QPHY_V3_PCS_FLL_CNT_VAL_H_TOL			0x0d0
#define QPHY_V3_PCS_FLL_MAN_CODE			0x0d4
#define QPHY_V3_PCS_RX_SYM_RESYNC_CTRL			0x134
#define QPHY_V3_PCS_RX_MIN_HIBERN8_TIME			0x138
#define QPHY_V3_PCS_RX_SIGDET_CTRL1			0x13c
#define QPHY_V3_PCS_RX_SIGDET_CTRL2			0x140
#define QPHY_V3_PCS_LP_WAKEUP_DLY_TIME_AUXCLK_MSB	0x1a8
#define QPHY_V3_PCS_OSC_DTCT_ACTIONS			0x1ac
#define QPHY_V3_PCS_SIGDET_CNTRL			0x1b0
#define QPHY_V3_PCS_TX_MID_TERM_CTRL1			0x1bc
#define QPHY_V3_PCS_MULTI_LANE_CTRL1			0x1c4
#define QPHY_V3_PCS_RX_SIGDET_LVL			0x1d8
#define QPHY_V3_PCS_L1SS_WAKEUP_DLY_TIME_AUXCLK_LSB	0x1dc
#define QPHY_V3_PCS_L1SS_WAKEUP_DLY_TIME_AUXCLK_MSB	0x1e0
#define QPHY_V3_PCS_REFGEN_REQ_CONFIG1			0x20c
#define QPHY_V3_PCS_REFGEN_REQ_CONFIG2			0x210

/* Only for QMP V3 PHY - PCS_MISC registers */
#define QPHY_V3_PCS_MISC_CLAMP_ENABLE			0x0c
#define QPHY_V3_PCS_MISC_OSC_DTCT_CONFIG2		0x2c
#define QPHY_V3_PCS_MISC_PCIE_INT_AUX_CLK_CONFIG1	0x44
#define QPHY_V3_PCS_MISC_OSC_DTCT_MODE2_CONFIG2		0x54
#define QPHY_V3_PCS_MISC_OSC_DTCT_MODE2_CONFIG4		0x5c
#define QPHY_V3_PCS_MISC_OSC_DTCT_MODE2_CONFIG5		0x60

/* Only for QMP V3 PHY - DP PHY registers */
#define QSERDES_V3_DP_PHY_REVISION_ID0			0x000
#define QSERDES_V3_DP_PHY_REVISION_ID1			0x004
#define QSERDES_V3_DP_PHY_REVISION_ID2			0x008
#define QSERDES_V3_DP_PHY_REVISION_ID3			0x00c
#define QSERDES_V3_DP_PHY_CFG				0x010
#define QSERDES_V3_DP_PHY_PD_CTL			0x018
# define DP_PHY_PD_CTL_PWRDN				0x001
# define DP_PHY_PD_CTL_PSR_PWRDN			0x002
# define DP_PHY_PD_CTL_AUX_PWRDN			0x004
# define DP_PHY_PD_CTL_LANE_0_1_PWRDN			0x008
# define DP_PHY_PD_CTL_LANE_2_3_PWRDN			0x010
# define DP_PHY_PD_CTL_PLL_PWRDN			0x020
# define DP_PHY_PD_CTL_DP_CLAMP_EN			0x040
#define QSERDES_V3_DP_PHY_MODE				0x01c
#define QSERDES_V3_DP_PHY_AUX_CFG0			0x020
#define QSERDES_V3_DP_PHY_AUX_CFG1			0x024
#define QSERDES_V3_DP_PHY_AUX_CFG2			0x028
#define QSERDES_V3_DP_PHY_AUX_CFG3			0x02c
#define QSERDES_V3_DP_PHY_AUX_CFG4			0x030
#define QSERDES_V3_DP_PHY_AUX_CFG5			0x034
#define QSERDES_V3_DP_PHY_AUX_CFG6			0x038
#define QSERDES_V3_DP_PHY_AUX_CFG7			0x03c
#define QSERDES_V3_DP_PHY_AUX_CFG8			0x040
#define QSERDES_V3_DP_PHY_AUX_CFG9			0x044

#define QSERDES_V3_DP_PHY_AUX_INTERRUPT_MASK		0x048
# define PHY_AUX_STOP_ERR_MASK				0x01
# define PHY_AUX_DEC_ERR_MASK				0x02
# define PHY_AUX_SYNC_ERR_MASK				0x04
# define PHY_AUX_ALIGN_ERR_MASK				0x08
# define PHY_AUX_REQ_ERR_MASK				0x10

#define QSERDES_V3_DP_PHY_AUX_INTERRUPT_CLEAR		0x04c
#define QSERDES_V3_DP_PHY_AUX_BIST_CFG			0x050

#define QSERDES_V3_DP_PHY_VCO_DIV			0x064
#define QSERDES_V3_DP_PHY_TX0_TX1_LANE_CTL		0x06c
#define QSERDES_V3_DP_PHY_TX2_TX3_LANE_CTL		0x088

#define QSERDES_V3_DP_PHY_SPARE0			0x0ac
#define DP_PHY_SPARE0_MASK				0x0f
#define DP_PHY_SPARE0_ORIENTATION_INFO_SHIFT		0x04(0x0004)

#define QSERDES_V3_DP_PHY_STATUS			0x0c0

/* Only for QMP V4 PHY - QSERDES COM registers */
#define QSERDES_V4_COM_SSC_EN_CENTER			0x010
#define QSERDES_V4_COM_SSC_PER1				0x01c
#define QSERDES_V4_COM_SSC_PER2				0x020
#define QSERDES_V4_COM_SSC_STEP_SIZE1_MODE0		0x024
#define QSERDES_V4_COM_SSC_STEP_SIZE2_MODE0		0x028
#define QSERDES_V4_COM_SSC_STEP_SIZE1_MODE1		0x030
#define QSERDES_V4_COM_SSC_STEP_SIZE2_MODE1		0x034
#define QSERDES_V4_COM_CLK_ENABLE1			0x048
#define QSERDES_V4_COM_SYSCLK_BUF_ENABLE		0x050
#define QSERDES_V4_COM_PLL_IVCO				0x058
#define QSERDES_V4_COM_CMN_IPTRIM			0x060
#define QSERDES_V4_COM_CP_CTRL_MODE0			0x074
#define QSERDES_V4_COM_CP_CTRL_MODE1			0x078
#define QSERDES_V4_COM_PLL_RCTRL_MODE0			0x07c
#define QSERDES_V4_COM_PLL_RCTRL_MODE1			0x080
#define QSERDES_V4_COM_PLL_CCTRL_MODE0			0x084
#define QSERDES_V4_COM_PLL_CCTRL_MODE1			0x088
#define QSERDES_V4_COM_SYSCLK_EN_SEL			0x094
#define QSERDES_V4_COM_LOCK_CMP_EN			0x0a4
#define QSERDES_V4_COM_LOCK_CMP1_MODE0			0x0ac
#define QSERDES_V4_COM_LOCK_CMP2_MODE0			0x0b0
#define QSERDES_V4_COM_LOCK_CMP1_MODE1			0x0b4
#define QSERDES_V4_COM_DEC_START_MODE0			0x0bc
#define QSERDES_V4_COM_LOCK_CMP2_MODE1			0x0b8
#define QSERDES_V4_COM_DEC_START_MODE1			0x0c4
#define QSERDES_V4_COM_DIV_FRAC_START1_MODE0		0x0cc
#define QSERDES_V4_COM_DIV_FRAC_START2_MODE0		0x0d0
#define QSERDES_V4_COM_DIV_FRAC_START3_MODE0		0x0d4
#define QSERDES_V4_COM_DIV_FRAC_START1_MODE1		0x0d8
#define QSERDES_V4_COM_DIV_FRAC_START2_MODE1		0x0dc
#define QSERDES_V4_COM_DIV_FRAC_START3_MODE1		0x0e0
#define QSERDES_V4_COM_VCO_TUNE_MAP			0x10c
#define QSERDES_V4_COM_VCO_TUNE1_MODE0			0x110
#define QSERDES_V4_COM_VCO_TUNE2_MODE0			0x114
#define QSERDES_V4_COM_VCO_TUNE1_MODE1			0x118
#define QSERDES_V4_COM_VCO_TUNE2_MODE1			0x11c
#define QSERDES_V4_COM_VCO_TUNE_INITVAL2		0x124
#define QSERDES_V4_COM_CLK_SELECT			0x154
#define QSERDES_V4_COM_HSCLK_SEL			0x158
#define QSERDES_V4_COM_HSCLK_HS_SWITCH_SEL		0x15c
#define QSERDES_V4_COM_CORECLK_DIV_MODE1		0x16c
#define QSERDES_V4_COM_SVS_MODE_CLK_SEL			0x184
#define QSERDES_V4_COM_BIN_VCOCAL_CMP_CODE1_MODE0	0x1ac
#define QSERDES_V4_COM_BIN_VCOCAL_CMP_CODE2_MODE0	0x1b0
#define QSERDES_V4_COM_BIN_VCOCAL_CMP_CODE1_MODE1	0x1b4
#define QSERDES_V4_COM_BIN_VCOCAL_HSCLK_SEL		0x1bc
#define QSERDES_V4_COM_BIN_VCOCAL_CMP_CODE2_MODE1	0x1b8

/* Only for QMP V4 PHY - TX registers */
#define QSERDES_V4_TX_RES_CODE_LANE_TX			0x34
#define QSERDES_V4_TX_RES_CODE_LANE_RX			0x38
#define QSERDES_V4_TX_RES_CODE_LANE_OFFSET_TX 		0x3c
#define QSERDES_V4_TX_RES_CODE_LANE_OFFSET_RX 		0x40
#define QSERDES_V4_TX_LANE_MODE_1			0x84
#define QSERDES_V4_TX_LANE_MODE_2			0x88
#define QSERDES_V4_TX_RCV_DETECT_LVL_2			0x9c
#define QSERDES_V4_TX_PWM_GEAR_1_DIVIDER_BAND0_1	0xd8
#define QSERDES_V4_TX_PWM_GEAR_2_DIVIDER_BAND0_1	0xdC
#define QSERDES_V4_TX_PWM_GEAR_3_DIVIDER_BAND0_1	0xe0
#define QSERDES_V4_TX_PWM_GEAR_4_DIVIDER_BAND0_1	0xe4
#define QSERDES_V4_TX_TRAN_DRVR_EMP_EN			0xb8
#define QSERDES_V4_TX_PI_QEC_CTRL		0x104

/* Only for QMP V4 PHY - RX registers */
#define QSERDES_V4_RX_UCDR_FO_GAIN			0x008
#define QSERDES_V4_RX_UCDR_SO_GAIN			0x014
#define QSERDES_V4_RX_UCDR_FASTLOCK_FO_GAIN		0x030
#define QSERDES_V4_RX_UCDR_SO_SATURATION_AND_ENABLE	0x034
#define QSERDES_V4_RX_UCDR_FASTLOCK_COUNT_LOW		0x03c
#define QSERDES_V4_RX_UCDR_FASTLOCK_COUNT_HIGH		0x040
#define QSERDES_V4_RX_UCDR_PI_CONTROLS			0x044
#define QSERDES_V4_RX_UCDR_PI_CTRL2			0x048
#define QSERDES_V4_RX_UCDR_SB2_THRESH1			0x04c
#define QSERDES_V4_RX_UCDR_SB2_THRESH2			0x050
#define QSERDES_V4_RX_UCDR_SB2_GAIN1			0x054
#define QSERDES_V4_RX_UCDR_SB2_GAIN2			0x058
#define QSERDES_V4_RX_AUX_DATA_TCOARSE_TFINE			0x060
#define QSERDES_V4_RX_RCLK_AUXDATA_SEL			0x064
#define QSERDES_V4_RX_AC_JTAG_ENABLE			0x068
#define QSERDES_V4_RX_AC_JTAG_MODE			0x078
#define QSERDES_V4_RX_RX_TERM_BW			0x080
#define QSERDES_V4_RX_VGA_CAL_CNTRL1			0x0d4
#define QSERDES_V4_RX_VGA_CAL_CNTRL2			0x0d8
#define QSERDES_V4_RX_GM_CAL				0x0dc
#define QSERDES_V4_RX_RX_EQU_ADAPTOR_CNTRL1		0x0e8
#define QSERDES_V4_RX_RX_EQU_ADAPTOR_CNTRL2		0x0ec
#define QSERDES_V4_RX_RX_EQU_ADAPTOR_CNTRL3		0x0f0
#define QSERDES_V4_RX_RX_EQU_ADAPTOR_CNTRL4		0x0f4
#define QSERDES_V4_RX_RX_IDAC_TSETTLE_LOW		0x0f8
#define QSERDES_V4_RX_RX_IDAC_TSETTLE_HIGH		0x0fc
#define QSERDES_V4_RX_RX_IDAC_MEASURE_TIME		0x100
#define QSERDES_V4_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1	0x110
#define QSERDES_V4_RX_RX_OFFSET_ADAPTOR_CNTRL2		0x114
#define QSERDES_V4_RX_SIGDET_ENABLES			0x118
#define QSERDES_V4_RX_SIGDET_CNTRL			0x11c
#define QSERDES_V4_RX_SIGDET_LVL			0x120
#define QSERDES_V4_RX_SIGDET_DEGLITCH_CNTRL		0x124
#define QSERDES_V4_RX_RX_BAND				0x128
#define QSERDES_V4_RX_RX_MODE_00_LOW			0x170
#define QSERDES_V4_RX_RX_MODE_00_HIGH			0x174
#define QSERDES_V4_RX_RX_MODE_00_HIGH2			0x178
#define QSERDES_V4_RX_RX_MODE_00_HIGH3			0x17c
#define QSERDES_V4_RX_RX_MODE_00_HIGH4			0x180
#define QSERDES_V4_RX_RX_MODE_01_LOW			0x184
#define QSERDES_V4_RX_RX_MODE_01_HIGH			0x188
#define QSERDES_V4_RX_RX_MODE_01_HIGH2			0x18c
#define QSERDES_V4_RX_RX_MODE_01_HIGH3			0x190
#define QSERDES_V4_RX_RX_MODE_01_HIGH4			0x194
#define QSERDES_V4_RX_RX_MODE_10_LOW			0x198
#define QSERDES_V4_RX_RX_MODE_10_HIGH			0x19c
#define QSERDES_V4_RX_RX_MODE_10_HIGH2			0x1a0
#define QSERDES_V4_RX_RX_MODE_10_HIGH3			0x1a4
#define QSERDES_V4_RX_RX_MODE_10_HIGH4			0x1a8
#define QSERDES_V4_RX_DFE_EN_TIMER			0x1b4
#define QSERDES_V4_RX_DFE_CTLE_POST_CAL_OFFSET		0x1b8
#define QSERDES_V4_RX_DCC_CTRL1				0x1bc
#define QSERDES_V4_RX_VTH_CODE				0x1c4

/* Only for QMP V4 PHY - UFS PCS registers */
#define QPHY_V4_PCS_UFS_PHY_START				0x000
#define QPHY_V4_PCS_UFS_POWER_DOWN_CONTROL			0x004
#define QPHY_V4_PCS_UFS_SW_RESET				0x008
#define QPHY_V4_PCS_UFS_TIMER_20US_CORECLK_STEPS_MSB		0x00c
#define QPHY_V4_PCS_UFS_TIMER_20US_CORECLK_STEPS_LSB		0x010
#define QPHY_V4_PCS_UFS_PLL_CNTL				0x02c
#define QPHY_V4_PCS_UFS_TX_LARGE_AMP_DRV_LVL			0x030
#define QPHY_V4_PCS_UFS_TX_SMALL_AMP_DRV_LVL			0x038
#define QPHY_V4_PCS_UFS_BIST_FIXED_PAT_CTRL			0x060
#define QPHY_V4_PCS_UFS_TX_HSGEAR_CAPABILITY			0x074
#define QPHY_V4_PCS_UFS_RX_HSGEAR_CAPABILITY			0x0b4
#define QPHY_V4_PCS_UFS_DEBUG_BUS_CLKSEL			0x124
#define QPHY_V4_PCS_UFS_LINECFG_DISABLE				0x148
#define QPHY_V4_PCS_UFS_RX_MIN_HIBERN8_TIME			0x150
#define QPHY_V4_PCS_UFS_RX_SIGDET_CTRL2				0x158
#define QPHY_V4_PCS_UFS_TX_PWM_GEAR_BAND			0x160
#define QPHY_V4_PCS_UFS_TX_HS_GEAR_BAND				0x168
#define QPHY_V4_PCS_UFS_READY_STATUS			0x180
#define QPHY_V4_PCS_UFS_TX_MID_TERM_CTRL1			0x1d8
#define QPHY_V4_PCS_UFS_MULTI_LANE_CTRL1			0x1e0

/* PCIE GEN3 COM registers */
#define PCIE_GEN3_QHP_COM_SSC_EN_CENTER			0x14
#define PCIE_GEN3_QHP_COM_SSC_PER1			0x20
#define PCIE_GEN3_QHP_COM_SSC_PER2			0x24
#define PCIE_GEN3_QHP_COM_SSC_STEP_SIZE1		0x28
#define PCIE_GEN3_QHP_COM_SSC_STEP_SIZE2		0x2c
#define PCIE_GEN3_QHP_COM_SSC_STEP_SIZE1_MODE1		0x34
#define PCIE_GEN3_QHP_COM_SSC_STEP_SIZE2_MODE1		0x38
#define PCIE_GEN3_QHP_COM_BIAS_EN_CKBUFLR_EN		0x54
#define PCIE_GEN3_QHP_COM_CLK_ENABLE1			0x58
#define PCIE_GEN3_QHP_COM_LOCK_CMP1_MODE0		0x6c
#define PCIE_GEN3_QHP_COM_LOCK_CMP2_MODE0		0x70
#define PCIE_GEN3_QHP_COM_LOCK_CMP1_MODE1		0x78
#define PCIE_GEN3_QHP_COM_LOCK_CMP2_MODE1		0x7c
#define PCIE_GEN3_QHP_COM_BGV_TRIM			0x98
#define PCIE_GEN3_QHP_COM_CP_CTRL_MODE0			0xb4
#define PCIE_GEN3_QHP_COM_CP_CTRL_MODE1			0xb8
#define PCIE_GEN3_QHP_COM_PLL_RCTRL_MODE0		0xc0
#define PCIE_GEN3_QHP_COM_PLL_RCTRL_MODE1		0xc4
#define PCIE_GEN3_QHP_COM_PLL_CCTRL_MODE0		0xcc
#define PCIE_GEN3_QHP_COM_PLL_CCTRL_MODE1		0xd0
#define PCIE_GEN3_QHP_COM_SYSCLK_EN_SEL			0xdc
#define PCIE_GEN3_QHP_COM_RESTRIM_CTRL2			0xf0
#define PCIE_GEN3_QHP_COM_LOCK_CMP_EN			0xf8
#define PCIE_GEN3_QHP_COM_DEC_START_MODE0		0x100
#define PCIE_GEN3_QHP_COM_DEC_START_MODE1		0x108
#define PCIE_GEN3_QHP_COM_DIV_FRAC_START1_MODE0		0x11c
#define PCIE_GEN3_QHP_COM_DIV_FRAC_START2_MODE0		0x120
#define PCIE_GEN3_QHP_COM_DIV_FRAC_START3_MODE0		0x124
#define PCIE_GEN3_QHP_COM_DIV_FRAC_START1_MODE1		0x128
#define PCIE_GEN3_QHP_COM_DIV_FRAC_START2_MODE1		0x12c
#define PCIE_GEN3_QHP_COM_DIV_FRAC_START3_MODE1		0x130
#define PCIE_GEN3_QHP_COM_INTEGLOOP_GAIN0_MODE0		0x150
#define PCIE_GEN3_QHP_COM_INTEGLOOP_GAIN0_MODE1		0x158
#define PCIE_GEN3_QHP_COM_VCO_TUNE_MAP			0x178
#define PCIE_GEN3_QHP_COM_BG_CTRL			0x1c8
#define PCIE_GEN3_QHP_COM_CLK_SELECT			0x1cc
#define PCIE_GEN3_QHP_COM_HSCLK_SEL1			0x1d0
#define PCIE_GEN3_QHP_COM_CORECLK_DIV			0x1e0
#define PCIE_GEN3_QHP_COM_CORE_CLK_EN			0x1e8
#define PCIE_GEN3_QHP_COM_CMN_CONFIG			0x1f0
#define PCIE_GEN3_QHP_COM_SVS_MODE_CLK_SEL		0x1fc
#define PCIE_GEN3_QHP_COM_CORECLK_DIV_MODE1		0x21c
#define PCIE_GEN3_QHP_COM_CMN_MODE			0x224
#define PCIE_GEN3_QHP_COM_VREGCLK_DIV1			0x228
#define PCIE_GEN3_QHP_COM_VREGCLK_DIV2			0x22c

/* PCIE GEN3 QHP Lane registers */
#define PCIE_GEN3_QHP_L0_DRVR_CTRL0			0xc
#define PCIE_GEN3_QHP_L0_DRVR_CTRL1			0x10
#define PCIE_GEN3_QHP_L0_DRVR_CTRL2			0x14
#define PCIE_GEN3_QHP_L0_DRVR_TAP_EN			0x18
#define PCIE_GEN3_QHP_L0_TX_BAND_MODE			0x60
#define PCIE_GEN3_QHP_L0_LANE_MODE			0x64
#define PCIE_GEN3_QHP_L0_PARALLEL_RATE			0x7c
#define PCIE_GEN3_QHP_L0_CML_CTRL_MODE0			0xc0
#define PCIE_GEN3_QHP_L0_CML_CTRL_MODE1			0xc4
#define PCIE_GEN3_QHP_L0_CML_CTRL_MODE2			0xc8
#define PCIE_GEN3_QHP_L0_PREAMP_CTRL_MODE1		0xd0
#define PCIE_GEN3_QHP_L0_PREAMP_CTRL_MODE2		0xd4
#define PCIE_GEN3_QHP_L0_MIXER_CTRL_MODE0		0xd8
#define PCIE_GEN3_QHP_L0_MIXER_CTRL_MODE1		0xdc
#define PCIE_GEN3_QHP_L0_MIXER_CTRL_MODE2		0xe0
#define PCIE_GEN3_QHP_L0_CTLE_THRESH_DFE		0xfc
#define PCIE_GEN3_QHP_L0_CGA_THRESH_DFE			0x100
#define PCIE_GEN3_QHP_L0_RXENGINE_EN0			0x108
#define PCIE_GEN3_QHP_L0_CTLE_TRAIN_TIME		0x114
#define PCIE_GEN3_QHP_L0_CTLE_DFE_OVRLP_TIME		0x118
#define PCIE_GEN3_QHP_L0_DFE_REFRESH_TIME		0x11c
#define PCIE_GEN3_QHP_L0_DFE_ENABLE_TIME		0x120
#define PCIE_GEN3_QHP_L0_VGA_GAIN			0x124
#define PCIE_GEN3_QHP_L0_DFE_GAIN			0x128
#define PCIE_GEN3_QHP_L0_EQ_GAIN			0x130
#define PCIE_GEN3_QHP_L0_OFFSET_GAIN			0x134
#define PCIE_GEN3_QHP_L0_PRE_GAIN			0x138
#define PCIE_GEN3_QHP_L0_VGA_INITVAL			0x13c
#define PCIE_GEN3_QHP_L0_EQ_INTVAL			0x154
#define PCIE_GEN3_QHP_L0_EDAC_INITVAL			0x160
#define PCIE_GEN3_QHP_L0_RXEQ_INITB0			0x168
#define PCIE_GEN3_QHP_L0_RXEQ_INITB1			0x16c
#define PCIE_GEN3_QHP_L0_RCVRDONE_THRESH1		0x178
#define PCIE_GEN3_QHP_L0_RXEQ_CTRL			0x180
#define PCIE_GEN3_QHP_L0_UCDR_FO_GAIN_MODE0		0x184
#define PCIE_GEN3_QHP_L0_UCDR_FO_GAIN_MODE1		0x188
#define PCIE_GEN3_QHP_L0_UCDR_FO_GAIN_MODE2		0x18c
#define PCIE_GEN3_QHP_L0_UCDR_SO_GAIN_MODE0		0x190
#define PCIE_GEN3_QHP_L0_UCDR_SO_GAIN_MODE1		0x194
#define PCIE_GEN3_QHP_L0_UCDR_SO_GAIN_MODE2		0x198
#define PCIE_GEN3_QHP_L0_UCDR_SO_CONFIG			0x19c
#define PCIE_GEN3_QHP_L0_RX_BAND			0x1a4
#define PCIE_GEN3_QHP_L0_RX_RCVR_PATH1_MODE0		0x1c0
#define PCIE_GEN3_QHP_L0_RX_RCVR_PATH1_MODE1		0x1c4
#define PCIE_GEN3_QHP_L0_RX_RCVR_PATH1_MODE2		0x1c8
#define PCIE_GEN3_QHP_L0_SIGDET_ENABLES			0x230
#define PCIE_GEN3_QHP_L0_SIGDET_CNTRL			0x234
#define PCIE_GEN3_QHP_L0_SIGDET_DEGLITCH_CNTRL		0x238
#define PCIE_GEN3_QHP_L0_DCC_GAIN			0x2a4
#define PCIE_GEN3_QHP_L0_RSM_START			0x2a8
#define PCIE_GEN3_QHP_L0_RX_EN_SIGNAL			0x2ac
#define PCIE_GEN3_QHP_L0_PSM_RX_EN_CAL			0x2b0
#define PCIE_GEN3_QHP_L0_RX_MISC_CNTRL0			0x2b8
#define PCIE_GEN3_QHP_L0_TS0_TIMER			0x2c0
#define PCIE_GEN3_QHP_L0_DLL_HIGHDATARATE		0x2c4
#define PCIE_GEN3_QHP_L0_RX_RESETCODE_OFFSET		0x2cc

/* PCIE GEN3 PCS registers */
#define PCIE_GEN3_QHP_PHY_TXMGN_MAIN_V0_M3P5DB		0x2c
#define PCIE_GEN3_QHP_PHY_TXMGN_POST_V0_M3P5DB		0x40
#define PCIE_GEN3_QHP_PHY_TXMGN_MAIN_V0_M6DB		0x54
#define PCIE_GEN3_QHP_PHY_TXMGN_POST_V0_M6DB		0x68
#define PCIE_GEN3_QHP_PHY_POWER_STATE_CONFIG		0x15c
#define PCIE_GEN3_QHP_PHY_POWER_STATE_CONFIG5		0x16c
#define PCIE_GEN3_QHP_PHY_PCS_TX_RX_CONFIG		0x174

/* Only for QMP V4 PHY - USB/PCIe PCS registers */
#define QPHY_V4_PCS_SW_RESET				0x000
#define QPHY_V4_PCS_REVISION_ID0			0x004
#define QPHY_V4_PCS_REVISION_ID1			0x008
#define QPHY_V4_PCS_REVISION_ID2			0x00c
#define QPHY_V4_PCS_REVISION_ID3			0x010
#define QPHY_V4_PCS_PCS_STATUS1				0x014
#define QPHY_V4_PCS_PCS_STATUS2				0x018
#define QPHY_V4_PCS_PCS_STATUS3				0x01c
#define QPHY_V4_PCS_PCS_STATUS4				0x020
#define QPHY_V4_PCS_PCS_STATUS5				0x024
#define QPHY_V4_PCS_PCS_STATUS6				0x028
#define QPHY_V4_PCS_PCS_STATUS7				0x02c
#define QPHY_V4_PCS_DEBUG_BUS_0_STATUS			0x030
#define QPHY_V4_PCS_DEBUG_BUS_1_STATUS			0x034
#define QPHY_V4_PCS_DEBUG_BUS_2_STATUS			0x038
#define QPHY_V4_PCS_DEBUG_BUS_3_STATUS			0x03c
#define QPHY_V4_PCS_POWER_DOWN_CONTROL			0x040
#define QPHY_V4_PCS_START_CONTROL			0x044
#define QPHY_V4_PCS_INSIG_SW_CTRL1			0x048
#define QPHY_V4_PCS_INSIG_SW_CTRL2			0x04c
#define QPHY_V4_PCS_INSIG_SW_CTRL3			0x050
#define QPHY_V4_PCS_INSIG_SW_CTRL4			0x054
#define QPHY_V4_PCS_INSIG_SW_CTRL5			0x058
#define QPHY_V4_PCS_INSIG_SW_CTRL6			0x05c
#define QPHY_V4_PCS_INSIG_SW_CTRL7			0x060
#define QPHY_V4_PCS_INSIG_SW_CTRL8			0x064
#define QPHY_V4_PCS_INSIG_MX_CTRL1			0x068
#define QPHY_V4_PCS_INSIG_MX_CTRL2			0x06c
#define QPHY_V4_PCS_INSIG_MX_CTRL3			0x070
#define QPHY_V4_PCS_INSIG_MX_CTRL4			0x074
#define QPHY_V4_PCS_INSIG_MX_CTRL5			0x078
#define QPHY_V4_PCS_INSIG_MX_CTRL7			0x07c
#define QPHY_V4_PCS_INSIG_MX_CTRL8			0x080
#define QPHY_V4_PCS_OUTSIG_SW_CTRL1			0x084
#define QPHY_V4_PCS_OUTSIG_MX_CTRL1			0x088
#define QPHY_V4_PCS_CLAMP_ENABLE			0x08c
#define QPHY_V4_PCS_POWER_STATE_CONFIG1			0x090
#define QPHY_V4_PCS_POWER_STATE_CONFIG2			0x094
#define QPHY_V4_PCS_FLL_CNTRL1				0x098
#define QPHY_V4_PCS_FLL_CNTRL2				0x09c
#define QPHY_V4_PCS_FLL_CNT_VAL_L			0x0a0
#define QPHY_V4_PCS_FLL_CNT_VAL_H_TOL			0x0a4
#define QPHY_V4_PCS_FLL_MAN_CODE			0x0a8
#define QPHY_V4_PCS_TEST_CONTROL1			0x0ac
#define QPHY_V4_PCS_TEST_CONTROL2			0x0b0
#define QPHY_V4_PCS_TEST_CONTROL3			0x0b4
#define QPHY_V4_PCS_TEST_CONTROL4			0x0b8
#define QPHY_V4_PCS_TEST_CONTROL5			0x0bc
#define QPHY_V4_PCS_TEST_CONTROL6			0x0c0
#define QPHY_V4_PCS_LOCK_DETECT_CONFIG1			0x0c4
#define QPHY_V4_PCS_LOCK_DETECT_CONFIG2			0x0c8
#define QPHY_V4_PCS_LOCK_DETECT_CONFIG3			0x0cc
#define QPHY_V4_PCS_LOCK_DETECT_CONFIG4			0x0d0
#define QPHY_V4_PCS_LOCK_DETECT_CONFIG5			0x0d4
#define QPHY_V4_PCS_LOCK_DETECT_CONFIG6			0x0d8
#define QPHY_V4_PCS_REFGEN_REQ_CONFIG1			0x0dc
#define QPHY_V4_PCS_REFGEN_REQ_CONFIG2			0x0e0
#define QPHY_V4_PCS_REFGEN_REQ_CONFIG3			0x0e4
#define QPHY_V4_PCS_BIST_CTRL				0x0e8
#define QPHY_V4_PCS_PRBS_POLY0				0x0ec
#define QPHY_V4_PCS_PRBS_POLY1				0x0f0
#define QPHY_V4_PCS_FIXED_PAT0				0x0f4
#define QPHY_V4_PCS_FIXED_PAT1				0x0f8
#define QPHY_V4_PCS_FIXED_PAT2				0x0fc
#define QPHY_V4_PCS_FIXED_PAT3				0x100
#define QPHY_V4_PCS_FIXED_PAT4				0x104
#define QPHY_V4_PCS_FIXED_PAT5				0x108
#define QPHY_V4_PCS_FIXED_PAT6				0x10c
#define QPHY_V4_PCS_FIXED_PAT7				0x110
#define QPHY_V4_PCS_FIXED_PAT8				0x114
#define QPHY_V4_PCS_FIXED_PAT9				0x118
#define QPHY_V4_PCS_FIXED_PAT10				0x11c
#define QPHY_V4_PCS_FIXED_PAT11				0x120
#define QPHY_V4_PCS_FIXED_PAT12				0x124
#define QPHY_V4_PCS_FIXED_PAT13				0x128
#define QPHY_V4_PCS_FIXED_PAT14				0x12c
#define QPHY_V4_PCS_FIXED_PAT15				0x130
#define QPHY_V4_PCS_TXMGN_CONFIG			0x134
#define QPHY_V4_PCS_G12S1_TXMGN_V0			0x138
#define QPHY_V4_PCS_G12S1_TXMGN_V1			0x13c
#define QPHY_V4_PCS_G12S1_TXMGN_V2			0x140
#define QPHY_V4_PCS_G12S1_TXMGN_V3			0x144
#define QPHY_V4_PCS_G12S1_TXMGN_V4			0x148
#define QPHY_V4_PCS_G12S1_TXMGN_V0_RS			0x14c
#define QPHY_V4_PCS_G12S1_TXMGN_V1_RS			0x150
#define QPHY_V4_PCS_G12S1_TXMGN_V2_RS			0x154
#define QPHY_V4_PCS_G12S1_TXMGN_V3_RS			0x158
#define QPHY_V4_PCS_G12S1_TXMGN_V4_RS			0x15c
#define QPHY_V4_PCS_G3S2_TXMGN_MAIN			0x160
#define QPHY_V4_PCS_G3S2_TXMGN_MAIN_RS			0x164
#define QPHY_V4_PCS_G12S1_TXDEEMPH_M6DB			0x168
#define QPHY_V4_PCS_G12S1_TXDEEMPH_M3P5DB		0x16c
#define QPHY_V4_PCS_G3S2_PRE_GAIN			0x170
#define QPHY_V4_PCS_G3S2_POST_GAIN			0x174
#define QPHY_V4_PCS_G3S2_PRE_POST_OFFSET		0x178
#define QPHY_V4_PCS_G3S2_PRE_GAIN_RS			0x17c
#define QPHY_V4_PCS_G3S2_POST_GAIN_RS			0x180
#define QPHY_V4_PCS_G3S2_PRE_POST_OFFSET_RS		0x184
#define QPHY_V4_PCS_RX_SIGDET_LVL			0x188
#define QPHY_V4_PCS_RX_SIGDET_DTCT_CNTRL		0x18c
#define QPHY_V4_PCS_RCVR_DTCT_DLY_P1U2_L		0x190
#define QPHY_V4_PCS_RCVR_DTCT_DLY_P1U2_H		0x194
#define QPHY_V4_PCS_RATE_SLEW_CNTRL1			0x198
#define QPHY_V4_PCS_RATE_SLEW_CNTRL2			0x19c
#define QPHY_V4_PCS_PWRUP_RESET_DLY_TIME_AUXCLK		0x1a0
#define QPHY_V4_PCS_P2U3_WAKEUP_DLY_TIME_AUXCLK_L	0x1a4
#define QPHY_V4_PCS_P2U3_WAKEUP_DLY_TIME_AUXCLK_H	0x1a8
#define QPHY_V4_PCS_TSYNC_RSYNC_TIME			0x1ac
#define QPHY_V4_PCS_CDR_RESET_TIME			0x1b0
#define QPHY_V4_PCS_TSYNC_DLY_TIME			0x1b4
#define QPHY_V4_PCS_ELECIDLE_DLY_SEL			0x1b8
#define QPHY_V4_PCS_CMN_ACK_OUT_SEL			0x1bc
#define QPHY_V4_PCS_ALIGN_DETECT_CONFIG1		0x1c0
#define QPHY_V4_PCS_ALIGN_DETECT_CONFIG2		0x1c4
#define QPHY_V4_PCS_ALIGN_DETECT_CONFIG3		0x1c8
#define QPHY_V4_PCS_ALIGN_DETECT_CONFIG4		0x1cc
#define QPHY_V4_PCS_PCS_TX_RX_CONFIG			0x1d0
#define QPHY_V4_PCS_RX_IDLE_DTCT_CNTRL			0x1d4
#define QPHY_V4_PCS_RX_DCC_CAL_CONFIG			0x1d8
#define QPHY_V4_PCS_EQ_CONFIG1				0x1dc
#define QPHY_V4_PCS_EQ_CONFIG2				0x1e0
#define QPHY_V4_PCS_EQ_CONFIG3				0x1e4
#define QPHY_V4_PCS_EQ_CONFIG4				0x1e8
#define QPHY_V4_PCS_EQ_CONFIG5				0x1ec
#define QPHY_V4_PCS_USB3_POWER_STATE_CONFIG1		0x300
#define QPHY_V4_PCS_USB3_AUTONOMOUS_MODE_STATUS		0x304
#define QPHY_V4_PCS_USB3_AUTONOMOUS_MODE_CTRL		0x308
#define QPHY_V4_PCS_USB3_AUTONOMOUS_MODE_CTRL2		0x30c
#define QPHY_V4_PCS_USB3_LFPS_RXTERM_IRQ_SOURCE_STATUS	0x310
#define QPHY_V4_PCS_USB3_LFPS_RXTERM_IRQ_CLEAR		0x314
#define QPHY_V4_PCS_USB3_LFPS_DET_HIGH_COUNT_VAL	0x318
#define QPHY_V4_PCS_USB3_LFPS_TX_ECSTART		0x31c
#define QPHY_V4_PCS_USB3_LFPS_PER_TIMER_VAL		0x320
#define QPHY_V4_PCS_USB3_LFPS_TX_END_CNT_U3_START	0x324
#define QPHY_V4_PCS_USB3_RXEQTRAINING_LOCK_TIME		0x328
#define QPHY_V4_PCS_USB3_RXEQTRAINING_WAIT_TIME		0x32c
#define QPHY_V4_PCS_USB3_RXEQTRAINING_CTLE_TIME		0x330
#define QPHY_V4_PCS_USB3_RXEQTRAINING_WAIT_TIME_S2	0x334
#define QPHY_V4_PCS_USB3_RXEQTRAINING_DFE_TIME_S2	0x338
#define QPHY_V4_PCS_USB3_RCVR_DTCT_DLY_U3_L		0x33c
#define QPHY_V4_PCS_USB3_RCVR_DTCT_DLY_U3_H		0x340
#define QPHY_V4_PCS_USB3_ARCVR_DTCT_EN_PERIOD		0x344
#define QPHY_V4_PCS_USB3_ARCVR_DTCT_CM_DLY		0x348
#define QPHY_V4_PCS_USB3_TXONESZEROS_RUN_LENGTH		0x34c
#define QPHY_V4_PCS_USB3_ALFPS_DEGLITCH_VAL		0x350
#define QPHY_V4_PCS_USB3_SIGDET_STARTUP_TIMER_VAL	0x354
#define QPHY_V4_PCS_USB3_TEST_CONTROL			0x358

/* Only for QMP V4 PHY - UNI has 0x300 offset for PCS_USB3 regs */
#define QPHY_V4_PCS_USB3_UNI_LFPS_DET_HIGH_COUNT_VAL	0x618
#define QPHY_V4_PCS_USB3_UNI_RXEQTRAINING_DFE_TIME_S2	0x638

/* Only for QMP V4 PHY - PCS_MISC registers */
#define QPHY_V4_PCS_MISC_TYPEC_CTRL			0x00
#define QPHY_V4_PCS_MISC_TYPEC_PWRDN_CTRL		0x04
#define QPHY_V4_PCS_MISC_PCS_MISC_CONFIG1		0x08
#define QPHY_V4_PCS_MISC_CLAMP_ENABLE			0x0c
#define QPHY_V4_PCS_MISC_TYPEC_STATUS			0x10
#define QPHY_V4_PCS_MISC_PLACEHOLDER_STATUS		0x14

/* Only for QMP V4 PHY - PCS_PCIE registers (same as PCS_MISC?) */
#define QPHY_V4_PCS_PCIE_POWER_STATE_CONFIG2		0x0c
#define QPHY_V4_PCS_PCIE_POWER_STATE_CONFIG4		0x14
#define QPHY_V4_PCS_PCIE_ENDPOINT_REFCLK_DRIVE		0x1c
#define QPHY_V4_PCS_PCIE_L1P1_WAKEUP_DLY_TIME_AUXCLK_L	0x40
#define QPHY_V4_PCS_PCIE_L1P2_WAKEUP_DLY_TIME_AUXCLK_L	0x48
#define QPHY_V4_PCS_PCIE_INT_AUX_CLK_CONFIG1		0x50
#define QPHY_V4_PCS_PCIE_OSC_DTCT_ACTIONS		0x90
#define QPHY_V4_PCS_PCIE_EQ_CONFIG2			0xa4
#define QPHY_V4_PCS_PCIE_PRESET_P6_P7_PRE		0xb4
#define QPHY_V4_PCS_PCIE_PRESET_P10_PRE			0xbc
#define QPHY_V4_PCS_PCIE_PRESET_P10_POST		0xe0

#endif
