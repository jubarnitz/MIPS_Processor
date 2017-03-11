#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include "pipe_reg.h"

#define CACHE_SIZE 256

unsigned int memory[10000];

unsigned int i_cache[CACHE_SIZE];

unsigned int d_cache[CACHE_SIZE];

void init_memory();
unsigned int init_i_cache();
void init_d_cache();

#endif