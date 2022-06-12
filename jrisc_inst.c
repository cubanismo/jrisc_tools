#include "jrisc_base.h"
#include "jrisc_ctx.h"
#include "jrisc_inst.h"

#include <stddef.h>

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
	/* XXX Handle signed/unsigned immediates separately */
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

	case JRISC_pcoffset: /* Fall through */
	case JRISC_immediate:
		ret = jriscValidateImmediate(raw, &out.val.immediate);
		if (ret != JRISC_success) return ret;
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
	uint16_t rawImmediate;
	uint16_t raw;
	uint8_t rawCode;
	uint8_t rawSrc;
	uint8_t rawDst;

	ret = context->read(context, sizeof(raw), &raw);
	if (ret != JRISC_success) return ret;

	/* XXX swap at appropriate times */
	raw = (raw << 8) | (raw >> 8);

	rawCode = raw >> JRISC_RAWCODE_SHIFT;
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

	ret = jriscRegFromRaw(rawSrc, match->regSrc.type, &out.regSrc);
	if (ret != JRISC_success) return ret;

	ret = jriscRegFromRaw(rawDst, match->regDst.type, &out.regDst);
	if (ret != JRISC_success) return ret;

	/*
	 * movei is special: Its immediate value is taken from the two following
	 * "instruction" slots.
	 */
	if (out.opName == JRISC_op_movei) {
		ret = context->read(context, sizeof(rawImmediate), &rawImmediate);
		if (ret != JRISC_success) return ret;

		/* XXX swap at appropriate times */
		rawImmediate = (rawImmediate << 8) | (rawImmediate >> 8);

		out.longImmediate = rawImmediate;

		ret = context->read(context, sizeof(rawImmediate), &rawImmediate);
		if (ret != JRISC_success) return ret;

		/* XXX swap at appropriate times */
		rawImmediate = (rawImmediate << 8) | (rawImmediate >> 8);

		out.longImmediate |= (uint32_t)rawImmediate << 16;
	}

	*instructionOut = out;

	return ret;
}
