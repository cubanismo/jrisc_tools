###############################################################################
#
# SPDX-License-Identifier: CC0-1.0
#
# Author: James Jones
#
###############################################################################

.PHONY: all testjdis

all: testjdis testmem.pass

testjdis: test.bin
	awk '/\t/' test.s > test.raw.s
	../jdis test.bin > test.disassembled.s
	diff -u test.raw.s test.disassembled.s
	rm test.raw.s test.disassembled.s

test.bin:	test.s
	rmac -fr $< -o $@

testmem.pass: testmem testmem.gold
	./testmem > testmem.out
	diff testmem.out testmem.gold
	test $$? -eq 0 && rm testmem.out && touch testmem.pass

LOCAL_OBJECTS = testmem.o
DEPS = $(patsubst %.o,.%.dep,$(LOCAL_OBJECTS))

CDEFS ?=
CFLAGS ?=
CPPFLAGS ?=

CDEFS += -I..

CPPFLAGS += $(CDEFS)
CFLAGS += $(CPPFLAGS)

testmem: testmem.o ../libjrisc.a

.PHONY: clean
clean:
	rm -f testmem.pass testmem $(LOCAL_OBJECTS)

.%.dep: %.c
	$(CC) $(CFLAGS) -MM $^ -o $@

include $(DEPS)
