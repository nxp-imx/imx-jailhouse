/*
 * i.MX95 Target
 *
 * Copyright 2023 NXP
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <jailhouse/types.h>
#include <jailhouse/cell-config.h>

struct {
	struct jailhouse_system header;
	__u64 cpus[1];
	struct jailhouse_memory mem_regions[25];
	struct jailhouse_irqchip irqchips[3];
	struct jailhouse_pci_device pci_devices[4];
} __attribute__((packed)) config = {
	.header = {
		.signature = JAILHOUSE_SYSTEM_SIGNATURE,
		.revision = JAILHOUSE_CONFIG_REVISION,
		.architecture = JAILHOUSE_ARM64,
		.flags = JAILHOUSE_SYS_VIRTUAL_DEBUG_CONSOLE,
		.hypervisor_memory = {
			.phys_start = 0xffc00000,
			.size =       0x00400000,
		},
		.debug_console = {
			.address = 0x44380000,
			.size = 0x1000,
			.flags = JAILHOUSE_CON_ACCESS_MMIO |
				 JAILHOUSE_CON_REGDIST_4,
			.type = JAILHOUSE_CON_TYPE_IMX_LPUART,
		},
		.platform_info = {
			.pci_mmconfig_base = 0xff700000,
			.pci_mmconfig_end_bus = 0,
			.pci_is_virtual = 1,
			.pci_domain = 4,

			.arm = {
				.gic_version = 3,
				.gicd_base = 0x48000000,
				.gicr_base = 0x48060000,
				.maintenance_irq = 25,
			},
		},
		.root_cell = {
			.name = "imx95",
			.cpu_set_size = sizeof(config.cpus),
			.num_memory_regions = ARRAY_SIZE(config.mem_regions),
			.num_irqchips = ARRAY_SIZE(config.irqchips),
			.num_pci_devices = ARRAY_SIZE(config.pci_devices),
			.vpci_irq_base = 376,
		},
	},

	.cpus = {
		0x3f,
	},

	.mem_regions = {
		/* IVSHMEM shared memory region (virtio-blk back-end) */
		{
			.phys_start = 0xff900000,
			.virt_start = 0xff900000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ,
		},
		{
			.phys_start = 0xff901000,
			.virt_start = 0xff901000,
			.size = 0xdf000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,
		},
		{ 0 },
		{ 0 },
		/* IVSHMEM shared memory region (virtio-con back-end) */
		{
			.phys_start = 0xff9e0000,
			.virt_start = 0xff9e0000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ,
		},
		{
			.phys_start = 0xff9e1000,
			.virt_start = 0xff9e1000,
			.size = 0xf000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,
		},
		{ 0 },
		{ 0 },
		/* IVHSMEM shared memory region for 00:00.0 (demo)*/
		{
			.phys_start = 0xff9f0000,
			.virt_start = 0xff9f0000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ,
		},
		{
			.phys_start = 0xff9f1000,
			.virt_start = 0xff9f1000,
			.size = 0x9000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,
		},
		{
			.phys_start = 0xff9fa000,
			.virt_start = 0xff9fa000,
			.size = 0x2000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,
		},
		{
			.phys_start = 0xff9fc000,
			.virt_start = 0xff9fc000,
			.size = 0x2000,
			.flags = JAILHOUSE_MEM_READ,
		},
		{
			.phys_start = 0xff9fe000,
			.virt_start = 0xff9fe000,
			.size = 0x2000,
			.flags = JAILHOUSE_MEM_READ,
		},
		/* IVSHMEM shared memory regions for 00:01.0 (networking) */
		JAILHOUSE_SHMEM_NET_REGIONS(0xffa00000, 0),
		/* io */ {
			.phys_start = 0x00000000,
			.virt_start = 0x00000000,
			.size =	      0x80000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* M7 secure memory or non-secure depends on SM M7 cfg */ {
			.phys_start = 0x80000000,
			.virt_start = 0x80000000,
			.size = 0x8000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,
		},
		/* M7-AP rpmsg */ {
			.phys_start = 0x88000000,
			.virt_start = 0x88000000,
			.size = 0x2000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,
		},
		/* TEE Shared */ {
			.phys_start = 0x8e000000,
			.virt_start = 0x8e000000,
			.size = 0x200000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,
		},
		/* ROOT */ {
			.phys_start = 0x8e200000,
			.virt_start = 0x8e200000,
			.size = 0x61e00000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				 JAILHOUSE_MEM_EXECUTE,
		},
		/* inmate memory */{
			.phys_start = 0xf0000000,
			.virt_start = 0xf0000000,
			.size = 0xf700000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				 JAILHOUSE_MEM_EXECUTE,
		},
		/* loader */{
			.phys_start = 0xffb00000,
			.virt_start = 0xffb00000,
			.size = 0x100000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				 JAILHOUSE_MEM_EXECUTE,
		},
		/* ROOT */ {
			.phys_start = 0x100000000,
			.virt_start = 0x100000000,
			.size = 0x380000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				 JAILHOUSE_MEM_EXECUTE,
		},
	},

	.irqchips = {
		/* GIC */ {
			.address = 0x48000000,
			.pin_base = 32,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
		/* GIC */ {
			.address = 0x48000000,
			.pin_base = 160,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
		/* GIC */ {
			.address = 0x48000000,
			.pin_base = 288,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
	},

	.pci_devices = {
		{ /* IVSHMEM 0004:00:00.0 (demo) */
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 4,
			.bdf = 0 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 8,
			.shmem_dev_id = 0,
			.shmem_peers = 3,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_UNDEFINED,
		},
		{ /* IVSHMEM 0004:00:01.0 (networking) */
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 4,
			.bdf = 1 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 13,
			.shmem_dev_id = 0,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VETH,
		},
		{
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 4,
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
			.domain = 4,
			.bdf = 3 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 4,
			.shmem_dev_id = 0,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VIRTIO_BACK +
					  VIRTIO_DEV_CONSOLE,
		},
	},
};
