/*
 * SPDX-License-Identifier: CC0-1.0
 *
 * Author: James Jones
 */

#ifndef JRISC_INST_STRING_H_
#define JRISC_INST_STRING_H_

#include "jrisc_base.h"
#include "jrisc_inst.h"

#include <stddef.h>

#define JRISC_STRINGFLAG_ADDRESS				0x000000001
#define JRISC_STRINGFLAG_MACHINE_CODE			0x000000002

extern void
jriscInstructionToString(const struct JRISC_Instruction *instruction,
						 uint32_t flags,
						 char *string,
						 size_t *stringLengthInOut);

extern enum JRISC_Error
jriscInstructionPrint(const struct JRISC_Instruction *instruction,
					  uint32_t flags);

#endif /* JRISC_INST_STRING_H_ */
