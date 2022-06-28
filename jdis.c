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

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

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
	printf("Usage: jdis [-gdamhv] [-o <offset>] [-b <base address>] <JRISC machine code file>\n");
	printf("\n");
	printf("Options:\n");
	printf("  -g: Parse code as Tom/GPU instructions [default].\n");
	printf("  -d: Parse code as Jerry/DSP instructions.\n");
	printf("  -a: Print address in hex of each disassembled word.\n");
	printf("  -m: Print machine code in hex of each disassembled word.\n");
	printf("  -o <offset>: Specify offset into file (0x<hex> or <decimal>)\n");
	printf("  -b <base address>: Specify the base load address of the code\n");
	printf("  -h: Help. Print this text.\n");
	printf("  -v: Version. Print the version and exit.\n");
	printf("\n");
	printf("Offsets and addresses are parsed as hex if they start '0x',\n");
	printf("  octal if they start with '0', or decimal otherwise.\n");
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
	uint64_t fileOffset = 0;
	static const uint32_t defaultGpuBase = 0xf03000;
	static const uint32_t defaultDspBase = 0xf1b000;
	uint32_t baseAddress = 0;
	bool baseSpecified = false;
	uint32_t stringFlags = 0;
	int i;
	int j;
	bool skipParam;
	char *end;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			for (j = 1, skipParam = false; !skipParam && argv[i][j]; j++) {
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

				case 'a':
					stringFlags |= JRISC_STRINGFLAG_ADDRESS;
					break;

				case 'm':
					stringFlags |= JRISC_STRINGFLAG_MACHINE_CODE;
					break;

				case 'o':
					if ((argv[i][j+1]) || (++i >= argc)) {
						usage();
						exit(1);
					}
					errno = 0;
					fileOffset = strtoll(argv[i], &end, 0);
					if (errno != 0) {
						perror("Error parsing file offset");
						printf("\n");
						usage();
						exit(1);
					}
					if (!argv[i][0] || end[0]) {
						printf("Error parsing file offset\n\n");
						usage();
						exit(1);
					}
					skipParam = true;
					break;

				case 'b':
					if ((argv[i][j+1]) || (++i >= argc)) {
						usage();
						exit(1);
					}
					errno = 0;
					baseAddress = strtol(argv[i], &end, 0);
					if (errno != 0) {
						perror("Error parsing base address");
						printf("\n");
						usage();
						exit(1);
					}
					if (!argv[i][0] || end[0]) {
						printf("Error parsing base address\n\n");
						usage();
						exit(1);
					}
					baseSpecified = true;
					skipParam = true;
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

	if (!baseSpecified) {
		baseAddress = (cpu == JRISC_gpu) ? defaultGpuBase : defaultDspBase;
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

	err = jriscContextFromFile(fp, fileOffset,		/* read file */
							   NULL, 0,				/* write file+offset */
							   baseAddress,
							   &ctx);

	if (err != JRISC_success) {
		fprintf(stderr, "Failed to create context\n");
		exit(1);
	}

	while ((err = jriscInstructionRead(ctx, cpu, &inst)) == JRISC_success)
		jriscInstructionPrint(&inst, stringFlags);

	jriscContextDestroy(ctx);

	return 0;
}
