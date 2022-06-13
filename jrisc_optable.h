/*
 * SPDX-License-Identifier: CC0-1.0
 *
 * Author: James Jones
 */

/*
 * Jaguar RISC opcode table.
 *
 * To use this file, define a macro named as such:
 *
 *    JRISC_OP(opName, opNum, regSrcType, regDstType, swapRegs, cpus)
 *
 * Then include this file. The macro will be evaluated once for each Jaguar
 * RISC opcode.
 *
 * Notes:
 *
 * -The source and destination registers are swapped on store instructions,
 *  I.e., they're arranged the same as those of an equivalent load instruction.
 *
 * -The 32-bit immediate value in a movei is stored in the next two instruction
 *  words as <low 16-bits> <high 16-bits>
 */

#include <stdbool.h>
#include "jrisc_regtype.h"

/*		 opName	opNum	regSrcType			regDstType		swapRegs cpus */
JRISC_OP(add,		0,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(addc,		1,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(addq,		2,	JRISC_uimmediate,	JRISC_reg,		false,	JRISC_both)
JRISC_OP(addqt,		3,	JRISC_uimmediate,	JRISC_reg,		false,	JRISC_both)
JRISC_OP(sub,		4,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(subc,		5,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(subq,		6,	JRISC_uimmediate,	JRISC_reg,		false,	JRISC_both)
JRISC_OP(subqt,		7,	JRISC_uimmediate,	JRISC_reg,		false,	JRISC_both)
JRISC_OP(neg,		8,	JRISC_unused,		JRISC_reg,		false,	JRISC_both)
JRISC_OP(and,		9,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(or,		10,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(xor,		11,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(not,		12,	JRISC_unused,		JRISC_reg,		false,	JRISC_both)
JRISC_OP(btst,		13,	JRISC_zuimmediate,	JRISC_reg,		false,	JRISC_both)
JRISC_OP(bset,		14,	JRISC_zuimmediate,	JRISC_reg,		false,	JRISC_both)
JRISC_OP(bclr,		15,	JRISC_zuimmediate,	JRISC_reg,		false,	JRISC_both)
JRISC_OP(mult,		16,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(imult,		17,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(imultn,	18,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(resmac,	19,	JRISC_unused,		JRISC_reg,		false,	JRISC_both)
JRISC_OP(imacn,		20,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(div,		21,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(abs,		22,	JRISC_unused,		JRISC_reg,		false,	JRISC_both)
JRISC_OP(sh,		23,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(shlq,		24,	JRISC_shlimmediate,	JRISC_reg,		false,	JRISC_both)
JRISC_OP(shrq,		25,	JRISC_uimmediate,	JRISC_reg,		false,	JRISC_both)
JRISC_OP(sha,		26,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(sharq,		27,	JRISC_uimmediate,	JRISC_reg,		false,	JRISC_both)
JRISC_OP(ror,		28,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(rorq,		29,	JRISC_uimmediate,	JRISC_reg,		false,	JRISC_both)
JRISC_OP(cmp,		30,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(cmpq,		31,	JRISC_simmediate,	JRISC_reg,		false,	JRISC_both)
JRISC_OP(sat8,		32,	JRISC_unused,		JRISC_reg,		false,	JRISC_gpu)
JRISC_OP(subqmod,	32,	JRISC_uimmediate,	JRISC_reg,		false,	JRISC_dsp)
JRISC_OP(sat16,		33,	JRISC_unused,		JRISC_reg,		false,	JRISC_gpu)
JRISC_OP(sat16s,	33,	JRISC_unused,		JRISC_reg,		false,	JRISC_dsp)
JRISC_OP(move,		34,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(moveq,		35,	JRISC_zuimmediate,	JRISC_reg,		false,	JRISC_both)
JRISC_OP(moveta,	36,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(movefa,	37,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(movei,		38,	JRISC_unused,		JRISC_reg,		false,	JRISC_both)
JRISC_OP(loadb,		39,	JRISC_indirect,		JRISC_reg,		false,	JRISC_both)
JRISC_OP(loadw,		40,	JRISC_indirect,		JRISC_reg,		false,	JRISC_both)
JRISC_OP(load,		41,	JRISC_indirect,		JRISC_reg,		false,	JRISC_both)
JRISC_OP(loadp,		42,	JRISC_indirect,		JRISC_reg,		false,	JRISC_gpu)
JRISC_OP(sat32s,	42,	JRISC_unused,		JRISC_reg,		false,	JRISC_dsp)
JRISC_OP(loadr14n,	43,	JRISC_uimmediate,	JRISC_reg,		false,	JRISC_both)
JRISC_OP(loadr15n,	44,	JRISC_uimmediate,	JRISC_reg,		false,	JRISC_both)
JRISC_OP(storeb,	45,	JRISC_indirect,		JRISC_reg,		true,	JRISC_both)
JRISC_OP(storew,	46,	JRISC_indirect,		JRISC_reg,		true,	JRISC_both)
JRISC_OP(store,		47,	JRISC_indirect,		JRISC_reg,		true,	JRISC_both)
JRISC_OP(storep,	48,	JRISC_indirect,		JRISC_reg,		true,	JRISC_gpu)
JRISC_OP(mirror,	48,	JRISC_reg,			JRISC_reg,		true,	JRISC_dsp)
JRISC_OP(storer14n,	49,	JRISC_uimmediate,	JRISC_reg,		true,	JRISC_both)
JRISC_OP(storer15n,	50,	JRISC_uimmediate,	JRISC_reg,		true,	JRISC_both)
JRISC_OP(movepc,	51,	JRISC_unused,		JRISC_reg,		true,	JRISC_reg)
JRISC_OP(jump,		52,	JRISC_indirect,		JRISC_condition,true,	JRISC_both)
JRISC_OP(jr,		53,	JRISC_pcoffset,		JRISC_condition,true,	JRISC_both)
JRISC_OP(mmult,		54,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(mtoi,		55,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(normi,		56,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(nop,		57,	JRISC_unused,		JRISC_unused,	false,	JRISC_both)
JRISC_OP(loadr14r,	58,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(loadr15r,	59,	JRISC_reg,			JRISC_reg,		false,	JRISC_both)
JRISC_OP(storer14r,	60,	JRISC_reg,			JRISC_reg,		true,	JRISC_both)
JRISC_OP(storer15r,	61,	JRISC_reg,			JRISC_reg,		true,	JRISC_both)
JRISC_OP(sat24,		62,	JRISC_unused,		JRISC_reg,		false,	JRISC_gpu)
JRISC_OP(addqmod,	63,	JRISC_uimmediate,	JRISC_reg,		false,	JRISC_dsp)
JRISC_OP(pack,		63,	JRISC_flag,			JRISC_reg,		false,	JRISC_gpu)
JRISC_OP(unpack,	63,	JRISC_flag,			JRISC_reg,		false,	JRISC_gpu)
