CC = gcc
CFLAGS = -c -g -Wall 
LFLAGS = -Wall  

.PHONY: all clean

MIPS.out: MIPS_Processor.c flush_reg.c cache.c MIPS_Processor.h flush_reg.h cache.h pipe_reg.h
	gcc -o MIPS.out MIPS_Processor.c flush_reg.c cache.c -lm
