#
# Jailhouse, a Linux-based partitioning hypervisor
#
# Copyright (c) Siemens AG, 2013
#
# Authors:
#  Jan Kiszka <jan.kiszka@siemens.com>
#
# This work is licensed under the terms of the GNU GPL, version 2.  See
# the COPYING file in the top-level directory.
#

include $(ALWAYS_COMPAT_MK)

-include $(GEN_CONFIG_MK)

LINUXINCLUDE := -I$(src)/../hypervisor/arch/$(SRCARCH)/include \
		-I$(src)/../hypervisor/include \
		-I$(src)/../include
KBUILD_CFLAGS := -Werror -Wall -Wextra -D__LINUX_COMPILER_TYPES_H

ifneq ($(wildcard $(obj)/../include/jailhouse/config.h),)
KBUILD_CFLAGS += -include $(obj)/../include/jailhouse/config.h
endif

OBJCOPYFLAGS := -O binary --remove-section=.note.gnu.property

CONFIGS = $(shell cd $(src); ls $(SRCARCH)/*.c)

always-y := $(CONFIGS:.c=.cell)

targets += $(CONFIGS:.c=.o) $(CONFIGS:.c=.cell)

DTS = $(shell cd $(src); ls $(SRCARCH)/dts/*.dts 2>/dev/null)
always-y += $(DTS:.dts=.dtb)
targets += $(DTS:.dts=.dtb)

# prevent deleting intermediate files which would cause rebuilds
kernel_version = $(shell expr $(VERSION) \* 65536 + $(PATCHLEVEL) \* 256)
kernel_v_6_3 = $(shell expr 6 \* 65536 + 3 \* 256)
ifeq ($(shell expr $(kernel_version) \>= $(kernel_v_6_3)),1)
ifneq ($(and $(filter notintermediate, $(.FEATURES)),$(filter-out 4.4,$(MAKE_VERSION))),)
.NOTINTERMEDIATE: $(addprefix $(obj)/,$(CONFIGS:.c=.o))
else
.SECONDARY: $(addprefix $(obj)/,$(CONFIGS:.c=.o))
endif
else
.SECONDARY: $(addprefix $(obj)/,$(CONFIGS:.c=.o))
endif

$(obj)/%.cell: $(obj)/%.o FORCE
	$(call if_changed,objcopy)
