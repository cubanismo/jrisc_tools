/*
 * SPDX-License-Identifier: CC0-1.0
 *
 * Author: James Jones
 */

#ifndef JRISC_CONTEXT_MEM_H_
#define JRISC_CONTEXT_MEM_H_

#include "jrisc_ctx.h"

#include <stddef.h>

extern enum JRISC_Error
jriscContextFromMemory(const void *readMemory,
					   size_t readSize,
					   void *writeMemory,
					   size_t writeSize,
					   struct JRISC_Context **contextOut);

#endif /* JRISC_CONTEXT_MEM_H_ */
