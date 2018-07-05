/*
 * i.MX8MQ Target
 *
 * Copyright 2017-2018 NXP
 *
 * Authors:
 *  Peng Fan <peng.fan@nxp.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 *
 * Reservation via device tree: reg = <0x0 0xffaf0000 0x0 0x510000>
 */

#include <jailhouse/types.h>
#include <jailhouse/cell-config.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

struct {
	struct jailhouse_system header;
	__u64 cpus[1];
	struct jailhouse_memory mem_regions[71];
	struct jailhouse_irqchip irqchips[3];
	struct jailhouse_pci_device pci_devices[2];
} __attribute__((packed)) config = {
	.header = {
		.signature = JAILHOUSE_SYSTEM_SIGNATURE,
		.revision = JAILHOUSE_CONFIG_REVISION,
		.hypervisor_memory = {
			.phys_start = 0xffc00000,
			.size =       0x00400000,
		},
		.debug_console = {
			.address = 0x30860000,
			.size = 0x1000,
			.flags = JAILHOUSE_CON1_TYPE_IMX |
				 JAILHOUSE_CON1_ACCESS_MMIO |
				 JAILHOUSE_CON1_REGDIST_4 |
				 JAILHOUSE_CON2_TYPE_ROOTPAGE,
		},
		.platform_info = {
			/*
			 * .pci_mmconfig_base is fixed; if you change it,
			 * update the value in mach.h
			 * (PCI_CFG_BASE) and regenerate the inmate library
			 */
			.pci_mmconfig_base = 0xbfb00000,
			.pci_mmconfig_end_bus = 0x0,
			.pci_is_virtual = 1,

			.arm = {
				.gic_version = 3,
				.gicd_base = 0x38800000,
				.gicr_base = 0x38880000,
				.maintenance_irq = 25,
			},
		},
		.root_cell = {
			.name = "imx8mq",

			.num_pci_devices = ARRAY_SIZE(config.pci_devices),
			.cpu_set_size = sizeof(config.cpus),
			.num_memory_regions = ARRAY_SIZE(config.mem_regions),
			.num_irqchips = ARRAY_SIZE(config.irqchips),
			.vpci_irq_base = 56, /* Not include 32 base */
		},
	},

	.cpus = {
		0xf,
	},

	.mem_regions = {
		/* SAI1 */ {
			.phys_start = 0x30010000,
			.virt_start = 0x30010000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* SAI3 */ {
			.phys_start = 0x30030000,
			.virt_start = 0x30030000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* SAI5 */ {
			.phys_start = 0x30040000,
			.virt_start = 0x30040000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* SAI4 */ {
			.phys_start = 0x30050000,
			.virt_start = 0x30050000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* GPIO1 */ {
			.phys_start = 0x30200000,
			.virt_start = 0x30200000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* GPIO2 */ {
			.phys_start = 0x30210000,
			.virt_start = 0x30210000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* GPIO3 */ {
			.phys_start = 0x30220000,
			.virt_start = 0x30220000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* GPIO4 */ {
			.phys_start = 0x30230000,
			.virt_start = 0x30230000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* GPIO5 */ {
			.phys_start = 0x30240000,
			.virt_start = 0x30240000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* TMU */ {
			.phys_start = 0x30260000,
			.virt_start = 0x30260000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* WDOG1 */ {
			.phys_start = 0x30280000,
			.virt_start = 0x30280000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* WDOG2 */ {
			.phys_start = 0x30290000,
			.virt_start = 0x30290000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* WDOG3 */ {
			.phys_start = 0x302a0000,
			.virt_start = 0x302a0000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* SDMA2 */ {
			.phys_start = 0x302c0000,
			.virt_start = 0x302c0000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* GPT1 */ {
			.phys_start = 0x302d0000,
			.virt_start = 0x302d0000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* LCDIF */ {
			.phys_start = 0x30320000,
			.virt_start = 0x30320000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* IOMUXC */ {
			.phys_start = 0x30330000,
			.virt_start = 0x30330000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* GPR */ {
			.phys_start = 0x30340000,
			.virt_start = 0x30340000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* OCOTP */ {
			.phys_start = 0x30350000,
			.virt_start = 0x30350000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* ANATOP */ {
			.phys_start = 0x30360000,
			.virt_start = 0x30360000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* SNVS */ {
			.phys_start = 0x30370000,
			.virt_start = 0x30370000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* CCM */ {
			.phys_start = 0x30380000,
			.virt_start = 0x30380000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* SRC */ {
			.phys_start = 0x30390000,
			.virt_start = 0x30390000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* PWM1 */ {
			.phys_start = 0x30660000,
			.virt_start = 0x30660000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* PWM2 */ {
			.phys_start = 0x30670000,
			.virt_start = 0x30670000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* PWM3 */ {
			.phys_start = 0x30680000,
			.virt_start = 0x30680000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* PWM4 */ {
			.phys_start = 0x30690000,
			.virt_start = 0x30690000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* SYS COUNTER */ {
			.phys_start = 0x306a0000,
			.virt_start = 0x306a0000,
			.size =	      0x30000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* SPDIF */ {
			.phys_start = 0x30810000,
			.virt_start = 0x30810000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* ECSPI1 */ {
			.phys_start = 0x30820000,
			.virt_start = 0x30820000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* ECSPI2 */ {
			.phys_start = 0x30830000,
			.virt_start = 0x30830000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* ECSPI3 */ {
			.phys_start = 0x30840000,
			.virt_start = 0x30840000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* UART1 */ {
			.phys_start = 0x30860000,
			.virt_start = 0x30860000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* UART3 */ {
			.phys_start = 0x30880000,
			.virt_start = 0x30880000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* UART2 */ {
			.phys_start = 0x30890000,
			.virt_start = 0x30890000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* SPDIF2 */ {
			.phys_start = 0x308a0000,
			.virt_start = 0x308a0000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* SAI2 */ {
			.phys_start = 0x308b0000,
			.virt_start = 0x308b0000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* SAI3 */ {
			.phys_start = 0x308c0000,
			.virt_start = 0x308c0000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* CAAM */ {
			.phys_start = 0x30900000,
			.virt_start = 0x30900000,
			.size =	      0x40000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* MIPI_DSI_PHY/BRIDGE */ {
			.phys_start = 0x30a00000,
			.virt_start = 0x30a00000,
			.size =	      0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* I2C1 */ {
			.phys_start = 0x30a20000,
			.virt_start = 0x30a20000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* I2C2 */ {
			.phys_start = 0x30a30000,
			.virt_start = 0x30a30000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* I2C3 */ {
			.phys_start = 0x30a40000,
			.virt_start = 0x30a40000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* I2C4 */ {
			.phys_start = 0x30a50000,
			.virt_start = 0x30a50000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* UART4 */ {
			.phys_start = 0x30a60000,
			.virt_start = 0x30a60000,
			.size =	      0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* MIPI_CSI_1 */ {
			.phys_start = 0x30a70000,
			.virt_start = 0x30a70000,
			.size =	      0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* CSI1_BRIDGE */ {
			.phys_start = 0x30a90000,
			.virt_start = 0x30a90000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* MU */ {
			.phys_start = 0x30aa0000,
			.virt_start = 0x30aa0000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* USDHC1 */ {
			.phys_start = 0x30b40000,
			.virt_start = 0x30b40000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* USDHC2 */ {
			.phys_start = 0x30b50000,
			.virt_start = 0x30b50000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* MIPI_CSI_2 */ {
			.phys_start = 0x30b60000,
			.virt_start = 0x30b60000,
			.size =	      0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* CSI2_BRIDGE */ {
			.phys_start = 0x30b80000,
			.virt_start = 0x30b80000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* QSPI1 */ {
			.phys_start = 0x30bb0000,
			.virt_start = 0x30bb0000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* SDMA1 */ {
			.phys_start = 0x30bd0000,
			.virt_start = 0x30bd0000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* FEC1 */ {
			.phys_start = 0x30be0000,
			.virt_start = 0x30be0000,
			.size =	      0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* HDMI */ {
			.phys_start = 0x32c00000,
			.virt_start = 0x32c00000,
			.size =	      0x100000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* IRQSTEER_DCSS DCSS */ {
			.phys_start = 0x32e00000,
			.virt_start = 0x32e00000,
			.size =	      0x30000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* DMA-APHB */ {
			.phys_start = 0x33000000,
			.virt_start = 0x33000000,
			.size =	      0x2000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* GPMI */ {
			.phys_start = 0x33002000,
			.virt_start = 0x33002000,
			.size =	      0x6000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* PCIE0 */ {
			.phys_start = 0x33800000,
			.virt_start = 0x33800000,
			.size =	      0x400000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* PCIE1 */ {
			.phys_start = 0x33c00000,
			.virt_start = 0x33c00000,
			.size =	      0x400000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* GPU */ {
			.phys_start = 0x38000000,
			.virt_start = 0x38000000,
			.size =	      0x40000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* USB1 */ {
			.phys_start = 0x38100000,
			.virt_start = 0x38100000,
			.size =	      0x100000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* USB3 */ {
			.phys_start = 0x38200000,
			.virt_start = 0x38200000,
			.size =	      0x100000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* VPU */ {
			.phys_start = 0x38300000,
			.virt_start = 0x38300000,
			.size =	      0x200000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* DDR_PMU */ {
			.phys_start = 0x3d800000,
			.virt_start = 0x3d800000,
			.size =	      0x400000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE,
		},
		/* RAM */ {
			.phys_start = 0x40000000,
			.virt_start = 0x40000000,
			.size = 0x7fb00000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE,
		},
		/* IVHSMEM shared memory region for 00:00.0 */ {
			.phys_start = 0xbfe00000,
			.virt_start = 0xbfe00000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE ,
		},
		/* IVHSMEM shared memory region for 00:01.0 */ {
			.phys_start = 0xbfd00000,
			.virt_start = 0xbfd00000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE ,
		},
		/* Linux Loader */{
			.phys_start = 0xbff00000,
			.virt_start = 0xbff00000,
			.size = 0x100000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE,
		},
		/* Inmate memory */{
			.phys_start = 0xc0000000,
			.virt_start = 0xc0000000,
			.size = 0x3fc00000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE,
		},
	},

	.irqchips = {
		/* GIC */ {
			.address = 0x38800000,
			.pin_base = 32,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
		/* GIC */ {
			.address = 0x38800000,
			.pin_base = 160,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
		/* GIC */ {
			.address = 0x38800000,
			.pin_base = 288,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
	},

	.pci_devices = {
		{
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.bdf = 0x0 << 3,
			.bar_mask = {
				0xffffff00, 0xffffffff, 0x00000000,
				0x00000000, 0x00000000, 0x00000000,
			},
			
			/*num_msix_vectors needs to be 0 for INTx operation*/
			.num_msix_vectors = 0,
			.shmem_region = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_UNDEFINED,
			.domain = 0x0,
		},
		{
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.bdf = 0x1 << 3,
			.bar_mask = {
				0xffffff00, 0xffffffff, 0x00000000,
				0x00000000, 0x00000000, 0x00000000,
			},
			
			/*num_msix_vectors needs to be 0 for INTx operation*/
			.num_msix_vectors = 0,
			.shmem_region = 3,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_UNDEFINED,
			.domain = 0x0,
		},
	},
};
