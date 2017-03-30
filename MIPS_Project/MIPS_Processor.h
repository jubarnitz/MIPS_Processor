#ifndef MIPS_PROCESSOR_H_INCLUDED
#define MIPS_PROCESSOR_H_INCLUDED

#include <stdio.h>
#include "pipe_reg.h"
#include "cache.h"
#include "flush_reg.h"

#define OP_MASK 0xFC000000
#define rs_MASK 0x03E00000
#define rt_MASK 0x1F0000
#define rd_MASK 0xF800
#define func_MASK 0x3F
#define imm_MASK 0xFFFF
#define jump_MASK 0x3FFFFFF
#define sham_MASK 0x7C0

void IF();
void ID();
void EX();
void MEM();
void WB();
void Update();

int clock_cycle;

IF_ID IFID;
ID_EX IDEX;
EX_MEM EXMEM;
MEM_WB MEMWB;
PC_Reg PC;
IF_ID IFID_SHADOW;
ID_EX IDEX_SHADOW;
EX_MEM EXMEM_SHADOW;
MEM_WB MEMWB_SHADOW;
PC_Reg PC_SHADOW;

IF_ID *IFID_ptr = &IFID;
ID_EX *IDEX_ptr = &IDEX;
EX_MEM *EXMEM_ptr = &EXMEM;
MEM_WB *MEMWB_ptr = &MEMWB;

#endif // MIPS_PROCESSOR_H_INCLUDED
