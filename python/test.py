#!/usr/bin/env python3
###############################################################################
#
# SPDX-License-Identifier: CC0-1.0
#
# Author: James Jones
#
###############################################################################

import jrisc

#
# Disassemble the regset commands. Should print:
#
#	movei	#$5bc, r31
#	store	r0, (r31)
#	addq	#4, r31
#	load	(r31), r0
#	movei	#$f02114, r31
#	moveq	#0, r30
#	store	r30, (r31)
#	jr	T, *-2
#	nop
#	nop
#
print(*jrisc.disassemble(b'\x98\x1f\x05\xbc\x00\x00\xbf\xe0\x08\x9f\xa7\xe0\x98\x1f\x21\x14\x00\xf0\x8c\x1e\xbf\xfe\xd7\xc0\xe4\x00\xe4\x00', baseAddress=0x0, dsp=False, machineCode=True), sep = "\n")
