CFLAGS=-O0 -g

jdis: jdis.c jrisc_ctx.o jrisc_ctx_file.o jrisc_inst.o
	$(CC) $(CFLAGS) -o $@ $^

jrisc_ctx.o: jrisc_ctx.c
	$(CC) $(CFLAGS) -c -o $@ $<

jrisc_ctx_file.o: jrisc_ctx_file.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
jrisc_inst.o: jrisc_inst.c
	$(CC) $(CFLAGS) -c -o $@ $<
