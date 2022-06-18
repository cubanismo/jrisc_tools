/*
 * SPDX-License-Identifier: CC0-1.0
 *
 * Author: James Jones
 */

#include "jrisc_base.h"
#include "jrisc_ctx.h"
#include "jrisc_ctx_file.h"

#include <stdlib.h>

struct FileContext {
	FILE *fpRead;
	FILE *fpWrite;

	uint64_t readOffset;
	uint64_t writeOffset;

	/* Cache these locally to improve support for pipes/non-seekable files */
	uint64_t readLocation;
	uint64_t writeLocation;
};

static enum JRISC_Error
jriscFileRead(void *userData,
			  uint64_t location,
			  uint64_t size,
			  void *dst)
{
	struct FileContext *fCtx = (struct FileContext *)userData;
	FILE *f = fCtx->fpRead;
	uint64_t fileLocation = location + fCtx->readOffset;

	if (!f) return JRISC_ERROR_ioError;

	if (fCtx->readLocation != fileLocation) {
		if (fseek(f, fileLocation, SEEK_SET)) {
			return JRISC_ERROR_ioError;
		}
		fCtx->readLocation = fileLocation;
	}

	if (fread(dst, 1, size, f) != size) {
		return JRISC_ERROR_ioError;
	}

	fCtx->readLocation += size;

	return JRISC_success;
}

static enum JRISC_Error
jriscFileWrite(void *userData,
			   uint64_t location,
			   uint64_t size,
			   const void *src)
{
	struct FileContext *fCtx = (struct FileContext *)userData;
	FILE *f = fCtx->fpWrite;
	uint64_t fileLocation = location + fCtx->readOffset;

	if (!f) return JRISC_ERROR_ioError;

	if (fCtx->writeLocation != fileLocation) {
		if (fseek(f, fileLocation, SEEK_SET)) {
			return JRISC_ERROR_ioError;
		}
		fCtx->writeLocation = fileLocation;
	}

	if (fwrite(src, 1, size, f) != size) {
		return JRISC_ERROR_ioError;
	}

	fCtx->writeLocation += size;

	return JRISC_success;
}

static void
jriscDestroyFileData(void *userData)
{
	free(userData);
}

enum JRISC_Error
jriscContextFromFile(FILE *fpRead,
					 uint64_t readOffset,
					 FILE *fpWrite,
					 uint64_t writeOffset,
					 uint32_t baseAddress,
					 struct JRISC_Context **contextOut)
{
	struct FileContext *fCtx = calloc(1, sizeof(*fCtx));
	enum JRISC_Error ret;

	if (!fCtx) {
		return JRISC_ERROR_outOfMemory;
	}

	fCtx->fpRead = fpRead;
	fCtx->readOffset = readOffset;
	fCtx->fpWrite = fpWrite;
	fCtx->writeOffset = writeOffset;

	ret = jriscContextCreate(jriscFileRead,
							 jriscFileWrite,
							 jriscDestroyFileData,
							 fCtx,
							 baseAddress,
							 contextOut);

	if (ret != JRISC_success) free(fCtx);

	return ret;
}

