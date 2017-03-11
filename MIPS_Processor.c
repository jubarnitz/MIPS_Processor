/**
 *
 * ECEN 4593 Computer Organization
 * Justin Barnitz and Jacob Levine
 *
 */
#include <stdio.h>
#include "pipe_reg.h"
#include "cache.h"
#include "flush_reg.h"

void Init_IF_ID(IF_ID *);
void Init_ID_EX(ID_EX *);
void Init_EX_MEM(EX_MEM *);
void Init_MEM_WB(MEM_WB *);

int main()
{
	IF_ID IFID;
	ID_EX IDEX;
	EX_MEM EXMEM;
	MEM_WB MEMWB;
	PC_Reg PC;

	IF_ID *IFID_ptr = &IFID;
	ID_EX *IDEX_ptr = &IDEX;
	EX_MEM *EXMEM_ptr = &EXMEM;
	MEM_WB *MEMWB_ptr = &MEMWB;
	
	/*----------  Init State Elements   ----------*/
	printf("Starting Processor\n");
	
	Flush_IF_ID(IFID_ptr);
	Flush_ID_EX(IDEX_ptr);
	Flush_EX_MEM(EXMEM_ptr);
	Flush_MEM_WB(MEMWB_ptr);

	init_memory();
	init_i_cache();
	init_d_cache();

	/*----------  Execute MIPS program  ----------*/
	
	
}
