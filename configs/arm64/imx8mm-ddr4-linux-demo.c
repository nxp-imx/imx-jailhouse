/*
 * iMX8MM target - linux-demo
 *
 * Copyright 2019 NXP
 *
 * Authors:
 *  Alice Guo <alice.guo@nxp.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

/*
 * Boot 2nd Linux cmdline:
 * export PATH=$PATH:/usr/share/jailhouse/tools/
 *
 * jailhouse cell linux imx8mm-ddr4-linux-demo.cell /run/media/mmcblk1p1/Image -d \
 * /run/media/mmcblk1p1/imx8mm-ddr4-evk-revb-inmate.dtb -c \
 * "console=ttymxc3,115200 earlycon=ec_imx6q,0x30890000,115200 ubi.mtd=5 \
 * root=ubi0:nandrootfs rootfstype=ubifs \
 * mtdparts=gpmi-nand:64m(nandboot),16m(nandfit),32m(nandkernel),16m(nanddtb),8m(nandtee),-(nandrootfs)"
 */
#include <jailhouse/types.h>
#include <jailhouse/cell-config.h>

struct {
	struct jailhouse_cell_desc cell;
	__u64 cpus[1];
	struct jailhouse_memory mem_regions[8];
	struct jailhouse_irqchip irqchips[2];
	struct jailhouse_pci_device pci_devices[1];
} __attribute__((packed)) config = {
	.cell = {
		.signature = JAILHOUSE_CELL_DESC_SIGNATURE,
		.revision = JAILHOUSE_CONFIG_REVISION,
		.name = "linux-inmate-demo",
		.flags = JAILHOUSE_CELL_PASSIVE_COMMREG,

		.cpu_set_size = sizeof(config.cpus),
		.num_memory_regions = ARRAY_SIZE(config.mem_regions),
		.num_irqchips = ARRAY_SIZE(config.irqchips),
		.num_pci_devices = ARRAY_SIZE(config.pci_devices),
		.vpci_irq_base = 76, /* Not include 32 base */
		.pio_bitmap_size = 0,
	},

	.cpus = {
		0xc,
	},

	.mem_regions = {
		/* UART2 earlycon */ {
			.phys_start = 0x30890000,
			.virt_start = 0x30890000,
			.size = 0x1000,
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
		/* APBH DMA */ {
		        .phys_start = 0x33000000,
			.virt_start = 0x33000000,
			.size = 0x2000,
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
		/* IVSHMEM */ {
			.phys_start = 0xbba00000,
			.virt_start = 0xbba00000,
			.size = 0x00200000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_ROOTSHARED,
		},
		/* RAM: Top at 4GB Space */ {
			.phys_start = 0xbb700000,
			.virt_start = 0,
			.size = 0x10000, /* 64KB */
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_LOADABLE,
		},
		/* RAM */ {
			/*
			 * We could not use 0x80000000 which conflicts with
			 * COMM_REGION_BASE
			 */
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
	},

	.irqchips = {
		/* uart2/sdhc1/gpmi/apbh-dma */ {
			.address = 0x38800000,
			.pin_base = 32,
			.pin_bitmap = {
				(1 << (24 + 32 - 32)) | (1 << (29 + 32 - 32))
                                | (1 << (14 + 32 - 32)) | (1 << (12 + 32 - 32))
			},
		},
		/* IVSHMEM */ {
			.address = 0x38800000,
			.pin_base = 96,
			.pin_bitmap = {
				0x1 << (76 + 32 - 96) /* SPI 76 */
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
			.shmem_region = 4,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VETH,
			.domain = 0x0,
		},
	},
};
