#ifndef JRISC_CONTEXT_MEM_H_
#define JRISC_CONTEXT_MEM_H_

#include "jrisc_ctx.h"

#include <stddef.h>

extern enum JRISC_Error
jriscContextFromMemory(void *memory,
					   size_t memorySize,
					   struct JRISC_Context **contextOut);

#endif /* JRISC_CONTEXT_MEM_H_ */
