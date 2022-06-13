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

extern void
jriscInstructionToString(const struct JRISC_Instruction *instruction,
						 char *string,
						 size_t *stringLengthInOut);

extern enum JRISC_Error
jriscInstructionPrint(const struct JRISC_Instruction *instruction);

#endif /* JRISC_INST_STRING_H_ */
