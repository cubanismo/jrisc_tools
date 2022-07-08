###############################################################################
#
# SPDX-License-Identifier: CC0-1.0
#
# Author: James Jones
#
###############################################################################

# Adjust these when making new releases

JDIS_MAJOR = 1
JDIS_MINOR = 3
JDIS_MICRO = 0

# Compiler/linker flags
PIC_FLAGS = -fPIC

CDEFS ?=

CDEFS += -DJDIS_MAJOR=$(JDIS_MAJOR) \
	-DJDIS_MINOR=$(JDIS_MINOR) \
	-DJDIS_MICRO=$(JDIS_MICRO)

CPPFLAGS += $(CDEFS)

CFLAGS += $(PIC_FLAGS)
LDFLAGS += $(PIC_FLAGS)

# Disable deterministic mode to get correct incremental archive builds
ARFLAGS = rvU

# Define the JRISC static library
JRISC_CORE_OBJECTS = jrisc_ctx.o jrisc_inst.o
JRISC_UTIL_OBJECTS = jrisc_ctx_file.o jrisc_ctx_mem.o jrisc_inst_string.o
JRISC_LIB_OBJECTS = $(JRISC_CORE_OBJECTS) $(JRISC_UTIL_OBJECTS)
JRISC_LIB = libjrisc.a
JRISC_LIB_MEMBERS = $(patsubst %.o,$(JRISC_LIB)(%.o),$(JRISC_LIB_OBJECTS))

# Define rules to build the jdis JRISC disassembler program
JDIS_OBJECTS = jdis.o
JDIS = jdis

# Build a comprehensive list of object files
ALL_OBJECTS = $(JRISC_CORE_OBJECTS) $(JRISC_UTIL_OBJECTS) $(JDIS_OBJECTS)

# Build lists of targets by type
LIBS = $(JRISC_LIB)
PROGS = $(JDIS)

# Rules begin here:
.PHONY: all clean
all: $(PROGS) $(LIBS)
$(JRISC_LIB): $(JRISC_LIB_MEMBERS)
$(JDIS): $(JDIS_OBJECTS) $(JRISC_LIB)

clean:
	rm -f $(ALL_OBJECTS) $(PROGS) $(JRISC_LIB)

.%.dep: %.c
	$(CC) -MM $^ -o $@

DEPS = $(patsubst %.o,.%.dep,$(ALL_OBJECTS))
include $(DEPS)
