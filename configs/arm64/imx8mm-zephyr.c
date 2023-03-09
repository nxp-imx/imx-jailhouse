/*
 * iMX8MM target - linux-demo
 *
 * Copyright 2019 NXP
 *
 * Authors:
 *  Peng Fan <peng.fan@nxp.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

/*
 * Boot 2nd Linux cmdline:
 * export PATH=$PATH:/usr/share/jailhouse/tools/
 * jailhouse cell linux imx8mm-linux-demo.cell Image -d fsl-imx8mm-evk-inmate.dtb -c "clk_ignore_unused console=ttymxc3,115200 earlycon=ec_imx6q,0x30890000,115200  root=/dev/mmcblk2p2 rootwait rw"
 */
#include <jailhouse/types.h>
#include <jailhouse/cell-config.h>

#include "imx8mm-irq.h"

struct {
	struct jailhouse_cell_desc cell;
	__u64 cpus[1];
	struct jailhouse_memory mem_regions[38];
	struct jailhouse_irqchip irqchips[1];
	struct jailhouse_pci_device pci_devices[2];
} __attribute__((packed)) config = {
	.cell = {
		.signature = JAILHOUSE_CELL_DESC_SIGNATURE,
		.revision = JAILHOUSE_CONFIG_REVISION,
		.architecture = JAILHOUSE_ARM64,
		.name = "zephyr-inmate-demo",
		.flags = JAILHOUSE_CELL_PASSIVE_COMMREG,

		.cpu_set_size = sizeof(config.cpus),
		.num_memory_regions = ARRAY_SIZE(config.mem_regions),
		.num_irqchips = ARRAY_SIZE(config.irqchips),
		.num_pci_devices = ARRAY_SIZE(config.pci_devices),
		.vpci_irq_base = 74, /* Not include 32 base */
		.cpu_reset_address = 0x93c00000,
	},

	.cpus = {
		0x3,
	},

	.mem_regions = {
		/* IVHSMEM shared memory region for 00:00.0 */ {
			.phys_start = 0xbba00000,
			.virt_start = 0xbba00000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0xbba01000,
			.virt_start = 0xbba01000,
			.size = 0x9000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0xbba0a000,
			.virt_start = 0xbba0a000,
			.size = 0x2000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0xbba0c000,
			.virt_start = 0xbba0c000,
			.size = 0x2000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0xbba0e000,
			.virt_start = 0xbba0e000,
			.size = 0x2000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_ROOTSHARED,
		},
		/* IVSHMEM shared memory regions for 00:01.0 (networking) */
		JAILHOUSE_SHMEM_NET_REGIONS(0xbbb00000, 1),
		/* GPIO1 */ {
			.phys_start = 0x30200000,
			.virt_start = 0x30200000,
			.size =          0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* GPIO3 */ {
			.phys_start = 0x30220000,
			.virt_start = 0x30220000,
			.size =          0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* GPIO5 */ {
			.phys_start = 0x30240000,
			.virt_start = 0x30240000,
			.size =       0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* I2S1 */ {
			.phys_start = 0x30010000,
			.virt_start = 0x30010000,
			.size =          0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* I2S2 */ {
			.phys_start = 0x30020000,
			.virt_start = 0x30020000,
			.size =          0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* I2S3 */ {
			.phys_start = 0x30030000,
			.virt_start = 0x30030000,
			.size =          0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* I2S5 */ {
			.phys_start = 0x30050000,
			.virt_start = 0x30050000,
			.size =          0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* I2S6 */ {
			.phys_start = 0x30060000,
			.virt_start = 0x30060000,
			.size =          0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* MICFIL */ {
			.phys_start = 0x30080000,
			.virt_start = 0x30080000,
			.size =          0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* I2C3 */ {
			.phys_start = 0x30A40000,
			.virt_start = 0x30A40000,
			.size =          0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* UART4 */ {
			.phys_start = 0x30a60000,
			.virt_start = 0x30a60000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* IOMUXC */ {
			.phys_start = 0x30330000,
			.virt_start = 0x30330000,
			.size =          0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* IOMUXC GPR */ {
			.phys_start = 0x30340000,
			.virt_start = 0x30340000,
			.size =          0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* ANA_OSC */ {
			.phys_start = 0x30270000,
			.virt_start = 0x30270000,
			.size =          0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* ANATOP */ {
			.phys_start = 0x30360000,
			.virt_start = 0x30360000,
			.size =          0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* CCM */ {
			.phys_start = 0x30380000,
			.virt_start = 0x30380000,
			.size =          0x10000,
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
		/* SDMA2 */ {
			.phys_start = 0x302c0000,
			.virt_start = 0x302c0000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* GPT1 */ {
			.phys_start = 0x302d0000,
			.virt_start = 0x302d0000,
			.size =       0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* GPT2 */ {
			.phys_start = 0x302e0000,
			.virt_start = 0x302e0000,
			.size =       0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* GPT3 */ {
			.phys_start = 0x302f0000,
			.virt_start = 0x302f0000,
			.size =       0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* GPT4 */ {
			.phys_start = 0x30700000,
			.virt_start = 0x30700000,
			.size =       0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* GPT5 */ {
			.phys_start = 0x306f0000,
			.virt_start = 0x306f0000,
			.size =       0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* GPT6 */ {
			.phys_start = 0x306e0000,
			.virt_start = 0x306e0000,
			.size =       0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* SPDIF1 */ {
			.phys_start = 0x30090000,
			.virt_start = 0x30090000,
			.size =       0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* RAM: Top at 4GB Space */ {
			.phys_start = 0xbb700000,
			.virt_start = 0,
			.size = 0x100000, /* 1M */
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_LOADABLE,
		},
		/* RAM */ {
			/*
			 * * We could not use 0x80000000 which conflicts with
			 * * COMM_REGION_BASE
			 * */
			.phys_start = 0x93c00000,
			.virt_start = 0x93c00000,
			.size = 0x24000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_DMA |
				JAILHOUSE_MEM_LOADABLE,
		},
		/* communication region */ {
			.virt_start = 0x80000000,
			.size = 0x00001000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_COMM_REGION,
		},
		/* SRAM: 256kB */ {
			.phys_start = 0x00900000,
			.virt_start = 0x00900000,
			.size = 0x400000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_LOADABLE,
		},
	},

	.irqchips = {
		{
			.address = 0x38800000,
			.pin_base = 32,
			.pin_bitmap = {
				/* interrupts 32..63 */
				IRQMAP(UART4_IRQn, 32) | \
				IRQMAP(SPDIF1_IRQn, 32),
				/* interrupts 64..95*/
				IRQMAP(I2C3_IRQn, 32) | \
				IRQMAP(GPT1_IRQn, 32) | \
				IRQMAP(GPT2_IRQn, 32) | \
				IRQMAP(GPT3_IRQn, 32) | \
				IRQMAP(I2S3_IRQn, 32) | \
				IRQMAP(GPT4_IRQn, 32) | \
				IRQMAP(GPT5_IRQn, 32) | \
				IRQMAP(GPT6_IRQn, 32) | \
				IRQMAP(SDMA3_IRQn, 32),
				/* interrupts 96..127*/
				IRQMAP(GPIO1_Combined_0_15_IRQn, 32) | \
				IRQMAP(I2S1_IRQn, 32) | \
				IRQMAP(I2S56_IRQn, 32) | \
				IRQMAP(VPCI_IRQ_BASE, 32) | \
				IRQMAP(VPCI_IRQ_BASE + 1, 32) | \
				IRQMAP(VPCI_IRQ_BASE + 2, 32) | \
				IRQMAP(VPCI_IRQ_BASE + 3, 32),
				/* interrupts 128..159 */
				IRQMAP(PDM_EVENT_IRQn, 32) | \
				IRQMAP(SDMA2_IRQn, 32) | \
				IRQMAP(I2S2_IRQn, 32)  | \
				IRQMAP(PDM_ERROR_IRQn, 32),
			},
		},
	},

	.pci_devices = {
		{ /* IVSHMEM 00:00.0 (demo) */
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 0,
			.bdf = 0 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 0,
			.shmem_dev_id = 2,
			.shmem_peers = 3,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_UNDEFINED,
		},
		{ /* IVSHMEM 00:01.0 (networking) */
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 0,
			.bdf = 1 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 5,
			.shmem_dev_id = 1,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VETH,
		},
	},
};
