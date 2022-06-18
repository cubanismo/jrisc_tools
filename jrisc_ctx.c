/*
 * SPDX-License-Identifier: CC0-1.0
 *
 * Author: James Jones
 */

#include "jrisc_base.h"
#include "jrisc_ctx.h"

#include <stdlib.h>

static enum JRISC_Error
jriscContextRead(struct JRISC_Context *context,
				 uint64_t size,
				 void *dst,
				 uint32_t *address)
{
	enum JRISC_Error ret = context->readFunc(context->userData,
											 context->readLocation,
											 size,
											 dst);

	if (JRISC_success == ret) {
		if (address) *address = context->readAddress;
		context->readLocation += size;
		context->readAddress += size;
	}

	return ret;
}

static enum JRISC_Error
jriscContextWrite(struct JRISC_Context *context,
				  uint64_t size,
				  const void *src,
				  uint32_t *address)
{
	enum JRISC_Error ret = context->writeFunc(context->userData,
											  context->writeLocation,
											  size,
											  src);

	if (JRISC_success == ret) {
		if (address) *address = context->writeAddress;
		context->writeLocation += size;
		context->writeAddress += size;
	}

	return ret;
}

enum JRISC_Error
jriscContextCreate(JRISC_ReadFunc readFunc,
				   JRISC_WriteFunc writeFunc,
				   JRISC_DestructorFunc userDestructor,
				   void *userData,
				   uint32_t baseAddress,
				   struct JRISC_Context **contextOut)
{
	struct JRISC_Context *ctx = calloc(1, sizeof(*ctx));

	if (!ctx)
	{
		return JRISC_ERROR_outOfMemory;
	}

	ctx->readFunc = readFunc;
	ctx->writeFunc = writeFunc;
	ctx->userDestructor = userDestructor;
	ctx->userData = userData;
	ctx->readLocation = 0;
	ctx->writeLocation = 0;
	ctx->readAddress = baseAddress;
	ctx->writeAddress = baseAddress;
	ctx->read = jriscContextRead;
	ctx->write = jriscContextWrite;

	*contextOut = ctx;

	return JRISC_success;
}

void
jriscContextDestroy(struct JRISC_Context *context)
{
	if (context->userDestructor) context->userDestructor(context->userData);

	free(context);
}
