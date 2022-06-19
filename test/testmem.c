/*
 * SPDX-License-Identifier: CC0-1.0
 *
 * Author: James Jones
 */

#include "jrisc_base.h"
#include "jrisc_ctx.h"
#include "jrisc_ctx_mem.h"
#include "jrisc_inst.h"
#include "jrisc_inst_string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int
main(int argc, char *argv[])
{
	const uint8_t mem[] = {
		0x98, 0x1f, 0x05, 0xbc, 0x00, 0x00, 0xbf, 0xe0, 0x08, 0x9f, 0xa7,
		0xe0, 0x98, 0x1f, 0x21, 0x14, 0x00, 0xf0, 0x8c, 0x1e, 0xbf, 0xfe,
		0xd7, 0xc0, 0xe4, 0x00, 0xe4, 0x00
	};
	size_t len = sizeof(mem);

	struct JRISC_Context *ctx;
	struct JRISC_Instruction inst;

	if (jriscContextFromMemory(mem, len, NULL, 0, 0, &ctx) != JRISC_success) {
		/* Throw some exception */
		printf("Failed to create context\n");
		return 1;
	}

	while (jriscInstructionRead(ctx, JRISC_gpu, &inst) == JRISC_success) {
		jriscInstructionPrint(&inst,
							  JRISC_STRINGFLAG_ADDRESS |
							  JRISC_STRINGFLAG_MACHINE_CODE);
	}

error:
	jriscContextDestroy(ctx);

	return 0;
}
