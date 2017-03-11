/**
 *
 * ECEN 4593 Computer Organization
 * Justin Barnitz and Jacob Levine
 *
 */
#include <stdio.h>
#include "pipe_reg.h"
#include "cache.h"

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
	Init_IF_ID(IFID_ptr);
	Init_ID_EX(IDEX_ptr);

	/*----------  Execute MIPS program  ----------*/
	
	
}

void Init_IF_ID(IF_ID *IFID_ptr)
{
	printf("Initializing IF/ID Pipeline Register\n");
	IFID_ptr->PC_Next = 0;
	IFID_ptr->reg_RS = 0;
	IFID_ptr->reg_RT = 0;
	IFID_ptr->reg_RD = 0;
	IFID_ptr->OP_Code = 0;
	IFID_ptr->sham = 0;
	IFID_ptr->func = 0;
	IFID_ptr->jmp_addr = 0;
	IFID_ptr->imm = 0;
}

void Init_ID_EX(ID_EX *IDEX_ptr)
{
	printf("Initializing ID/EX Pipeline Register\n");
	IDEX_ptr->ALU_Src = 0;
	IDEX_ptr->ALU_Op = 0;
	IDEX_ptr->Reg_Dst = 0;
	IDEX_ptr->Reg_Wrt = 0;
	IDEX_ptr->Mem_to_Reg = 0;
	IDEX_ptr->Mem_Read = 0;
	IDEX_ptr->Mem_Wrt = 0;
	IDEX_ptr->Reg_RS_val = 0;
	IDEX_ptr->Reg_RT_val = 0;
	IDEX_ptr->reg_RS = 0;
	IDEX_ptr->reg_RT = 0;
	IDEX_ptr->reg_RD = 0;
	IDEX_ptr->PC_Next = 0;
	IDEX_ptr->sign_ext_imm = 0;
}