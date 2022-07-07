/*
 * SPDX-License-Identifier: CC0-1.0
 *
 * Author: James Jones
 */

#include "jrisc_inst_string.h"

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>

static const char *jriscOpNameToStringTable[] = {
#define JRISC_OP(opName, opNum, regSrcType, regDstType, swapRegs, cpus) #opName,
#include "jrisc_optable.h"
#undef JRISC_OP
};

static const char *
jriscOpNameToBaseRegString(enum JRISC_OpName opName) {
	switch (opName) {
	case JRISC_op_storer14n:	/* Fall through */
	case JRISC_op_storer14r:	/* Fall through */
	case JRISC_op_loadr14n:		/* Fall through */
	case JRISC_op_loadr14r:
		return "r14";

	case JRISC_op_storer15n:
	case JRISC_op_storer15r:
	case JRISC_op_loadr15n:
	case JRISC_op_loadr15r:
		return "r15";

	default:
		return NULL;
	}
}

static const char *jriscOpNameToString(enum JRISC_OpName opName)
{
	/* Special case some outliers: */
	switch (opName) {
	case JRISC_op_storer14n:	/* Fall through */
	case JRISC_op_storer14r:	/* Fall through */
	case JRISC_op_storer15n:	/* Fall through */
	case JRISC_op_storer15r:
		return "store";

	case JRISC_op_loadr14n:		/* Fall through */
	case JRISC_op_loadr14r:		/* Fall through */
	case JRISC_op_loadr15n:		/* Fall through */
	case JRISC_op_loadr15r:
		return "load";
	case JRISC_op_movepc:
		return "move";
	default:
		return jriscOpNameToStringTable[opName];
	}
}

/* Helper macro to build strings */
#define ADD_STRING(...)													\
	do {																\
		int tmpLength = snprintf(string, stringLength, __VA_ARGS__);	\
		if (string) string += tmpLength;								\
		if (tmpLength >= stringLength) stringLength = 0;				\
		else stringLength -= tmpLength;									\
		outLength += tmpLength;											\
	} while (0)

static inline uint8_t
jriscRegUnsignedValue(const struct JRISC_OpReg *reg)
{
	return reg->val.uimmediate ? reg->val.uimmediate : 32;
}

/* Returns true if the register field is something visible */
static bool
jriscRegToString(const struct JRISC_OpReg *reg,
				 const char *baseIndirect,
				 char *string,
				 size_t *stringLengthInOut,
				 uint32_t flags,
				 uint32_t address)
{
	size_t stringLength = string ? *stringLengthInOut : 0;
	int outLength = 0;
	bool visible = true;

	switch (reg->type) {
	case JRISC_reg:
		if (baseIndirect) {
			ADD_STRING("(%s+r%d)", baseIndirect, reg->val.reg);
		} else {
			ADD_STRING("r%d", reg->val.reg);
		}
		break;

	case JRISC_indirect:
		assert(!baseIndirect);
		ADD_STRING("(r%d)", reg->val.reg);
		break;

	case JRISC_condition:
		assert(!baseIndirect);
		switch (reg->val.condition) {
		case 0x0:	visible = false;	/* True/Always */	break;
		case 0x1:	ADD_STRING("NE");	/* Not  Equal */	break;
		case 0x2:	ADD_STRING("EQ");	/* Equal */			break;
		case 0x4:	ADD_STRING("CC");	/* Carry Clear */	break;
		case 0x5:	ADD_STRING("HI");	/* Higher */		break;
		case 0x8:	ADD_STRING("CS");	/* Carry Set */		break;
		case 0x14:	ADD_STRING("PL");	/* Plus/Positive */	break;
		case 0x18:	ADD_STRING("MI");	/* Minus/Negative */break;
		default:
			ADD_STRING("$%x", reg->val.condition);
			break;
		}
		break;

	case JRISC_uimmediate:
		if (baseIndirect) {
			ADD_STRING("(%s+%" PRIu8 ")", baseIndirect,
					   jriscRegUnsignedValue(reg));
		} else {
			ADD_STRING("#%" PRIu8, jriscRegUnsignedValue(reg));
		}
		break;

	case JRISC_zuimmediate:
		assert(!baseIndirect);
		ADD_STRING("#%" PRIu8, reg->val.uimmediate);
		break;

	case JRISC_shlimmediate:
		assert(!baseIndirect);
		ADD_STRING("#%" PRIu8, 32 - jriscRegUnsignedValue(reg));
		break;

	case JRISC_simmediate:
		assert(!baseIndirect);
		ADD_STRING("#%" PRId8, reg->val.simmediate);
		break;

	case JRISC_pcoffset:
		assert(!baseIndirect);
		if (flags & JRISC_STRINGFLAG_ADDRESS) {
			ADD_STRING("$%x",(reg->val.simmediate + 1) * 2 + address);
		} else {
			ADD_STRING("*%+" PRId8, (reg->val.simmediate + 1) * 2);
		}
		break;

	case JRISC_flag:
		// fall through
	case JRISC_unused:
		// Nothing to do.
		assert(!baseIndirect);
		visible = false;
		break;
	}

	*stringLengthInOut = outLength + 1 /* For '/0' */;

	return visible;
}

void
jriscInstructionToString(const struct JRISC_Instruction *instruction,
						 uint32_t flags,
						 char *string,
						 size_t *stringLengthInOut)
{
	const char *reg1BaseIndirect =
		jriscOpNameToBaseRegString(instruction->opName);
	const char *reg2BaseIndirect = NULL;
	const struct JRISC_OpReg *reg1 = &instruction->regSrc;
	const struct JRISC_OpReg *reg2 = &instruction->regDst;
	const char *opIndent = flags ? "" : "\t";
	const char *opNameFmt = "%s%-8s";
	size_t stringLength = string ? *stringLengthInOut : 0;
	size_t localLength;
	int outLength = 0;
	bool regVisible = true;

	if (instruction->swapRegs) {
		reg1 = &instruction->regDst;
		reg2 = &instruction->regSrc;
		reg2BaseIndirect = reg1BaseIndirect;
		reg1BaseIndirect = NULL;
	}

	if (flags & JRISC_STRINGFLAG_ADDRESS) {
		ADD_STRING("%08x:", instruction->address);
		opIndent = " ";
	}

	if (flags & JRISC_STRINGFLAG_MACHINE_CODE) {
		uint16_t machineCode =
			((uint16_t)instruction->opCode << JRISC_OPCODE_SHIFT) |
			((uint16_t)jriscRegToRaw(&instruction->regSrc) <<
			 JRISC_REGSRC_SHIFT) |
			(uint16_t)jriscRegToRaw(&instruction->regDst);

		ADD_STRING("%s%02x%02x", opIndent,
				   machineCode >> 8, (machineCode & 0xff));
		opIndent = "  ";

		if (instruction->opName == JRISC_op_movei) {
			uint16_t word;

			word = jriscInstructionLongImmediateLow(instruction);
			ADD_STRING(" %02x%02x", word >> 8, (word & 0xff));

			word = jriscInstructionLongImmediateHigh(instruction);
			ADD_STRING(" %02x%02x", word >> 8, (word & 0xff));
		} else {
			ADD_STRING("          ");
		}
	}

	if ((JRISC_unused == reg1->type) &&
		(JRISC_unused == reg2->type)) {
		/* If neither register is used, don't insert trailing whitespace */
		opNameFmt = "%s%s";
	}

	ADD_STRING(opNameFmt, opIndent, jriscOpNameToString(instruction->opName));

	if (instruction->opName == JRISC_op_movei) {
		ADD_STRING("#$%x", instruction->longImmediate);
    } else if (instruction->opName == JRISC_op_movepc) {
		ADD_STRING("PC,", instruction->longImmediate);
		regVisible = 0;
		reg2 = reg1;
	} else {
		localLength = stringLength;
		regVisible = jriscRegToString(reg1,
									  reg1BaseIndirect,
									  string,
									  &localLength,
									  flags,
									  instruction->address);
		localLength -= 1 /* For '\0' */;
		string += localLength;
		outLength += localLength;
		if (localLength > stringLength) stringLength = 0;
		else stringLength -= localLength;
	}

	if (regVisible) ADD_STRING(", ");

	localLength = stringLength;
	jriscRegToString(reg2,
					 reg2BaseIndirect,
					 string,
					 &localLength,
					 flags,
					 instruction->address);
	localLength -= 1 /* For '\0' */;
	string += localLength;
	outLength += localLength;
	if (localLength > stringLength) stringLength = 0;
	else stringLength -= localLength;

	*stringLengthInOut = outLength + 1 /* For '\0' */;
}

enum JRISC_Error
jriscInstructionPrint(const struct JRISC_Instruction *instruction,
					  uint32_t flags)
{
	static char tempBuf[32];
	char *outBuf = NULL;
	size_t length = sizeof(tempBuf);

	jriscInstructionToString(instruction, flags, tempBuf, &length);

	if (length > sizeof(tempBuf)) {
		/*
		 * Instruction did not fit in default buffer size. Allocate a larger
		 * buffer for it.
		 */
		outBuf = calloc(1, length);
		if (!outBuf) {
			return JRISC_ERROR_outOfMemory;
		}

		jriscInstructionToString(instruction, flags, outBuf, &length);
	} else {
		outBuf = &tempBuf[0];
	}

	printf("%s\n", outBuf);

	if (outBuf != &tempBuf[0]) {
		free(outBuf);
	}

	return JRISC_success;
}
