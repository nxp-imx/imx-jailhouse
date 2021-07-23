/*
 * iMX8MN target - Little Kernel
 *
 * Copyright 2020-2021 NXP
 *
 * Authors:
 *  Frank Wang <lei.wang_15@nxp.com>
 *  Fabrice Goucem <fabrice.goucem@nxp.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <jailhouse/types.h>
#include <jailhouse/cell-config.h>

#define ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])
#define IRQMAP(irqn, pinbase) ((1) << (((irqn) - (pinbase)) % 32))
#define IRQ_BASE 32
#define VPCI_IRQ_BASE 124

typedef enum IRQn
{
    GPR_IRQ_IRQn                = IRQ_BASE + 0,
    DAP_IRQn                    = IRQ_BASE + 1,
    SDMA1_IRQn                  = IRQ_BASE + 2,
    GPU3D_IRQn                  = IRQ_BASE + 3,
    SNVS_IRQn                   = IRQ_BASE + 4,
    LCDIF_IRQn                  = IRQ_BASE + 5,
    SPDIF1_IRQn                 = IRQ_BASE + 6,
    Reserved23_IRQn             = IRQ_BASE + 7,
    Reserved24_IRQn             = IRQ_BASE + 8,
    QOS_IRQn                    = IRQ_BASE + 9,
    WDOG3_IRQn                  = IRQ_BASE + 10,
    HS_CP1_IRQn                 = IRQ_BASE + 11,
    APBHDMA_IRQn                = IRQ_BASE + 12,
    Reserved29_IRQn             = IRQ_BASE + 13,
    BCH_IRQn                    = IRQ_BASE + 14,
    GPMI_IRQn                   = IRQ_BASE + 15,
    ISI_CH0_IRQn                = IRQ_BASE + 16,
    MIPI_CSI1_IRQn              = IRQ_BASE + 17,
    MIPI_DSI_IRQn               = IRQ_BASE + 18,
    SNVS_Consolidated_IRQn      = IRQ_BASE + 19,
    SNVS_Security_IRQn          = IRQ_BASE + 20,
    CSU_IRQn                    = IRQ_BASE + 21,
    USDHC1_IRQn                 = IRQ_BASE + 22,
    USDHC2_IRQn                 = IRQ_BASE + 23,
    USDHC3_IRQn                 = IRQ_BASE + 24,
    Reserved41_IRQn             = IRQ_BASE + 25,
    UART1_IRQn                  = IRQ_BASE + 26,
    UART2_IRQn                  = IRQ_BASE + 27,
    UART3_IRQn                  = IRQ_BASE + 28,
    UART4_IRQn                  = IRQ_BASE + 29,
    Reserved46_IRQn             = IRQ_BASE + 30,
    ECSPI1_IRQn                 = IRQ_BASE + 31,
    ECSPI2_IRQn                 = IRQ_BASE + 32,
    ECSPI3_IRQn                 = IRQ_BASE + 33,
    SDMA3_IRQn                  = IRQ_BASE + 34,
    I2C1_IRQn                   = IRQ_BASE + 35,
    I2C2_IRQn                   = IRQ_BASE + 36,
    I2C3_IRQn                   = IRQ_BASE + 37,
    I2C4_IRQn                   = IRQ_BASE + 38,
    RDC_IRQn                    = IRQ_BASE + 39,
    USB1_IRQn                   = IRQ_BASE + 40,
    Reserved57_IRQn             = IRQ_BASE + 41,
    ISI_CH1_IRQn                = IRQ_BASE + 42,
    ISI_CH2_IRQn                = IRQ_BASE + 43,
    PDM_HWVAD_EVENT_IRQn        = IRQ_BASE + 44,
    PDM_HWVAD_ERROR_IRQn        = IRQ_BASE + 45,
    GPT6_IRQn                   = IRQ_BASE + 46,
    SCTR_IRQ0_IRQn              = IRQ_BASE + 47,
    SCTR_IRQ1_IRQn              = IRQ_BASE + 48,
    TEMPMON_LOW_IRQn            = IRQ_BASE + 49,
    I2S3_IRQn                   = IRQ_BASE + 50,
    GPT5_IRQn                   = IRQ_BASE + 51,
    GPT4_IRQn                   = IRQ_BASE + 52,
    GPT3_IRQn                   = IRQ_BASE + 53,
    GPT2_IRQn                   = IRQ_BASE + 54,
    GPT1_IRQn                   = IRQ_BASE + 55,
    GPIO1_INT7_IRQn             = IRQ_BASE + 56,
    GPIO1_INT6_IRQn             = IRQ_BASE + 57,
    GPIO1_INT5_IRQn             = IRQ_BASE + 58,
    GPIO1_INT4_IRQn             = IRQ_BASE + 59,
    GPIO1_INT3_IRQn             = IRQ_BASE + 60,
    GPIO1_INT2_IRQn             = IRQ_BASE + 61,
    GPIO1_INT1_IRQn             = IRQ_BASE + 62,
    GPIO1_INT0_IRQn             = IRQ_BASE + 63,
    GPIO1_Combined_0_15_IRQn    = IRQ_BASE + 64,
    GPIO1_Combined_16_31_IRQn   = IRQ_BASE + 65,
    GPIO2_Combined_0_15_IRQn    = IRQ_BASE + 66,
    GPIO2_Combined_16_31_IRQn   = IRQ_BASE + 67,
    GPIO3_Combined_0_15_IRQn    = IRQ_BASE + 68,
    GPIO3_Combined_16_31_IRQn   = IRQ_BASE + 69,
    GPIO4_Combined_0_15_IRQn    = IRQ_BASE + 70,
    GPIO4_Combined_16_31_IRQn   = IRQ_BASE + 71,
    GPIO5_Combined_0_15_IRQn    = IRQ_BASE + 72,
    GPIO5_Combined_16_31_IRQn   = IRQ_BASE + 73,
    Reserved90_IRQn             = IRQ_BASE + 74,
    Reserved91_IRQn             = IRQ_BASE + 75,
    Reserved92_IRQn             = IRQ_BASE + 76,
    Reserved93_IRQn             = IRQ_BASE + 77,
    WDOG1_IRQn                  = IRQ_BASE + 78,
    WDOG2_IRQn                  = IRQ_BASE + 79,
    Reserved96_IRQn             = IRQ_BASE + 80,
    PWM1_IRQn                   = IRQ_BASE + 81,
    PWM2_IRQn                   = IRQ_BASE + 82,
    PWM3_IRQn                   = IRQ_BASE + 83,
    PWM4_IRQn                   = IRQ_BASE + 84,
    CCM_IRQ1_IRQn               = IRQ_BASE + 85,
    CCM_IRQ2_IRQn               = IRQ_BASE + 86,
    GPC_IRQn                    = IRQ_BASE + 87,
    MU_A53_IRQn                 = IRQ_BASE + 88,
    SRC_IRQn                    = IRQ_BASE + 89,
    I2S56_IRQn                  = IRQ_BASE + 90,
    RTIC_IRQn                   = IRQ_BASE + 91,
    CPU_PerformanceUnit_IRQn    = IRQ_BASE + 92,
    CPU_CTI_Trigger_IRQn        = IRQ_BASE + 93,
    SRC_Combined_IRQn           = IRQ_BASE + 94,
    Reserved111_IRQn            = IRQ_BASE + 95,
    I2S2_IRQn                   = IRQ_BASE + 96,
    MU_M7_IRQn                  = IRQ_BASE + 97,
    DDR_PerformanceMonitor_IRQn = IRQ_BASE + 98,
    DDR_IRQn                    = IRQ_BASE + 99,
    Reserved116_IRQn            = IRQ_BASE + 100,
    CPU_Error_AXI_IRQn          = IRQ_BASE + 101,
    CPU_Error_L2RAM_IRQn        = IRQ_BASE + 102,
    SDMA2_IRQn                  = IRQ_BASE + 103,
    SJC_IRQn                    = IRQ_BASE + 104,
    CAAM_IRQ0_IRQn              = IRQ_BASE + 105,
    CAAM_IRQ1_IRQn              = IRQ_BASE + 106,
    QSPI_IRQn                   = IRQ_BASE + 107,
    TZASC_IRQn                  = IRQ_BASE + 108,
    PDM_EVENT_IRQn              = IRQ_BASE + 109,
    PDM_ERROR_IRQn              = IRQ_BASE + 110,
    I2S7_IRQn                   = IRQ_BASE + 111,
    PERFMON1_IRQn               = IRQ_BASE + 112,
    PERFMON2_IRQn               = IRQ_BASE + 113,
    CAAM_IRQ2_IRQn              = IRQ_BASE + 114,
    CAAM_ERROR_IRQn             = IRQ_BASE + 115,
    HS_CP0_IRQn                 = IRQ_BASE + 116,
    CM7_CTI_IRQn                = IRQ_BASE + 117,
    ENET_MAC0_Rx_Tx_Done1_IRQn  = IRQ_BASE + 118,
    ENET_MAC0_Rx_Tx_Done2_IRQn  = IRQ_BASE + 119,
    ENET_IRQn                   = IRQ_BASE + 120,
    ENET_1588_IRQn              = IRQ_BASE + 121,
    ASRC_IRQn                   = IRQ_BASE + 122,
    Reserved139_IRQn            = IRQ_BASE + 123,
    Reserved140_IRQn            = IRQ_BASE + 124,
    Reserved141_IRQn            = IRQ_BASE + 125,
    ISI_CH3_IRQn                = IRQ_BASE + 126,
    Reserved143_IRQn            = IRQ_BASE + 127
} IRQn_Type;


struct {
	struct jailhouse_cell_desc cell;
	__u64 cpus[1];
	struct jailhouse_memory mem_regions[26];
	struct jailhouse_irqchip irqchips[2];
	struct jailhouse_pci_device pci_devices[1];
} __attribute__((packed)) config = {
	.cell = {
		.signature = JAILHOUSE_CELL_DESC_SIGNATURE,
		.revision = JAILHOUSE_CONFIG_REVISION,
		.name = "lk",
		.flags = JAILHOUSE_CELL_PASSIVE_COMMREG,

		.cpu_set_size = sizeof(config.cpus),
		.num_memory_regions = ARRAY_SIZE(config.mem_regions),
		.num_irqchips = ARRAY_SIZE(config.irqchips),
		.num_pci_devices = ARRAY_SIZE(config.pci_devices),
		.vpci_irq_base = VPCI_IRQ_BASE, /* Not include 32 base */
		.cpu_reset_address = 0x80010000,
	},

	.cpus = {
		0x3,
	},

	.mem_regions = {
		/* IVSHMEM shared memory regions (demo) */
		{
			.phys_start = 0xbbaf0000,
			.virt_start = 0xbbaf0000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0xbbaf1000,
			.virt_start = 0xbbaf1000,
			.size = 0x1ff000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0xbbcf0000,
			.virt_start = 0xbbcf0000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0xbbd00000,
			.virt_start = 0xbbd00000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* GPIO1 */ {
			.phys_start = 0x30200000,
			.virt_start = 0x30200000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* GPIO3 */ {
			.phys_start = 0x30220000,
			.virt_start = 0x30220000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* GPIO5 */ {
			.phys_start = 0x30240000,
			.virt_start = 0x30240000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* SPDIF1 */ {
			.phys_start = 0x30090000,
			.virt_start = 0x30090000,
			.size =	0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* I2S1 */ {
			.phys_start = 0x30010000,
			.virt_start = 0x30010000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* I2S3 */ {
			.phys_start = 0x30030000,
			.virt_start = 0x30030000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* I2S5 */ {
			.phys_start = 0x30050000,
			.virt_start = 0x30050000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* I2S6 */ {
			.phys_start = 0x30060000,
			.virt_start = 0x30060000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* MICFIL */ {
			.phys_start = 0x30080000,
			.virt_start = 0x30080000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* I2C3 */ {
			.phys_start = 0x30a40000,
			.virt_start = 0x30a40000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* UART4 */ {
			.phys_start = 0x30a60000,
			.virt_start = 0x30a60000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* IOMUXC */ {
			.phys_start = 0x30330000,
			.virt_start = 0x30330000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
			JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* IOMUXC GPR */ {
			.phys_start = 0x30340000,
			.virt_start = 0x30340000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* ANA_OSC */ {
			.phys_start = 0x30270000,
			.virt_start = 0x30270000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* OCOTP, ANATOP */ {
			.phys_start = 0x30350000,
			.virt_start = 0x30350000,
			.size = 0x20000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* CCM */ {
			.phys_start = 0x30380000,
			.virt_start = 0x30380000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* SDMA3 */ {
			.phys_start = 0x302b0000,
			.virt_start = 0x302b0000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* GPT1 */ {
			.phys_start = 0x302d0000,
			.virt_start = 0x302d0000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* GPT[456] */ {
			.phys_start = 0x306e0000,
			.virt_start = 0x306e0000,
			.size = 0x30000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* RAM */{
			/*
			 * Physical Address and size have to fit into inmate_reserved
			 * definition from reserved-memory node in Linux dts.
			 * Intermediate Physical Address (virt_start) is LK build
			 * dependency (MEMBASE) so value comes from there.
			 */
			.phys_start = 0x93c00000,
			.virt_start = 0x80000000,
			.size = 0x20000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_LOADABLE,
		},
		/* communication region */ {
			.virt_start = 0x60000000,
			.size = 0x00001000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_COMM_REGION,
		},
		/* SRAM: 256kB */ {
			.phys_start = 0x00900000,
			.virt_start = 0x00900000,
			.size = 0x00400000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_LOADABLE,
		},
	},

	.irqchips = {
		/* GIC */
		{
			.address = 0x38800000,
			.pin_base = 32,
			.pin_bitmap = {
				/* interrupts 32..63 */
				IRQMAP(UART4_IRQn, 32)
					| IRQMAP(SPDIF1_IRQn, 32),
				/* interrupts 64..95*/
				IRQMAP(I2C3_IRQn, 32)
					| IRQMAP(I2C4_IRQn, 32)
					| IRQMAP(PDM_HWVAD_EVENT_IRQn, 32)
					| IRQMAP(PDM_HWVAD_ERROR_IRQn, 32)
					| IRQMAP(I2S3_IRQn, 32)
					| IRQMAP(SDMA3_IRQn, 32),
				/* interrupts 96..127*/
				IRQMAP(I2S56_IRQn, 32),
				/* interrupts 128..159 */
				IRQMAP(PDM_EVENT_IRQn, 32)
					| IRQMAP(I2S7_IRQn, 32)
					| IRQMAP(SDMA2_IRQn, 32)
					| IRQMAP(I2S2_IRQn, 32)
					| IRQMAP(ASRC_IRQn, 32)
					| IRQMAP(PDM_ERROR_IRQn, 32),
			},
		},
		{
			.address = 0x38800000,
			.pin_base = 128,
			.pin_bitmap = {
				IRQMAP(VPCI_IRQ_BASE + 32, 128)
			},
		},
	},

	.pci_devices = {
		{
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 1,
			.bdf = 0 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 0,
			.shmem_dev_id = 1,
			.shmem_peers = 3,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_UNDEFINED,
		},
	},
};
