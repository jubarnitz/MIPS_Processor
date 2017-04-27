#ifndef CACHE_H_INCLUDED
#define CACHE_H_INCLUDED

#include <stdio.h>
#include "pipe_reg.h"
#include <math.h>

#define ICACHE_SIZE 64              // bytes
#define DCACHE_SIZE 256             // bytes
#define ICACHE_BLOCK_SIZE 1         // words
#define DCACHE_BLOCK_SIZE 1         // words

#define NUM_REGISTERS 32
#define MEMORY_SIZE 3000            // words

struct Cache
{
    unsigned int data[(ICACHE_SIZE / 4)];
    unsigned int tag[(ICACHE_SIZE / 4) / ICACHE_BLOCK_SIZE];
    unsigned int valid[(ICACHE_SIZE / 4) / ICACHE_BLOCK_SIZE];
    unsigned int dirty[(ICACHE_SIZE / 4) / ICACHE_BLOCK_SIZE];
};


unsigned int icache_index_mask;
unsigned int icache_tag_mask;
unsigned int icache_entries;
unsigned int block_offset_bits;
unsigned int block_index;

int filling_i_cache;
int filling_d_cache;
int i_cache_hit;
int d_cache_hit;
int i_cache_penalty;
int d_cache_penalty;
int main_memory_penalty;
int mem_first_entry_filled;
int mem_penalty_count;

unsigned int memory[MEMORY_SIZE];

struct Cache i_cache;

struct Cache d_cache;

unsigned int reg[NUM_REGISTERS];

unsigned int program_image[MEMORY_SIZE];

void init_memory();
unsigned int init_i_cache();
void init_d_cache();
void Initialize_Simulation_Memory();
void init_reg();
int icache_access(unsigned int address, unsigned int *data);
int dcache_access(int read, unsigned int address, unsigned int *data);
int memory_access(int read, unsigned int address, unsigned int *data, int block_size);

#endif // CACHE_H_INCLUDED
