###############################################################################
#
# SPDX-License-Identifier: CC0-1.0
#
# Author: James Jones
#
###############################################################################

# Adjust these when making new releases

JDIS_MAJOR = 1
JDIS_MINOR = 0
JDIS_MICRO = 0

CDEFS ?=
CPPFLAGS ?=
CFLAGS ?=
LDLIBS ?=

CDEFS += -DJDIS_MAJOR=$(JDIS_MAJOR) \
	-DJDIS_MINOR=$(JDIS_MINOR) \
	-DJDIS_MICRO=$(JDIS_MICRO)

CPPFLAGS += $(CDEFS)

JRISC_CORE_OBJECTS = jrisc_ctx.o jrisc_inst.o
JRISC_UTIL_OBJECTS = jrisc_ctx_file.o jrisc_ctx_mem.o jrisc_inst_string.o

JDIS_OBJECTS = jdis.o

ALL_OBJECTS = $(JRISC_CORE_OBJECTS) $(JRISC_UTIL_OBJECTS) $(JDIS_OBJECTS)
DEPS = $(patsubst %.o,.%.dep,$(ALL_OBJECTS))

PROGS = jdis

all: $(PROGS)

.%.dep: %.c
	$(CC) -MM $^ -o $@

jdis: $(JDIS_OBJECTS) $(JRISC_CORE_OBJECTS) $(JRISC_UTIL_OBJECTS)

clean:
	rm -f $(ALL_OBJECTS) $(PROGS)

include $(DEPS)
