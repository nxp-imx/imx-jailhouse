/*
 * i.MX8MM Target
 *
 * Copyright 2018,2021 NXP
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

struct {
	struct jailhouse_system header;
	__u64 cpus[1];
	struct jailhouse_memory mem_regions[24];
	struct jailhouse_irqchip irqchips[3];
	struct jailhouse_pci_device pci_devices[4];
} __attribute__((packed)) config = {
	.header = {
		.signature = JAILHOUSE_SYSTEM_SIGNATURE,
		.revision = JAILHOUSE_CONFIG_REVISION,
		.flags = JAILHOUSE_SYS_VIRTUAL_DEBUG_CONSOLE,
		.hypervisor_memory = {
			.phys_start = 0xb7c00000,
			.size =       0x00400000,
		},
		.debug_console = {
			.address = 0x30890000,
			.size = 0x1000,
			.flags = JAILHOUSE_CON_TYPE_IMX |
				 JAILHOUSE_CON_ACCESS_MMIO |
				 JAILHOUSE_CON_REGDIST_4,
			.type = JAILHOUSE_CON_TYPE_IMX,
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
			.pci_domain = 1,

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
		/* IVSHMEM shared memory region (virtio-blk back-end) */
		{
			.phys_start = 0xbba00000,
			.virt_start = 0xbba00000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ ,
		},
		{
			.phys_start = 0xbba01000,
			.virt_start = 0xbba01000,
			.size = 0xdf000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,
		},
		{ 0 },
		{ 0 },
		/* IVSHMEM shared memory region (virtio-con back-end) */
		{
			.phys_start = 0xbbae0000,
			.virt_start = 0xbbae0000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ,
		},
		{
			.phys_start = 0xbbae1000,
			.virt_start = 0xbbae1000,
			.size = 0xf000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,
		},
		{ 0 },
		{ 0 },
		/* IVHSMEM shared memory region for 00:00.0 (demo )*/ {
			.phys_start = 0xbbaf0000,
			.virt_start = 0xbbaf0000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ,
		},
		{
			.phys_start = 0xbbaf1000,
			.virt_start = 0xbbaf1000,
			.size = 0x1ff000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE ,
		},
		{
			.phys_start = 0xbbcf0000,
			.virt_start = 0xbbcf0000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE ,
		},
		{
			.phys_start = 0xbbd00000,
			.virt_start = 0xbbd00000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ,
		},
		/* IVSHMEM shared memory regions for 00:01.0 (networking) */
		JAILHOUSE_SHMEM_NET_REGIONS(0xbbd01000, 0),
		/* IO */ {
			.phys_start = 0x00000000,
			.virt_start = 0x00000000,
			.size =	      0x38800000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* IO */ {
			.phys_start = 0x38900000,
			.virt_start = 0x38900000,
			.size =	      0x7700000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* RAM 00*/ {
			.phys_start = 0x40000000,
			.virt_start = 0x40000000,
			.size = 0x73c00000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE,
		},
		/* Inmate memory */{
			.phys_start = 0xb3c00000,
			.virt_start = 0xb3c00000,
			.size = 0x04000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE,
		},
		/* RAM 01 */ {
			.phys_start = 0xb8000000,
			.virt_start = 0xb8000000,
			.size = 0x03700000,
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
		/* RAM 02 */ {
			.phys_start = 0xbbc00000,
			.virt_start = 0xbbc00000,
			.size = 0x02400000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE,
		},
		/* OP-TEE reserved memory */{
			.phys_start = 0xbe000000,
			.virt_start = 0xbe000000,
			.size = 0x2000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,
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
			.domain = 1,
			.bdf = 2 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 0,
			.shmem_dev_id = 0,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VIRTIO_BACK +
				VIRTIO_DEV_BLOCK,
		},
		{
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 1,
			.bdf = 3 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 4,
			.shmem_dev_id = 0,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VIRTIO_BACK +
				VIRTIO_DEV_CONSOLE,
		},
		{ /* IVSHMEM 0000:00:00.0 (demo) */
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 1,
			.bdf = 0 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 8,
			.shmem_dev_id = 0,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_UNDEFINED,
		},
		{ /* IVSHMEM 0000:00:01.0 (networking) */
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 1,
			.bdf = 1 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 12,
			.shmem_dev_id = 0,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VETH,
		},
	},
};
