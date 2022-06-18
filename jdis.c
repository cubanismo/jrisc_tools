/*
 * SPDX-License-Identifier: CC0-1.0
 *
 * Author: James Jones
 */

#include "jrisc_base.h"
#include "jrisc_ctx.h"
#include "jrisc_ctx_file.h"
#include "jrisc_inst.h"
#include "jrisc_inst_string.h"

#include <stdlib.h>
#include <stdio.h>

static void
version(void)
{
	printf("Jaguar RISC Disassembler Version %d.%d.%d\n",
		   JDIS_MAJOR, JDIS_MINOR, JDIS_MICRO);
}

static void
usage(void)
{
	version();
	printf("\n");
	printf("Usage: jdis [-gdhv] <JRISC machine code file>\n");
	printf("\n");
	printf("Options:\n");
	printf("  -g: Parse code as Tom/GPU instructions [default].\n");
	printf("  -d: Parse code as Jerry/DSP instructions.\n");
	printf("  -h: Help. Print this text.\n");
	printf("  -v: Version. Print the version and exit.\n");
}

int
main(int argc, char *argv[])
{
	FILE *fp;
	struct JRISC_Context *ctx;
	struct JRISC_Instruction inst;
	enum JRISC_Error err;
	const char *fileName = NULL;
	enum JRISC_CPU cpu = JRISC_gpu;
	int i;
	int j;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			for (j = 1; argv[i][j]; j++) {
				switch (argv[i][j]) {
				case 'h':
					usage();
					exit(0);

				case 'v':
					version();
					exit(0);

				case 'g':
					cpu = JRISC_gpu;
					break;

				case 'd':
					cpu = JRISC_dsp;
					break;

				default:
					usage();
					exit(1);
				}
			}
		} else if (!fileName) {
			fileName = argv[i];
		} else {
			usage();
			exit(1);
		}
	}
	
	if (!fileName) {
		usage();
		exit(1);
	}

	fp = fopen(fileName, "rb");

	if (!fp) {
		fprintf(stderr, "Could not open %s\n", fileName);
		exit(1);
	}

	err = jriscContextFromFile(fp, fp, &ctx);
	if (err != JRISC_success) {
		fprintf(stderr, "Failed to create context\n");
		exit(1);
	}

	while ((err = jriscInstructionRead(ctx, cpu, &inst)) == JRISC_success)
		jriscInstructionPrint(&inst);

	jriscContextDestroy(ctx);

	return 0;
}
