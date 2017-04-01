#ifndef CACHE_H_INCLUDED
#define CACHE_H_INCLUDED

#include <stdio.h>
#include "pipe_reg.h"

#define CACHE_SIZE 256
#define NUM_REGISTERS 32
#define MEMORY_SIZE 1000

unsigned int memory[MEMORY_SIZE];

unsigned int i_cache[CACHE_SIZE];

unsigned int d_cache[CACHE_SIZE];

unsigned int reg[NUM_REGISTERS];

unsigned int program_image[MEMORY_SIZE];

void init_memory();
unsigned int init_i_cache();
void init_d_cache();
void Initialize_Simulation_Memory();
void init_reg();

#endif // CACHE_H_INCLUDED
