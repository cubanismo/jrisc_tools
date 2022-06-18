/*
 * SPDX-License-Identifier: CC0-1.0
 *
 * Author: James Jones
 */

#include "jrisc_base.h"
#include "jrisc_ctx.h"
#include "jrisc_ctx_mem.h"

#include <stdlib.h>
#include <string.h>

struct MemoryContext {
	const uint8_t *memoryRead;
	size_t sizeRead;
	uint8_t *memoryWrite;
	size_t sizeWrite;
};

static enum JRISC_Error
jriscMemoryRead(void *userData,
				uint64_t location,
				uint64_t size,
				void *dst)
{
	struct MemoryContext *mCtx = (struct MemoryContext *)userData;

	if (!mCtx->memoryRead) return JRISC_ERROR_ioError;
	if ((location + size) > mCtx->sizeRead) return JRISC_ERROR_ioError;

	memcpy(dst, &mCtx->memoryRead[location], size);

	return JRISC_success;
}

static enum JRISC_Error
jriscMemoryWrite(void *userData,
				 uint64_t location,
				 uint64_t size,
				 const void *src)
{
	struct MemoryContext *mCtx = (struct MemoryContext *)userData;

	if (!mCtx->memoryWrite) return JRISC_ERROR_ioError;
	if ((location + size) > mCtx->sizeWrite) return JRISC_ERROR_ioError;

	memcpy(&mCtx->memoryWrite[location], src, size);

	return JRISC_success;
}

static void
jriscDestroyMemory(void *userData)
{
	free(userData);
}

enum JRISC_Error
jriscContextFromMemory(const void *memoryRead,
					   size_t sizeRead,
					   void *memoryWrite,
					   size_t sizeWrite,
					   uint32_t baseAddress,
					   struct JRISC_Context **contextOut)
{
	struct MemoryContext *mCtx = calloc(1, sizeof(*mCtx));
	enum JRISC_Error ret;

	if (!mCtx) return JRISC_ERROR_outOfMemory;

	mCtx->memoryRead = memoryRead;
	mCtx->sizeRead = sizeRead;
	mCtx->memoryWrite = memoryWrite;
	mCtx->sizeWrite = sizeWrite;

	ret = jriscContextCreate(jriscMemoryRead,
							 jriscMemoryWrite,
							 jriscDestroyMemory,
							 mCtx,
							 baseAddress,
							 contextOut);

	if (JRISC_success != ret) jriscDestroyMemory(mCtx);

	return ret;
}
