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
				 size_t *stringLengthInOut)
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
		case 0x0:	ADD_STRING("T");	/* True/Always */	break;
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
		ADD_STRING("*%+" PRId8, (reg->val.simmediate + 1) * 2);
		break;

	case JRISC_flag:
		// fall through
	case JRISC_unused:
		// Nothing to do.
		assert(!baseIndirect);
		visible = false;
		break;
	}

	*stringLengthInOut = outLength;

	return visible;
}

void
jriscInstructionToString(const struct JRISC_Instruction *instruction,
						 char *string,
						 size_t *stringLengthInOut)
{
	const char *reg1BaseIndirect =
		jriscOpNameToBaseRegString(instruction->opName);
	const char *reg2BaseIndirect = NULL;
	const struct JRISC_OpReg *reg1 = &instruction->regSrc;
	const struct JRISC_OpReg *reg2 = &instruction->regDst;
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

	ADD_STRING("\t%s\t", jriscOpNameToString(instruction->opName));

	if (instruction->opName == JRISC_op_movei) {
		ADD_STRING("#$%x", instruction->longImmediate);
	} else {
		localLength = stringLength;
		regVisible = jriscRegToString(reg1,
									  reg1BaseIndirect,
									  string,
									  &localLength);
		string += localLength;
		if (localLength > stringLength) stringLength = 0;
		else stringLength -= localLength;
	}


	if (regVisible) ADD_STRING(", ");

	localLength = stringLength;
	jriscRegToString(reg2, reg2BaseIndirect, string, &localLength);
	string += localLength;
	if (localLength > stringLength) stringLength = 0;
	else stringLength -= localLength;
}

enum JRISC_Error
jriscInstructionPrint(const struct JRISC_Instruction *instruction)
{
	static char tempBuf[32];
	char *outBuf = NULL;
	size_t length = sizeof(tempBuf);

	jriscInstructionToString(instruction, tempBuf, &length);

	if (length > sizeof(tempBuf)) {
		/*
		 * Instruction did not fit in default buffer size. Allocate a larger
		 * buffer for it.
		 */
		outBuf = calloc(1, length);
		if (!outBuf) {
			return JRISC_ERROR_outOfMemory;
		}

		jriscInstructionToString(instruction, outBuf, &length);
	} else {
		outBuf = &tempBuf[0];
	}

	printf("%s\n", outBuf);

	if (outBuf != &tempBuf[0]) {
		free(outBuf);
	}

	return JRISC_success;
}
