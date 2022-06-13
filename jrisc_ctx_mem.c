/*
 * SPDX-License-Identifier: CC0-1.0
 *
 * Author: James Jones
 */

#include "jrisc_base.h"
#include "jrisc_ctx.h"
#include "jrisc_ctx_mem.h"

#include  <string.h>

static enum JRISC_Error
jriscMemoryRead(void *data,
				void *userData,
				uint64_t location,
				uint64_t size,
				void *dst)
{
	uint64_t memorySize = (uint64_t)userData;
	const uint8_t *data8 = data;

	if ((location + size) > memorySize) return JRISC_ERROR_ioError;

	memcpy(dst, &data8[location], size);

	return JRISC_success;
}

static enum JRISC_Error
jriscMemoryWrite(void *data,
				 void *userData,
				 uint64_t location,
				 uint64_t size,
				 const void *src)
{
	uint64_t memorySize = (uint64_t)userData;
	uint8_t *data8 = data;

	if ((location + size) > memorySize) return JRISC_ERROR_ioError;

	memcpy(&data8[location], src, size);

	return JRISC_success;
}

enum JRISC_Error
jriscContextFromMemory(void *memory,
					   size_t memorySize,
					   struct JRISC_Context **contextOut)
{
	return jriscContextCreate(jriscMemoryRead,
							  memory,
							  jriscMemoryWrite,
							  memory,
							  (void *)memorySize,
							  contextOut);
}
