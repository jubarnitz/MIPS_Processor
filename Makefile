CC = gcc
CFLAGS = -c -g -Wall 
LFLAGS = -Wall  

.PHONY: all clean

MIPS.out: MIPS_Processor.c flush_reg.c cache.c
	gcc -o MIPS.out MIPS_Processor.c flush_reg.c cache.c -lm