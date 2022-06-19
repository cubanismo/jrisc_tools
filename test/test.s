; SPDX-License-Identifier: CC0-1.0
;
; Author: James Jones

.org $4000
.text
.gpu
.org $f10000
label:
; Jump to label:
	jr	T, *+0
	nop
	jr	T, *+4
	nop
	jump	T, (r2)
	nop
	add	r1, r2
	addq	#5, r2
	abs	r3
	store	r3, (r14)
	store	r3, (r14+8)
	store	r3, (r15+r8)
	moveq	#31, r31
	moveq	#15, r31
	moveq	#0, r28
	rorq	#32, r0
	rorq	#1, r0
	shrq	#2, r0
	shlq	#2, r0
	sharq	#2, r0
	sharq	#32, r0
	sharq	#1, r0
	cmpq	#8, r1
	cmpq	#-16, r1
	cmpq	#15, r1
	cmpq	#0, r1
	pack	r5
	unpack	r5
.end
