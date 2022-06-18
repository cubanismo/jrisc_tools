/*
 * SPDX-License-Identifier: CC0-1.0
 *
 * Author: James Jones
 */

#ifndef JRISC_CTX_H_
#define JRISC_CTX_H_

#include "jrisc_base.h"

#include <stdint.h>

typedef enum JRISC_Error (*JRISC_ReadFunc)(void *userData,
										   uint64_t location,
										   uint64_t size,
										   void *dst);

typedef enum JRISC_Error (*JRISC_WriteFunc)(void *userData,
											uint64_t location,
											uint64_t size,
											const void *src);

typedef void (*JRISC_DestructorFunc)(void *userData);

struct JRISC_Context {
	JRISC_ReadFunc readFunc;
	JRISC_WriteFunc writeFunc;
	JRISC_DestructorFunc userDestructor;
	void *userData;

	uint64_t readLocation;
	uint32_t readAddress;
	uint64_t writeLocation;
	uint32_t writeAddress;

	enum JRISC_Error (*read)(struct JRISC_Context *context,
							 uint64_t size,
							 void *dst,
							 uint32_t *address);
	enum JRISC_Error (*write)(struct JRISC_Context *context,
							  uint64_t size,
							  const void *src,
							  uint32_t *address);
};

extern enum JRISC_Error
jriscContextCreate(JRISC_ReadFunc readfunc,
				   JRISC_WriteFunc writefunc,
				   JRISC_DestructorFunc userDestructor,
				   void *userData,
				   uint32_t baseAddress,
				   struct JRISC_Context **contextOut);

extern void
jriscContextDestroy(struct JRISC_Context *context);

#endif /* JRISC_CTX_H_ */
