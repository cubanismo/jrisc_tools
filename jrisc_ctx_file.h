#ifndef JRISC_CONTEXT_FILE_H_
#define JRISC_CONTEXT_FILE_H_

#include "jrisc_ctx.h"

#include <stdio.h>

extern enum JRISC_Error
jriscContextFromFile(FILE *fpRead,
					 FILE *fpWrite,
					 struct JRISC_Context **contextOut);

#endif /* JRISC_CONTEXT_FILE_H_ */
