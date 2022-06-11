#include "jrisc_base.h"
#include "jrisc_ctx.h"
#include "jrisc_ctx_file.h"
#include "jrisc_inst.h"
#include "jrisc_inst_string.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	FILE *fp;
	struct JRISC_Context *ctx;
	struct JRISC_Instruction inst;
	enum JRISC_Error err;
	
	if (argc < 2) {
		printf("Usage: test <raw JRISC machine code>\n");
		exit(1);
	}

	fp = fopen(argv[1], "rb");

	if (!fp) {
		printf("Could not open %s\n", argv[1]);
		exit(1);
	}

	err = jriscContextFromFile(fp, fp, &ctx);
	if (err != JRISC_success) {
		printf("Failed to create context\n");
		exit(1);
	}

	while ((err = jriscInstructionRead(ctx, JRISC_gpu, &inst)) == JRISC_success)
		jriscInstructionPrint(&inst);

	return 0;
}
