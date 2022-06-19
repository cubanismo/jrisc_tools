/*
 * SPDX-License-Identifier: CC0-1.0
 *
 * Author: James Jones
 */

#include "jrisc_base.h"
#include "jrisc_ctx.h"
#include "jrisc_inst.h"

#include <stddef.h>
#include <assert.h>

const struct JRISC_Instruction
jriscInstructionTable[] = {
#define JRISC_OP(opName, opNum, regSrcType, regDstType, swapRegs, cpus)	\
	{																	\
		/* .opName = */			JRISC_op_##opName,						\
		/* .opCode = */			opNum,									\
		/* .regSrc.type = */	{ regSrcType },							\
		/* .regDst.type = */	{ regDstType },							\
		/* .swapRegs = */		swapRegs,								\
		/* .cpu = */			cpus,									\
		/* .longImmediate = */	0										\
	},
#include "jrisc_optable.h"
#undef JRISC_OP
};

static const struct JRISC_Instruction *
jriscInstructionsFromOpCode(uint8_t opCode)
{
	unsigned i;

	/*
	 * Table is sorted on raw op code values, and is a superset of those values.
	 * Any operations associated with this opCode will appear no earlier than
	 * <opCode> entries into the table.
	 */
	for (i = opCode; i < JRISC_invalidOpName; i++) {
		if (jriscInstructionTable[i].opCode == opCode) {
			return &jriscInstructionTable[i];
		}
	}

	return NULL;
}

static bool
jriscInstructionMatchesRaw(const struct JRISC_Instruction *instruction,
						   uint8_t opCode,
						   uint8_t rawSrc,
						   enum JRISC_CPU cpu)
{
	/* The instruction's raw opcode number must match that specified */
	if (instruction->opCode != opCode) return false;

	/* The instruction must be compatible with the specified CPU */
	if ((instruction->cpu != JRISC_both) &&
		(instruction->cpu != cpu)) return false;

	/* The pack & unpack instructions are differentiated by their rawSrc val */
	if ((instruction->opName == JRISC_op_pack) && (rawSrc != 0)) return false;
	if ((instruction->opName == JRISC_op_unpack) && (rawSrc != 1)) return false;

	return true;
}

static enum JRISC_Error
jriscValidateReg(uint8_t raw, enum JRISC_Reg *regOut)
{
	if (raw & ~JRISC_REG_MASK) {
		return JRISC_ERROR_invalidReg;
	}

	*regOut = (enum JRISC_Reg)raw;

	return JRISC_success;
}

static enum JRISC_Error
jriscValidateCondition(uint8_t raw, uint8_t *conditionOut)
{
	if (raw & ~JRISC_REG_MASK) {
		return JRISC_ERROR_invalidValue;
	}

	*conditionOut = raw;

	return JRISC_success;
}

static enum JRISC_Error
jriscValidateImmediate(uint8_t raw, uint8_t *immediateOut)
{
	if (raw & ~JRISC_REG_MASK) {
		return JRISC_ERROR_invalidValue;
	}

	*immediateOut = raw;

	return JRISC_success;
}

static enum JRISC_Error
jriscValidateFlag(uint8_t raw, bool *flagOut)
{
	if (raw & ~0x1) {
		return JRISC_ERROR_invalidValue;
	}

	*flagOut = (bool)raw;

	return JRISC_success;
}

static enum JRISC_Error
jriscRegFromRaw(uint8_t raw,
				enum JRISC_RegType type,
				struct JRISC_OpReg *regOut)
{
	struct JRISC_OpReg out = { type };
	enum JRISC_Error ret = JRISC_success;

	switch (type) {
	case JRISC_indirect: /* Fall through */
	case JRISC_reg:
		ret = jriscValidateReg(raw, &out.val.reg);
		if (ret != JRISC_success) return ret;
		break;

	case JRISC_condition:
		ret = jriscValidateCondition(raw, &out.val.condition);
		if (ret != JRISC_success) return ret;
		break;

	case JRISC_pcoffset:		/* Fall through */
	case JRISC_simmediate:		/* Fall through */
	case JRISC_shlimmediate:	/* Fall through */
	case JRISC_uimmediate:		/* Fall through */
	case JRISC_zuimmediate:		/* Fall through*/
		ret = jriscValidateImmediate(raw, &out.val.uimmediate);
		if (ret != JRISC_success) return ret;

		if (((type == JRISC_pcoffset) || (type == JRISC_simmediate)) &&
			(out.val.uimmediate & 0x10)) {
			out.val.simmediate = out.val.simmediate | 0xe0;
		}
		break;

	case JRISC_flag:
		ret = jriscValidateFlag(raw, &out.val.flag);
		if (ret != JRISC_success) return ret;
		break;

	case JRISC_unused:
		/* Nothing to do. Could validate it is zero and warn */
		break;

	default:
		return JRISC_ERROR_invalidRegType;
	}

	*regOut = out;

	return ret;
}

uint8_t
jriscRegToRaw(const struct JRISC_OpReg *reg)
{
	switch (reg->type) {
	case JRISC_indirect: /* Fall through */
	case JRISC_reg:
		return (uint8_t)reg->val.reg;

	case JRISC_condition:
		return reg->val.condition;

	case JRISC_shlimmediate:	/* Fall through */
	case JRISC_uimmediate:		/* Fall through */
	case JRISC_zuimmediate:
		return reg->val.uimmediate;

	case JRISC_pcoffset:		/* Fall through */
	case JRISC_simmediate:
		return (uint8_t)(reg->val.simmediate & JRISC_REG_MASK);

	case JRISC_flag:
		return reg->val.flag ? 1 : 0;

	case JRISC_unused:
		return 0;

	default:
		assert(!"Unreachable!");
		return 0;
	}
}

static const struct JRISC_Instruction *
jriscNextInstruction(const struct JRISC_Instruction *current)
{
	uint8_t opNameVal = current->opName;

	if ((opNameVal + 1) >= JRISC_invalidOpName) return NULL;

	return ++current;
}

enum JRISC_Error
jriscInstructionRead(struct JRISC_Context *context,
					 enum JRISC_CPU cpu,
					 struct JRISC_Instruction *instructionOut)
{
	const struct JRISC_Instruction *templates;
	const struct JRISC_Instruction *match = NULL;
	struct JRISC_Instruction out;
	enum JRISC_Error ret;
	uint32_t address;
	uint16_t rawImmediate;
	uint16_t raw;
	uint8_t rawCode;
	uint8_t rawSrc;
	uint8_t rawDst;

	ret = context->read(context, sizeof(raw), &raw, &address);
	if (ret != JRISC_success) return ret;

	/* XXX swap at appropriate times */
	raw = (raw << 8) | (raw >> 8);

	rawCode = raw >> JRISC_OPCODE_SHIFT;
	rawSrc = (raw >> JRISC_REGSRC_SHIFT) & JRISC_REG_MASK;
	rawDst = raw & JRISC_REG_MASK;

	templates = jriscInstructionsFromOpCode(rawCode);
	if (!templates) return JRISC_ERROR_invalidOpCode;

	do {
		if (jriscInstructionMatchesRaw(templates, rawCode, rawSrc, cpu)) {
			match = templates;
			break;
		}
	} while ((templates = jriscNextInstruction(templates)) &&
			  (templates->opCode == rawCode));

	if (!match) return JRISC_ERROR_invalidOpCode;

	out = *match;
	out.address = address;

	ret = jriscRegFromRaw(rawSrc, match->regSrc.type, &out.regSrc);
	if (ret != JRISC_success) return ret;

	ret = jriscRegFromRaw(rawDst, match->regDst.type, &out.regDst);
	if (ret != JRISC_success) return ret;

	/*
	 * movei is special: Its immediate value is taken from the two following
	 * "instruction" slots.
	 */
	if (out.opName == JRISC_op_movei) {
		ret = context->read(context, sizeof(rawImmediate), &rawImmediate, NULL);
		if (ret != JRISC_success) return ret;

		/* XXX swap at appropriate times */
		rawImmediate = (rawImmediate << 8) | (rawImmediate >> 8);

		out.longImmediate = rawImmediate;

		ret = context->read(context, sizeof(rawImmediate), &rawImmediate, NULL);
		if (ret != JRISC_success) return ret;

		/* XXX swap at appropriate times */
		rawImmediate = (rawImmediate << 8) | (rawImmediate >> 8);

		out.longImmediate |= (uint32_t)rawImmediate << 16;
	}

	*instructionOut = out;

	return ret;
}

uint16_t
jriscInstructionLongImmediateLow(const struct JRISC_Instruction *instruction)
{
	return instruction->longImmediate & 0xffff;
}

uint16_t
jriscInstructionLongImmediateHigh(const struct JRISC_Instruction *instruction)
{
	return instruction->longImmediate >> 16;
}
