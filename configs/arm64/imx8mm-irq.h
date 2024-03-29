/*
 * iMX8MM irq declaration
 *
 * Copyright 2023 NXP
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#define IRQMAP(irqn, pinbase) ((1) << (((irqn) - (pinbase)) % 32))

/* Device specific interrupts */

typedef enum IRQn
{
  GPR_IRQ_IRQn                 = 32,
  DAP_IRQn                     = 33,
  SDMA1_IRQn                   = 34,
  GPU3D_IRQn                   = 35,
  SNVS_IRQn                    = 36,
  LCDIF_IRQn                   = 37,
  SPDIF1_IRQn                  = 38,
  VPU_G1_IRQn                  = 39,
  VPU_G2_IRQn                  = 40,
  QOS_IRQn                     = 41,
  WDOG3_IRQn                   = 42,
  HS_CP1_IRQn                  = 43,
  APBHDMA_IRQn                 = 44,
  Reserved29_IRQn              = 45,
  BCH_IRQn                     = 46,
  GPMI_IRQn                    = 47,
  CSI1_IRQn                    = 48,
  MIPI_CSI1_IRQn               = 49,
  MIPI_DSI_IRQn                = 50,
  SNVS_Consolidated_IRQn       = 51,
  SNVS_Security_IRQn           = 52,
  CSU_IRQn                     = 53,
  USDHC1_IRQn                  = 54,
  USDHC2_IRQn                  = 55,
  USDHC3_IRQn                  = 56,
  GPU2D_IRQn                   = 57,
  UART1_IRQn                   = 58,
  UART2_IRQn                   = 59,
  UART3_IRQn                   = 60,
  UART4_IRQn                   = 61,
  VPU_H1_IRQn                  = 62,
  ECSPI1_IRQn                  = 63,
  ECSPI2_IRQn                  = 64,
  ECSPI3_IRQn                  = 65,
  SDMA3_IRQn                   = 66,
  I2C1_IRQn                    = 67,
  I2C2_IRQn                    = 68,
  I2C3_IRQn                    = 69,
  I2C4_IRQn                    = 70,
  RDC_IRQn                     = 71,
  USB1_IRQn                    = 72,
  USB2_IRQn                    = 73,
  Reserved58_IRQn              = 74,
  Reserved59_IRQn              = 75,
  PDM_HWVAD_EVENT_IRQn         = 76,
  PDM_HWVAD_ERROR_IRQn         = 77,
  GPT6_IRQn                    = 78,
  SCTR_IRQ0_IRQn               = 79,
  SCTR_IRQ1_IRQn               = 80,
  TEMPMON_LOW_IRQn             = 81,
  I2S3_IRQn                    = 82,
  GPT5_IRQn                    = 83,
  GPT4_IRQn                    = 84,
  GPT3_IRQn                    = 85,
  GPT2_IRQn                    = 86,
  GPT1_IRQn                    = 87,
  GPIO1_INT7_IRQn              = 88,
  GPIO1_INT6_IRQn              = 89,
  GPIO1_INT5_IRQn              = 90,
  GPIO1_INT4_IRQn              = 91,
  GPIO1_INT3_IRQn              = 92,
  GPIO1_INT2_IRQn              = 93,
  GPIO1_INT1_IRQn              = 94,
  GPIO1_INT0_IRQn              = 95,
  GPIO1_Combined_0_15_IRQn     = 96,
  GPIO1_Combined_16_31_IRQn    = 97,
  GPIO2_Combined_0_15_IRQn     = 98,
  GPIO2_Combined_16_31_IRQn    = 99,
  GPIO3_Combined_0_15_IRQn     = 100,
  GPIO3_Combined_16_31_IRQn    = 101,
  GPIO4_Combined_0_15_IRQn     = 102,
  GPIO4_Combined_16_31_IRQn    = 103,
  GPIO5_Combined_0_15_IRQn     = 104,
  GPIO5_Combined_16_31_IRQn    = 105,
  Reserved90_IRQn              = 106,
  Reserved91_IRQn              = 107,
  Reserved92_IRQn              = 108,
  Reserved93_IRQn              = 109,
  WDOG1_IRQn                   = 110,
  WDOG2_IRQn                   = 111,
  Reserved96_IRQn              = 112,
  PWM1_IRQn                    = 113,
  PWM2_IRQn                    = 114,
  PWM3_IRQn                    = 115,
  PWM4_IRQn                    = 116,
  CCM_IRQ1_IRQn                = 117,
  CCM_IRQ2_IRQn                = 118,
  GPC_IRQn                     = 119,
  MU_A53_IRQn                  = 120,
  SRC_IRQn                     = 121,
  I2S56_IRQn                   = 122,
  RTIC_IRQn                    = 123,
  CPU_PerformanceUnit_IRQn     = 124,
  CPU_CTI_Trigger_IRQn         = 125,
  SRC_Combined_IRQn            = 126,
  I2S1_IRQn                    = 127,
  I2S2_IRQn                    = 128,
  MU_M4_IRQn                   = 129,
  DDR_PerformanceMonitor_IRQn  = 130,
  DDR_IRQn                     = 131,
  Reserved116_IRQn             = 132,
  CPU_Error_AXI_IRQn           = 133,
  CPU_Error_L2RAM_IRQn         = 134,
  SDMA2_IRQn                   = 135,
  SJC_IRQn                     = 136,
  CAAM_IRQ0_IRQn               = 137,
  CAAM_IRQ1_IRQn               = 138,
  QSPI_IRQn                    = 139,
  TZASC_IRQn                   = 140,
  PDM_EVENT_IRQn               = 141,
  PDM_ERROR_IRQn               = 142,
  Reserved127_IRQn             = 143,
  PERFMON1_IRQn                = 144,
  PERFMON2_IRQn                = 145,
  CAAM_IRQ2_IRQn               = 146,
  CAAM_ERROR_IRQn              = 147,
  HS_CP0_IRQn                  = 148,
  Reserved133_IRQn             = 149,
  ENET_MAC0_Rx_Tx_Done1_IRQn   = 150,
  ENET_MAC0_Rx_Tx_Done2_IRQn   = 151,
  ENET_IRQn                    = 152,
  ENET_1588_IRQn               = 153,
  PCIE_CTRL1_IRQ0_IRQn         = 154,
  PCIE_CTRL1_IRQ1_IRQn         = 155,
  PCIE_CTRL1_IRQ2_IRQn         = 156,
  PCIE_CTRL1_IRQ3_IRQn         = 157,
  Reserved142_IRQn             = 158,
  PCIE_CTRL1_IRQn              = 159
} IRQn_Type;


#define VPCI_IRQ_BASE 106

