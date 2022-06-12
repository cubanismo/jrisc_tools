.PHONY: all
all: test.bin
	awk '/\t/' test.s > test.raw.s
	../jdis test.bin > test.disassembled.s
	diff -u test.raw.s test.disassembled.s
	rm test.raw.s test.disassembled.s

test.bin:	test.s
	rmac -fr $< -o $@
	
