#ifndef CACHE_H_INCLUDED
#define CACHE_H_INCLUDED

#include <stdio.h>
#include "pipe_reg.h"
#include <math.h>

#define WRITE_BACK 0

#define ICACHE_ON 0                 // 0 = false, 1 = true
#define ICACHE_SIZE 128             // bytes
#define ICACHE_BLOCK_SIZE 1        // words

#define DCACHE_ON 1                 // 0 = false, 1 = true
#define DCACHE_SIZE 128             // bytes
#define DCACHE_BLOCK_SIZE 1        // words

#define NUM_REGISTERS 32
#define MEMORY_SIZE 3000            // words

struct I_Cache
{
    unsigned int data[(ICACHE_SIZE / 4)];
    unsigned int tag[(ICACHE_SIZE / 4) / ICACHE_BLOCK_SIZE];
    unsigned int valid[(ICACHE_SIZE / 4) / ICACHE_BLOCK_SIZE];
    unsigned int dirty[(ICACHE_SIZE / 4) / ICACHE_BLOCK_SIZE];
};

struct D_Cache
{
    unsigned int data[(DCACHE_SIZE / 4)];
    unsigned int tag[(DCACHE_SIZE / 4) / DCACHE_BLOCK_SIZE];
    unsigned int valid[(DCACHE_SIZE / 4) / DCACHE_BLOCK_SIZE];
    unsigned int dirty[(DCACHE_SIZE / 4) / DCACHE_BLOCK_SIZE];
};


//unsigned int icache_index_mask;
//unsigned int icache_tag_mask;
unsigned int icache_entries;
unsigned int dcache_entries;

unsigned int i_block_offset_bits;
unsigned int d_block_offset_bits;

unsigned int i_block_index;
unsigned int d_block_index;


unsigned int i_block_offset;
unsigned int d_block_offset;

unsigned int mem_base_addr;

int filling_i_cache;
int filling_d_cache;
int i_cache_hit;
int d_cache_hit;
int i_cache_penalty;
int d_cache_penalty;
int main_memory_penalty;
int mem_first_entry_filled;
int mem_penalty_count;
int mem_handling_icache_req;
int mem_handling_dcache_req;
int mem_handling_write_req;
int reset_mem_penalty_count;


unsigned int memory[MEMORY_SIZE];

struct I_Cache i_cache;

struct D_Cache d_cache;

unsigned int reg[NUM_REGISTERS];

unsigned int program_image[MEMORY_SIZE];

void init_memory();
unsigned int init_i_cache();
void init_d_cache();
void Initialize_Simulation_Memory();
void init_reg();
int icache_access(unsigned int address, unsigned int *data);
int dcache_access(int read, unsigned int address, unsigned int *data);
int memory_access(int read, unsigned int address, unsigned int *data, int block_size, int i_cache_Request);

#endif // CACHE_H_INCLUDED
