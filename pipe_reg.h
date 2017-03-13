#ifndef PIPE_REG_H
#define PIPE_REG_H

// pipeline register for Instruction Decode / Instruction Fetch stage
typedef struct IF_ID_Reg
{
	unsigned int PC_Next;
	unsigned int reg_RS;
	unsigned int reg_RT;
	unsigned int reg_RD;
	unsigned int OP_Code;
	unsigned int sham;
	unsigned int func;
	unsigned int jmp_addr;
	short int imm;
}IF_ID;

// pipeline register for Instruction Decode / Execute stage
typedef struct ID_EX_Reg
{
	unsigned int ALU_Src;
	unsigned int ALU_Op;
	unsigned int Reg_Dst;
	unsigned int Reg_Wrt;
	unsigned int Mem_to_Reg;
	unsigned int Mem_Read;
	unsigned int Mem_Wrt;
	unsigned int Reg_RS_val;
	unsigned int Reg_RT_val;
	unsigned int reg_RS;
	unsigned int reg_RT;
	unsigned int reg_RD;
	unsigned int PC_Next;
	unsigned int branch;
	int sign_ext_imm;
} ID_EX;

// pipeline register for Execute / Memory stage
typedef struct EX_MEM_Reg
{
	unsigned int Reg_Wrt;
	unsigned int Mem_Read;
	unsigned int Mem_to_Reg;
	unsigned int Mem_Wrt;
	unsigned int zero;
	unsigned int Reg_RT_val;
	unsigned int WB_reg;
	unsigned int PC_Next;
	int ALU_result;
}EX_MEM;

// pipeline register for Memory / Write Back stage
typedef struct MEM_WB_Reg
{
	unsigned int Reg_Wrt;
	unsigned int Data_Mem_result;
	unsigned int WB_reg;
	int ALU_result;

}MEM_WB;

typedef struct PC_Register
{
	unsigned int pc;
}PC_Reg;

#endif