/*
 * SPDX-License-Identifier: CC0-1.0
 *
 * Author: James Jones
 */

#ifndef JRISC_CONTEXT_FILE_H_
#define JRISC_CONTEXT_FILE_H_

#include "jrisc_ctx.h"

#include <stdio.h>

extern enum JRISC_Error
jriscContextFromFile(FILE *fpRead,
					 uint64_t readOffset,
					 FILE *fpWrite,
					 uint64_t writeOffset,
					 uint32_t baseAddress,
					 struct JRISC_Context **contextOut);

#endif /* JRISC_CONTEXT_FILE_H_ */
