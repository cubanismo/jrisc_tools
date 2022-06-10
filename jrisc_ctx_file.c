#include "jrisc_base.h"
#include "jrisc_ctx.h"
#include "jrisc_ctx_file.h"

static enum JRISC_Error
jriscFileRead(void *data,
			  void *userData,
			  uint64_t location,
			  uint64_t size,
			  void *dst)
{
	FILE *f = data;

	if ((uint64_t)userData != location) {
		if (fseek(f, location, SEEK_SET)) {
			return JRISC_ERROR_ioError;
		}
	}

	userData = (void *)(location + size);

	if (fread(dst, 1, size, f) != size) {
		return JRISC_ERROR_ioError;
	}

	return JRISC_success;
}

static enum JRISC_Error
jriscFileWrite(void *data,
			   void *userData,
			   uint64_t location,
			   uint64_t size,
			   const void *src)
{
	FILE *f = data;
	(void)userData;

	if ((uint64_t)userData != location) {
		if (fseek(f, location, SEEK_SET)) {
			return JRISC_ERROR_ioError;
		}
	}

	userData = (void *)(location + size);

	if (fwrite(src, 1, size, f) != size) {
		return JRISC_ERROR_ioError;
	}

	return JRISC_success;
}

enum JRISC_Error
jriscContextFromFile(FILE *fpRead,
					 FILE *fpWrite,
					 struct JRISC_Context **contextOut)
{
	return jriscContextCreate(jriscFileRead,
							  fpRead,
							  jriscFileWrite,
							  fpWrite,
							  NULL,
							  contextOut);
}
