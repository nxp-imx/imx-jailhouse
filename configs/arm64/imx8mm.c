/*
 * i.MX8MM Target
 *
 * Copyright 2018 NXP
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
	struct jailhouse_memory mem_regions[6];
	struct jailhouse_irqchip irqchips[3];
	struct jailhouse_pci_device pci_devices[1];
} __attribute__((packed)) config = {
	.header = {
		.signature = JAILHOUSE_SYSTEM_SIGNATURE,
		.revision = JAILHOUSE_CONFIG_REVISION,
		.hypervisor_memory = {
			.phys_start = 0xbfc00000,
			.size =       0x00400000,
		},
		.debug_console = {
			.address = 0x30890000,
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
			.pci_mmconfig_base = 0xbb800000,
			.pci_mmconfig_end_bus = 0,
			.pci_is_virtual = 1,

			.arm = {
				.gic_version = 3,
				.gicd_base = 0x38800000,
				.gicr_base = 0x38880000,
				.maintenance_irq = 25,
			},
		},
		.root_cell = {
			.name = "imx8mm",

			.num_pci_devices = ARRAY_SIZE(config.pci_devices),
			.cpu_set_size = sizeof(config.cpus),
			.num_memory_regions = ARRAY_SIZE(config.mem_regions),
			.num_irqchips = ARRAY_SIZE(config.irqchips),
			.vpci_irq_base = 74, /* Not include 32 base */
		},
	},

	.cpus = {
		0xf,
	},

	.mem_regions = {
		/* IO */ {
			.phys_start = 0x00000000,
			.virt_start = 0x00000000,
			.size =	      0x40000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* RAM */ {
			.phys_start = 0x40000000,
			.virt_start = 0x40000000,
			.size = 0x7b700000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE,
		},
		/* Loader */{
			.phys_start = 0xbb700000,
			.virt_start = 0xbb700000,
			.size = 0x100000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE,
		},
		/* IVHSMEM shared memory region for 00:00.0 */ {
			.phys_start = 0xbba00000,
			.virt_start = 0xbba00000,
			.size = 0x100000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE ,
		},
		/* IVHSMEM shared memory region for 00:00.0 */ {
			.phys_start = 0xbbb00000,
			.virt_start = 0xbbb00000,
			.size = 0x100000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE ,
		},
		/* Inmate memory */{
			.phys_start = 0xbbc00000,
			.virt_start = 0xbbc00000,
			.size = 0x4000000,
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
			.shmem_region = 3,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_UNDEFINED,
			.domain = 0x0,
		},
	},
};
