/**
 *
 * ECEN 4593 Computer Organization
 * Justin Barnitz and Jacob Levine
 *
 */



/* Probably a better scheme for the ALU OP
    if Instruction is R-format then the ALUOp will be based on the func
    otherwise:
    ALUOp   Action needed by ALU
    00      Addition (for load and store)
    01      Subtraction (for beq)
https://cs.nyu.edu/courses/fall01/V22.0436-001/lectures/lecture-16.html
*/
#include "MIPS_Processor.h"

int main()
{
	int clock_cycle = 0;
	/*----------  Init State Elements   ----------*/
	printf("Starting Processor\n");

	Flush_IF_ID(IFID_ptr);
	Flush_ID_EX(IDEX_ptr);
	Flush_EX_MEM(EXMEM_ptr);
	Flush_MEM_WB(MEMWB_ptr);

	init_memory();

	PC.pc = 0;
	//init_d_cache();

	/*----------  Execute MIPS program  ----------*/


	// Currently there is only 2 instr in memory
	while(PC.pc < 2)
	{
		IF();
		ID();
		EX();
		MEM();
		WB();
		Update();
		clock_cycle++;
	}

}

void IF()
{
	unsigned int rs_mask = 0x03E00000;
	unsigned int rt_mask = 0x1F0000;
	unsigned int rd_mask = 0xF800;
	unsigned int func_mask = 0x3F;
	short int imm_mask = 0xFFFF;
	unsigned int jump_mask = 0x3FFFFFF;
	unsigned int sham_mask = 0x7C0;

	unsigned int instr = memory[PC.pc];
	printf("instruction = 0x%08x\n", instr);

	IFID_SHADOW.OP_Code = (instr & OP_MASK) >> 26;
	IFID_SHADOW.reg_RS = (instr & rs_MASK) >> 21;
	IFID_SHADOW.reg_RT = (instr & rt_MASK) >> 16;
	IFID_SHADOW.reg_RD = (instr & rd_MASK) >> 11;
	IFID_SHADOW.sham = (instr & sham_MASK) >> 6;
	IFID_SHADOW.func = (instr & func_MASK);
	IFID_SHADOW.imm = (instr & imm_MASK);
	IFID_SHADOW.jmp_addr = (instr & jump_MASK);
	IFID_SHADOW.PC_Next = PC.pc + 1;

	printf("OP code = 0x%x\n", IFID_SHADOW.OP_Code);
	printf("RS = %d\n", IFID_SHADOW.reg_RS);
	printf("RT = %d\n", IFID_SHADOW.reg_RT);
	printf("RD = %d\n", IFID_SHADOW.reg_RD);
	printf("sham= %d\n", IFID_SHADOW.sham);
	printf("func = 0x%x\n", IFID_SHADOW.func);
	printf("imm = %d\n", IFID_SHADOW.imm);
	printf("jump addr = 0x%x\n", IFID_SHADOW.jmp_addr);

	// Program Counter Logic
	if(PC.pc_src)
	{
		// go to instruction from branch
		PC.pc = EXMEM.branch_target;
	}
	else
	{
		// increment the pc by 1 to next instruction
		PC.pc = PC.pc + 1;
	}
}

void ID()
{
	printf("In instruction Decode stage\n");
	switch(IFID.OP_Code)
	{
		//TODO: combine cases when it makes sense

		//R-format
		case 0x0:
			printf("Instruction was R-format\n");
			IDEX_SHADOW.Reg_Dst = 1;
			IDEX_SHADOW.Reg_Wrt = 1;
			IDEX_SHADOW.OP_Code = IFID.OP_Code;
			IDEX_SHADOW.ALU_Op = IFID.func;
			IDEX_SHADOW.ALU_Src = 0;
			IDEX_SHADOW.branch = 0;
			IDEX_SHADOW.Mem_Read = 0;
			IDEX_SHADOW.Mem_Wrt = 0;
			IDEX_SHADOW.Mem_to_Reg = 0;
			IDEX_SHADOW.Reg_RS_val = reg[IFID.reg_RS];
			IDEX_SHADOW.Reg_RT_val = reg[IFID.reg_RT];
			IDEX_SHADOW.reg_RS = IFID.reg_RS;
			IDEX_SHADOW.reg_RT = IFID.reg_RT;
			IDEX_SHADOW.reg_RD = IFID.reg_RD;
			IDEX_SHADOW.sign_ext_imm = 0;
			//TODO: PC = PC + 4;
			break;
		// branch and trap instructions based on rt value
		case 0x1:
			IDEX_SHADOW.ALU_Src;
			IDEX_SHADOW.ALU_Op;
			IDEX_SHADOW.Reg_Dst;
			IDEX_SHADOW.Reg_Wrt;
			IDEX_SHADOW.Mem_to_Reg;
			IDEX_SHADOW.Mem_Read;
			IDEX_SHADOW.Mem_Wrt;
			IDEX_SHADOW.Reg_RS_val;
			IDEX_SHADOW.Reg_RT_val;
			IDEX_SHADOW.reg_RS;
			IDEX_SHADOW.reg_RT;
			IDEX_SHADOW.reg_RD;
			IDEX_SHADOW.PC_Next;
			IDEX_SHADOW.branch;
			IDEX_SHADOW.sign_ext_imm;
			break;
		// j instruction
		case 0x2:
			IDEX_SHADOW.ALU_Src;
			IDEX_SHADOW.ALU_Op;
			IDEX_SHADOW.Reg_Dst;
			IDEX_SHADOW.Reg_Wrt;
			IDEX_SHADOW.Mem_to_Reg;
			IDEX_SHADOW.Mem_Read;
			IDEX_SHADOW.Mem_Wrt;
			IDEX_SHADOW.Reg_RS_val;
			IDEX_SHADOW.Reg_RT_val;
			IDEX_SHADOW.reg_RS;
			IDEX_SHADOW.reg_RT;
			IDEX_SHADOW.reg_RD;
			IDEX_SHADOW.PC_Next;
			IDEX_SHADOW.branch;
			IDEX_SHADOW.sign_ext_imm;
			break;
		// jal instruction
		case 0x3:
			IDEX_SHADOW.ALU_Src;
			IDEX_SHADOW.ALU_Op;
			IDEX_SHADOW.Reg_Dst;
			IDEX_SHADOW.Reg_Wrt;
			IDEX_SHADOW.Mem_to_Reg;
			IDEX_SHADOW.Mem_Read;
			IDEX_SHADOW.Mem_Wrt;
			IDEX_SHADOW.Reg_RS_val;
			IDEX_SHADOW.Reg_RT_val;
			IDEX_SHADOW.reg_RS;
			IDEX_SHADOW.reg_RT;
			IDEX_SHADOW.reg_RD;
			IDEX_SHADOW.PC_Next;
			IDEX_SHADOW.branch;
			IDEX_SHADOW.sign_ext_imm;
			break;
		// beq instruction
		case 0x4:
			IDEX_SHADOW.ALU_Src;
			IDEX_SHADOW.ALU_Op;
			IDEX_SHADOW.Reg_Dst;
			IDEX_SHADOW.Reg_Wrt;
			IDEX_SHADOW.Mem_to_Reg;
			IDEX_SHADOW.Mem_Read;
			IDEX_SHADOW.Mem_Wrt;
			IDEX_SHADOW.Reg_RS_val;
			IDEX_SHADOW.Reg_RT_val;
			IDEX_SHADOW.reg_RS;
			IDEX_SHADOW.reg_RT;
			IDEX_SHADOW.reg_RD;
			IDEX_SHADOW.PC_Next;
			IDEX_SHADOW.branch;
			IDEX_SHADOW.sign_ext_imm;
			break;
		// bne instruction
		case 0x5:
			IDEX_SHADOW.ALU_Src;
			IDEX_SHADOW.ALU_Op;
			IDEX_SHADOW.Reg_Dst;
			IDEX_SHADOW.Reg_Wrt;
			IDEX_SHADOW.Mem_to_Reg;
			IDEX_SHADOW.Mem_Read;
			IDEX_SHADOW.Mem_Wrt;
			IDEX_SHADOW.Reg_RS_val;
			IDEX_SHADOW.Reg_RT_val;
			IDEX_SHADOW.reg_RS;
			IDEX_SHADOW.reg_RT;
			IDEX_SHADOW.reg_RD;
			IDEX_SHADOW.PC_Next;
			IDEX_SHADOW.branch;
			IDEX_SHADOW.sign_ext_imm;
			break;
		// blez instruction
		case 0x6:
			break;
		// bgtz instruction
		case 0x7:
			break;
		// addi instruction
		case 0x8:
			break;
		// addiu instruction
		case 0x9:
			break;
		// slti instruction
		case 0xA:
			break;
		// sltiu instruction
		case 0xB:
			break;
		// andi instruction
		case 0xC:
			break;
		// ori instruction
		case 0xD:
			break;
		// xori instruction
		case 0xE:
			break;
		// lui instruction
		case 0xF:
			break;
		// beql instruction
		case 0x14:
			break;
		// bnel instruction
		case 0x15:
			break;
		// blezl instruction
		case 0x16:
			break;
		// bgtzl instruction
		case 0x17:
			break;
		// lb instruction
		case 0x20:
			break;
		// lh instruction
		case 0x21:
			break;
		// lwl instruction
		case 0x22:
			break;
		// lw instruction
		case 0x23:
			break;
		// lbu instruction
		case 0x24:
			break;
		// lhu instruction
		case 0x25:
			break;
		// lwr instruction
		case 0x26:
			break;
		// sb instruction
		case 0x28:
			break;
		// sh instruction
		case 0x29:
			break;
		// swl instruction
		case 0x2A:
			break;
		// sw instruction
		case 0x2B:
			break;
		// swr instruction
		case 0x2E:
			break;
	}
}

void EX()
{
	int ALU_A = IDEX.Reg_RS_val;
	int ALU_B;

	//RegDst Mux
	if(IDEX.Reg_Dst == 1)
	{
		EXMEM_SHADOW.WB_reg = IDEX.reg_RD;
	}
	else
	{
		EXMEM_SHADOW.WB_reg = IDEX.reg_RT;
	}

	//ALUSrc Mux
	if(IDEX.ALU_Src == 1)
	{
		ALU_B = IDEX.sign_ext_imm;
	}
	else
	{
		ALU_B = IDEX.Reg_RT_val;
	}

	// ALU Control
	/*
	if Instruction is R-format then the ALUOp will be based on the func
    otherwise:
    ALUOp   Action needed by ALU
    00      Addition (for load and store)
    01      Subtraction (for beq)
    */
	// Check to see if R-format
	if(IDEX.OP_Code == 0)
    {
        switch(IDEX.ALU_Op)
        {
            //this is based on the instruction func code

            //add instruction
            case(0x20):
                EXMEM_SHADOW.ALU_result = ALU_A + ALU_B;
                break;
        }
    }
    //I or J instruction
    else
    {
        switch(IDEX.ALU_Op)
        {
            case(0):
                EXMEM_SHADOW.ALU_result = ALU_A + ALU_B;
                break;
            case(1):
                EXMEM_SHADOW.ALU_result = ALU_A - ALU_B;
                break;
        }
    }



	EXMEM_SHADOW.branch = IDEX.branch;
	EXMEM_SHADOW.branch_target = IDEX.PC_Next + (IDEX.sign_ext_imm << 2);
	EXMEM_SHADOW.Reg_RT_val = IDEX.Reg_RT_val;
	EXMEM_SHADOW.PC_Next = IDEX.PC_Next;
	EXMEM_SHADOW.Reg_Wrt = IDEX.Reg_Wrt;
	EXMEM_SHADOW.Mem_Read = IDEX.Mem_Read;
	EXMEM_SHADOW.Mem_to_Reg = IDEX.Mem_to_Reg;
	EXMEM_SHADOW.Mem_Wrt = IDEX.Mem_Wrt;
	//TODO: When to set zero flag (branch instructions)
	EXMEM_SHADOW.zero = 0;

	//Handle Forwarding issues

}

void MEM()
{
	//write data to memory
	if(EXMEM.Mem_Wrt == 1)
	{
		// wtite to memory
		//memory[EXMEM.ALU_result] = EXMEM.Reg_RT_val
	}

	if(EXMEM.Mem_Read == 1)
	{
		//MEMWB_SHADOW.Data_Mem_result = memory[EXMEM.ALU_result];
	}
	MEMWB_SHADOW.Reg_Wrt = EXMEM.Reg_Wrt;
	MEMWB_SHADOW.WB_reg = EXMEM.WB_reg;
	MEMWB_SHADOW.ALU_result = EXMEM.ALU_result;
	MEMWB_SHADOW.Mem_to_Reg = EXMEM.Mem_to_Reg;
}

void WB()
{

	//do not write to register $zero
	if( (MEMWB.Reg_Wrt == 1) && (MEMWB.WB_reg != 0) )
	{
		if(MEMWB.Mem_to_Reg == 1)
		{
			reg[MEMWB.WB_reg] = MEMWB.ALU_result;
		}
		else
		{
			reg[MEMWB.WB_reg] = MEMWB.Data_Mem_result;
		}
	}
}

void Update()
{
	IFID = IFID_SHADOW;
	IDEX = IDEX_SHADOW;
	EXMEM = EXMEM_SHADOW;
	MEMWB = MEMWB_SHADOW;
}