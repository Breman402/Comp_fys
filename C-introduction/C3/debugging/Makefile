CC = gcc

LIBS = \
       -lm

CFLAGS_ADDRESS = \
	 	 -Werror \
		 -Wall \
	 	 -pedantic \
	 	 -g \
	 	 -fsanitize=address \
		 -fno-omit-frame-pointer

CFLAGS = \
	-Werror \
	-pedantic \
	-g \
	-O0


address: clean
	$(CC) $(CFLAGS_ADDRESS) fix_me.c -o $@ $(LIBS)

gdb: clean
	$(CC) $(CFLAGS) fix_me.c -o program_gdb $(LIBS)

clean:
	find . -name '*.o' -exec rm -rf {} \;
	rm -rf program_gdb address
