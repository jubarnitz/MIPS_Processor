/**
 *
 * ECEN 4593 Computer Organization
 * Justin Barnitz and Jacob Levine
 *
 */

#include "flush_reg.h"

void Flush_IF_ID(IF_ID *IFID_ptr)
{
	printf("Flushing IF/ID Pipeline Register\n");
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

void Flush_ID_EX(ID_EX *IDEX_ptr)
{
	printf("Flushing ID/EX Pipeline Register\n");
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
	IDEX_ptr->OP_Code = 0;
	IDEX_ptr->branch = 0;
	IDEX_ptr->WB_reg = 0;
	IDEX_ptr->I_format = 0;
}

void Flush_EX_MEM(EX_MEM *EXMEM_ptr)
{
	EXMEM_ptr->Reg_Wrt = 0;
	EXMEM_ptr->Mem_Read = 0;
	EXMEM_ptr->Mem_to_Reg = 0;
	EXMEM_ptr->Mem_Wrt = 0;
	EXMEM_ptr->Reg_RT_val = 0;
	EXMEM_ptr->WB_reg = 0;
	EXMEM_ptr->PC_Next = 0;
	EXMEM_ptr->ALU_result = 0;
}

void Flush_MEM_WB(MEM_WB *MEMWB_ptr)
{
	MEMWB_ptr->Reg_Wrt = 0;
	MEMWB_ptr->Data_Mem_result = 0;
	MEMWB_ptr->WB_reg = 0;
	MEMWB_ptr->ALU_result = 0;
}
