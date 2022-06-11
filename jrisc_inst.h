#ifndef JRISC_INST_H_
#define JRISC_INST_H_

#include "jrisc_ctx.h"
#include "jrisc_regtype.h"

#include <stdint.h>
#include <stdbool.h>

enum JRISC_OpName {
/* Define a macro to generate an instruction name enum */
#define JRISC_OP(opName, opNum, regSrcType, regDstType, swapRegs, cpus) \
	JRISC_op_##opName,
#include "jrisc_optable.h"
#undef JRISC_OP

	JRISC_invalidOpName
};

enum JRISC_CPU {
	JRISC_both,
	JRISC_dsp,
	JRISC_gpu
};

enum JRISC_Reg {
	r0,
	r1,
	r2,
	r3,
	r4,
	r5,
	r6,
	r7,
	r8,
	r9,
	r10,
	r11,
	r12,
	r13,
	r14,
	r15,
	r16,
	r17,
	r18,
	r19,
	r20,
	r21,
	r22,
	r23,
	r24,
	r25,
	r26,
	r27,
	r28,
	r29,
	r30,
	r31
};

#define JRISC_RAWCODE_SHIFT (10)
#define JRISC_REGSRC_SHIFT (5)
#define JRISC_REG_MASK (0x1f)

struct JRISC_OpReg {
	enum JRISC_RegType type;

	union {
		enum JRISC_Reg reg;
		uint8_t condition;
		/* XXX Handle signed/unsigned immediates separately  */
		uint8_t immediate;
		bool flag;
	} val;
};

struct JRISC_Instruction {
    enum JRISC_OpName opName;
	uint8_t opCode;
	struct JRISC_OpReg regSrc;
	struct JRISC_OpReg regDst;
	bool swapRegs;

	enum JRISC_CPU cpu;

	uint32_t longImmediate;
};

extern const struct JRISC_Instruction
jriscInstructionTable[];

extern enum JRISC_Error
jriscInstructionRead(struct JRISC_Context *context,
					 enum JRISC_CPU cpu,
					 struct JRISC_Instruction *instructionOut);

#endif /*  JRISC_INST_H_ */
