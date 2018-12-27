/*
 * i.MX8QM Target
 *
 * Copyright 2018 NXP
 *
 * Authors:
 *  Peng Fan <peng.fan@nxp.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <jailhouse/types.h>
#include <jailhouse/cell-config.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

struct {
	struct jailhouse_system header;
	__u64 cpus[1];
	struct jailhouse_memory mem_regions[7];
	struct jailhouse_irqchip irqchips[3];
	struct jailhouse_pci_device pci_devices[1];
	struct jailhouse_smmu_sid smmu_sids[3];
} __attribute__((packed)) config = {
	.header = {
		.signature = JAILHOUSE_SYSTEM_SIGNATURE,
		.revision = JAILHOUSE_CONFIG_REVISION,
		.flags = JAILHOUSE_SYS_VIRTUAL_DEBUG_CONSOLE,
		.hypervisor_memory = {
			.phys_start = 0xfdc00000,
			.size =       0x00400000,
		},
		.debug_console = {
			.address = 0x5a060000,
			.size = 0x1000,
			.flags = JAILHOUSE_CON_TYPE_IMX_LPUART |
				 JAILHOUSE_CON_ACCESS_MMIO |
				 JAILHOUSE_CON_REGDIST_4,
			.type = JAILHOUSE_CON_TYPE_IMX_LPUART,
		},
		.platform_info = {
			/*
			 * .pci_mmconfig_base is fixed; if you change it,
			 * update the value in mach.h
			 * (PCI_CFG_BASE) and regenerate the inmate library
			 */
			.pci_mmconfig_base = 0xfd700000,
			.pci_mmconfig_end_bus = 0x0,
			.pci_is_virtual = 1,

			.arm = {
				.gic_version = 3,
				.gicd_base = 0x51a00000,
				.gicr_base = 0x51b00000,
				.maintenance_irq = 25,
				.iommu_units = {
					{
						.base = 0x51400000,
						.size = 0x40000,
						.arm_sid_mask = 0x7f80,
						.arm_smmu_arch = ARM_SMMU_V2,
						.arm_smmu_impl = ARM_MMU500,
					},
				},
			},
		},

		.root_cell = {
			.name = "imx8qm",

			.num_pci_devices = ARRAY_SIZE(config.pci_devices),
			.cpu_set_size = sizeof(config.cpus),
			.num_memory_regions = ARRAY_SIZE(config.mem_regions),
			.num_smmu_sids = ARRAY_SIZE(config.smmu_sids),
			.num_irqchips = ARRAY_SIZE(config.irqchips),
			/*
			 * 118/119 is not used by others, vpci_irq_base not
			 * include base 32
			 */
			.vpci_irq_base = 86,
		},
	},

	.cpus = {
		0x3f,
	},

	.mem_regions = {
		/* MMIO (permissive): TODO: refine the map */ {
			.phys_start = 0x00000000,
			.virt_start = 0x00000000,
			.size =	      0x80000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},

		/* RAM */ {
			.phys_start = 0x80200000,
			.virt_start = 0x80200000,
			.size = 0x5f500000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE,
		},
		/* Inmate memory */ {
			.phys_start = 0xdf700000,
			.virt_start = 0xdf700000,
			.size = 0x1e000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,
		},
		/* IVHSMEM shared memory region for 00:00.0 */ {
			.phys_start = 0xfd900000,
			.virt_start = 0xfd900000,
			.size = 0x200000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE ,
		},
		/* Loader */{
			.phys_start = 0xfdb00000,
			.virt_start = 0xfdb00000,
			.size = 0x100000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE,
		},
		/* OP-TEE reserved memory */{
			.phys_start = 0xfe000000,
			.virt_start = 0xfe000000,
			.size = 0x2000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,
		},
		/* RAM2 */ {
			.phys_start = 0x880000000,
			.virt_start = 0x880000000,
			.size = 0x100000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE,
		},
	},

	.irqchips = {
		/* GIC */ {
			.address = 0x51a00000,
			.pin_base = 32,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
		/* GIC */ {
			.address = 0x51a00000,
			.pin_base = 160,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
		/* GIC */ {
			.address = 0x51a00000,
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
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VETH,
			.domain = 0x0,
		},
	},

	.smmu_sids = {
		{
			.sid = 0x11,
			.sid_mask = 0x7f80,
		},
		{
			.sid = 0x12,
			.sid_mask = 0x7f80,
		},
		{
			.sid = 0x13,
			.sid_mask = 0x7f80,
		},
	},
};

