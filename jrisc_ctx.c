#include "jrisc_base.h"
#include "jrisc_ctx.h"

#include <stdlib.h>

static enum JRISC_Error
jriscContextRead(struct JRISC_Context *context, uint64_t size, void *dst)
{
	enum JRISC_Error ret = context->readFunc(context->readData,
											 context->userData,
											 context->location,
											 size,
											 dst);

	if (JRISC_success == ret) context->location += size;

	return ret;
}

static enum JRISC_Error
jriscContextWrite(struct JRISC_Context *context, uint64_t size, const void *src)
{
	enum JRISC_Error ret = context->writeFunc(context->writeData,
											  context->userData,
											  context->location,
											  size,
											  src);

	if (JRISC_success == ret) context->location += size;

	return ret;
}

enum JRISC_Error
jriscContextCreate(JRISC_ReadFunc readFunc,
				   void *readData,
				   JRISC_WriteFunc writeFunc,
				   void *writeData,
				   void *userData,
				   struct JRISC_Context **contextOut)
{
	struct JRISC_Context *ctx = calloc(1, sizeof(*ctx));

	if (!ctx)
	{
		return JRISC_ERROR_outOfMemory;
	}

	ctx->readFunc = readFunc;
	ctx->readData = readData;
	ctx->writeFunc = writeFunc;
	ctx->writeData = writeData;
	ctx->userData = userData;
	ctx->location = 0;
	ctx->read = jriscContextRead;
	ctx->write = jriscContextWrite;

	*contextOut = ctx;

	return JRISC_success;
}
