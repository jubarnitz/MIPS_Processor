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
	clock_cycle = 0;
	/*----------  Init State Elements   ----------*/
	printf("Starting Processor\n");

	Flush_IF_ID(IFID_ptr);
	Flush_ID_EX(IDEX_ptr);
	Flush_EX_MEM(EXMEM_ptr);
	Flush_MEM_WB(MEMWB_ptr);

	init_memory();
	init_reg();

	PC.pc = memory[5];
	//init_d_cache();

	/*----------  Execute MIPS program  ----------*/


	// When the pc counter goes to zero, the program has completed
	while(PC.pc != 0)
	{
		IF();
		WB();
		ID();
		EX();
		MEM();
		Update();
	}

}

void IF()
{
    // Branch Logic
	PC.pc_src = IDEX.branch;
	if(PC.pc_src)
	{
		// go to instruction from branch
		PC.pc = IDEX.branch_target;
	}

	unsigned int instr = memory[PC.pc];
	printf("In Instruction Fetch stage\n");
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

    /*
	printf("OP code = 0x%x\n", IFID_SHADOW.OP_Code);
	printf("RS = %d\n", IFID_SHADOW.reg_RS);
	printf("RT = %d\n", IFID_SHADOW.reg_RT);
	printf("RD = %d\n", IFID_SHADOW.reg_RD);
	printf("sham= %d\n", IFID_SHADOW.sham);
	printf("func = 0x%x\n", IFID_SHADOW.func);
	printf("imm = %d\n", IFID_SHADOW.imm);
	printf("jump addr = 0x%x\n", IFID_SHADOW.jmp_addr);
	*/


    // increment the pc by 1 to next instruction
	PC.pc = PC.pc + 1;
}

int ID()
{
	//TODO: write to dst reg
	printf("In instruction Decode stage\n");
	switch(IFID.OP_Code)
	{
		//TODO: combine cases when it makes sense

		//R-format
		case 0x0:
			//printf("Instruction was R-format\n");
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
			IDEX_SHADOW.PC_Next = IFID.PC_Next;
			IDEX_SHADOW.sham = IFID.sham;
			break;

		// branch and trap instructions based on rt value
		case 0x1:
            switch(IFID.reg_RT)
            {
                // bltz instruction
                case 0x0:
                    IDEX_SHADOW.Reg_Dst = 0;
                    IDEX_SHADOW.Reg_Wrt = 0;
                    IDEX_SHADOW.OP_Code = IFID.OP_Code;
                    IDEX_SHADOW.ALU_Op = 7;
                    IDEX_SHADOW.ALU_Src = 0;
                    IDEX_SHADOW.Mem_Read = 0;
                    IDEX_SHADOW.Mem_Wrt = 0;
                    IDEX_SHADOW.Mem_to_Reg = 0;
                    IDEX_SHADOW.Reg_RS_val = reg[IFID.reg_RS];
                    IDEX_SHADOW.Reg_RT_val = reg[IFID.reg_RT];
                    IDEX_SHADOW.reg_RS = IFID.reg_RS;
                    IDEX_SHADOW.reg_RT = IFID.reg_RT;
                    IDEX_SHADOW.reg_RD = IFID.reg_RD;
                    IDEX_SHADOW.sign_ext_imm = (int)IFID.imm;
                    IDEX_SHADOW.PC_Next = IFID.PC_Next;
                    if (IDEX_SHADOW.Reg_RS_val < 0)
                    {
                        IDEX_SHADOW.branch = 1;
                        IDEX_SHADOW.branch_target = IDEX.PC_Next + IDEX_SHADOW.sign_ext_imm;
                    }
                    else
                        IDEX_SHADOW.branch = 0;
                    break;
                default:
                    printf("Error: OpCode 1 instruction not implemented\n");
            }
            break;
		case 0x2: // j instruction
		case 0x3: // jal instruction
			IDEX_SHADOW.Reg_Dst = 0;
			IDEX_SHADOW.Reg_Wrt = (IFID.OP_Code == 0x2) ? 0 : 1;
			IDEX_SHADOW.OP_Code = IFID.OP_Code;
			IDEX_SHADOW.ALU_Op = 0;
			IDEX_SHADOW.ALU_Src = 0;
			IDEX_SHADOW.branch = 1;
			IDEX_SHADOW.Mem_Read = 0;
			IDEX_SHADOW.Mem_Wrt = 0;
			IDEX_SHADOW.Mem_to_Reg = 0;
			IDEX_SHADOW.Reg_RS_val = reg[IFID.reg_RS];
			IDEX_SHADOW.Reg_RT_val = reg[IFID.reg_RT];
			IDEX_SHADOW.reg_RS = IFID.reg_RS;
			IDEX_SHADOW.reg_RT = 31; // 31 is the $ra register (for jal)
			IDEX_SHADOW.reg_RD = IFID.reg_RD;
			IDEX_SHADOW.sign_ext_imm = IFID.imm;
			IDEX_SHADOW.branch_target =  (IDEX.PC_Next & 0xF0000000) | IFID.jmp_addr;
			IDEX_SHADOW.PC_Next = IFID.PC_Next;
			break;
		// beq instruction
		case 0x4:
			IDEX_SHADOW.Reg_Dst = 0;
			IDEX_SHADOW.Reg_Wrt = 0;
			IDEX_SHADOW.OP_Code = IFID.OP_Code;
			IDEX_SHADOW.ALU_Op = 1;
			IDEX_SHADOW.ALU_Src = 0;
			IDEX_SHADOW.Mem_Read = 0;
			IDEX_SHADOW.Mem_Wrt = 0;
			IDEX_SHADOW.Mem_to_Reg = 0;
			IDEX_SHADOW.Reg_RS_val = reg[IFID.reg_RS];
			IDEX_SHADOW.Reg_RT_val = reg[IFID.reg_RT];
			IDEX_SHADOW.reg_RS = IFID.reg_RS;
			IDEX_SHADOW.reg_RT = IFID.reg_RT;
			IDEX_SHADOW.reg_RD = IFID.reg_RD;
			IDEX_SHADOW.sign_ext_imm = (int)IFID.imm;
			IDEX_SHADOW.PC_Next = IFID.PC_Next;
            if (IDEX_SHADOW.Reg_RS_val == IDEX_SHADOW.Reg_RT_val)
            {
                IDEX_SHADOW.branch = 1;
                IDEX_SHADOW.branch_target = IDEX.PC_Next + IDEX_SHADOW.sign_ext_imm;
            }
            else
                IDEX_SHADOW.branch = 0;
			break;
		// bne instruction
		case 0x5:
			IDEX_SHADOW.Reg_Dst = 0;
			IDEX_SHADOW.Reg_Wrt = 0;
			IDEX_SHADOW.OP_Code = IFID.OP_Code;
			IDEX_SHADOW.ALU_Op = 4;
			IDEX_SHADOW.ALU_Src = 0;
			IDEX_SHADOW.Mem_Read = 0;
			IDEX_SHADOW.Mem_Wrt = 0;
			IDEX_SHADOW.Mem_to_Reg = 0;
			IDEX_SHADOW.Reg_RS_val = reg[IFID.reg_RS];
			IDEX_SHADOW.Reg_RT_val = reg[IFID.reg_RT];
			IDEX_SHADOW.reg_RS = IFID.reg_RS;
			IDEX_SHADOW.reg_RT = IFID.reg_RT;
			IDEX_SHADOW.reg_RD = IFID.reg_RD;
			IDEX_SHADOW.sign_ext_imm = (int)IFID.imm;
			IDEX_SHADOW.PC_Next = IFID.PC_Next;
            if (IDEX_SHADOW.Reg_RS_val != IDEX_SHADOW.Reg_RT_val)
            {
                IDEX_SHADOW.branch = 1;
                IDEX_SHADOW.branch_target = IDEX.PC_Next + IDEX_SHADOW.sign_ext_imm;
            }
            else
                IDEX_SHADOW.branch = 0;
			break;
		// blez instruction
		case 0x6:
            IDEX_SHADOW.Reg_Dst = 0;
            IDEX_SHADOW.Reg_Wrt = 0;
            IDEX_SHADOW.OP_Code = IFID.OP_Code;
            IDEX_SHADOW.ALU_Op = 7;
            IDEX_SHADOW.ALU_Src = 0;
            IDEX_SHADOW.Mem_Read = 0;
            IDEX_SHADOW.Mem_Wrt = 0;
            IDEX_SHADOW.Mem_to_Reg = 0;
            IDEX_SHADOW.Reg_RS_val = reg[IFID.reg_RS];
            IDEX_SHADOW.Reg_RT_val = reg[IFID.reg_RT];
            IDEX_SHADOW.reg_RS = IFID.reg_RS;
            IDEX_SHADOW.reg_RT = IFID.reg_RT;
            IDEX_SHADOW.reg_RD = IFID.reg_RD;
            IDEX_SHADOW.sign_ext_imm = (int)IFID.imm;
            IDEX_SHADOW.PC_Next = IFID.PC_Next;
            if (IDEX_SHADOW.Reg_RS_val <= 0)
            {
                IDEX_SHADOW.branch = 1;
                IDEX_SHADOW.branch_target = IDEX.PC_Next + IDEX_SHADOW.sign_ext_imm;
            }
            else
                IDEX_SHADOW.branch = 0;
			break;
		// bgtz instruction
		case 0x7:
            IDEX_SHADOW.Reg_Dst = 0;
            IDEX_SHADOW.Reg_Wrt = 0;
            IDEX_SHADOW.OP_Code = IFID.OP_Code;
            IDEX_SHADOW.ALU_Op = 0;
            IDEX_SHADOW.ALU_Src = 0;
            IDEX_SHADOW.Mem_Read = 0;
            IDEX_SHADOW.Mem_Wrt = 0;
            IDEX_SHADOW.Mem_to_Reg = 0;
            IDEX_SHADOW.Reg_RS_val = reg[IFID.reg_RS];
            IDEX_SHADOW.Reg_RT_val = reg[IFID.reg_RT];
            IDEX_SHADOW.reg_RS = IFID.reg_RS;
            IDEX_SHADOW.reg_RT = IFID.reg_RT;
            IDEX_SHADOW.reg_RD = IFID.reg_RD;
            IDEX_SHADOW.sign_ext_imm = (int)IFID.imm;
            IDEX_SHADOW.PC_Next = IFID.PC_Next;
            if (IDEX_SHADOW.Reg_RS_val > 0)
            {
                IDEX_SHADOW.branch = 1;
                IDEX_SHADOW.branch_target = IDEX.PC_Next + IDEX_SHADOW.sign_ext_imm;
            }
            else
                IDEX_SHADOW.branch = 0;
			break;
		case 0x8: // addi instruction
		case 0x9: // addiu instruction
            IDEX_SHADOW.ALU_Src = 1;
			IDEX_SHADOW.ALU_Op = 0;
			IDEX_SHADOW.Reg_Dst = 0;
			IDEX_SHADOW.Reg_Wrt = 1;
			IDEX_SHADOW.Mem_to_Reg = 0;
			IDEX_SHADOW.Mem_Read = 0;
			IDEX_SHADOW.Mem_Wrt = 0;
			IDEX_SHADOW.Reg_RS_val = reg[IFID.reg_RS];
			IDEX_SHADOW.Reg_RT_val = reg[IFID.reg_RT];
			IDEX_SHADOW.reg_RS = IFID.reg_RS;
			IDEX_SHADOW.reg_RT = IFID.reg_RT;
			IDEX_SHADOW.reg_RD = IFID.reg_RD;
			IDEX_SHADOW.PC_Next = IFID.PC_Next;
			IDEX_SHADOW.branch = 0;
			if(IFID.OP_Code == 0x8)
			{
			    IDEX_SHADOW.sign_ext_imm = (int)IFID.imm;
			    IDEX_SHADOW.OP_Code = 0x8;
			}
			else if(IFID.OP_Code == 0x9)
			{
			    IDEX_SHADOW.sign_ext_imm = (unsigned int)IFID.imm;
			    IDEX_SHADOW.OP_Code = 0x9;
			}
			break;
		case 0xA: // slti instruction
		case 0xB: // sltiu instruction
			IDEX_SHADOW.Reg_Dst = 0;
			IDEX_SHADOW.Reg_Wrt = 1;
			IDEX_SHADOW.ALU_Src = 1;
			IDEX_SHADOW.branch = 0;
			IDEX_SHADOW.Mem_Read = 0;
			IDEX_SHADOW.Mem_Wrt = 0;
			IDEX_SHADOW.Mem_to_Reg = 0;
			IDEX_SHADOW.Reg_RS_val = reg[IFID.reg_RS];
			IDEX_SHADOW.Reg_RT_val = reg[IFID.reg_RT];
			IDEX_SHADOW.reg_RS = IFID.reg_RS;
			IDEX_SHADOW.reg_RT = IFID.reg_RT;
			IDEX_SHADOW.reg_RD = IFID.reg_RD;
			IDEX_SHADOW.PC_Next = IFID.PC_Next;
			if(IFID.OP_Code == 0xA)
			{
				IDEX_SHADOW.OP_Code = 0; // set this to 0 so we can reuse the R-format slt
                IDEX_SHADOW.ALU_Op = 0x2A; // R-format slt
                IDEX_SHADOW.sign_ext_imm = (int)IFID.imm;
		}
			else if(IFID.OP_Code == 0xB)
			{
				IDEX_SHADOW.OP_Code = 0; // set this to 0 so we can reuse the R-format sltu
                IDEX_SHADOW.ALU_Op = 0x2B; // R-format sltu
                IDEX_SHADOW.sign_ext_imm = (unsigned int)IFID.imm;
		}
			break;
		case 0xC: // andi instruction
		    IDEX_SHADOW.ALU_Src = 1;
		    IDEX_SHADOW.ALU_Op = 2;
		    IDEX_SHADOW.Reg_Dst = 0;
		    IDEX_SHADOW.Reg_Wrt = 1;
		    IDEX_SHADOW.Mem_to_Reg = 0;
		    IDEX_SHADOW.Mem_Read = 0;
		    IDEX_SHADOW.Mem_Wrt = 0;
		    IDEX_SHADOW.Reg_RS_val = reg[IFID.reg_RS];
		    IDEX_SHADOW.Reg_RT_val = reg[IFID.reg_RT];
		    IDEX_SHADOW.reg_RS = IFID.reg_RS;
			IDEX_SHADOW.reg_RT = IFID.reg_RT;
			IDEX_SHADOW.reg_RD = IFID.reg_RD;
			IDEX_SHADOW.PC_Next = IFID.PC_Next;
			IDEX_SHADOW.branch = 0;
			IDEX_SHADOW.sign_ext_imm = (int)IFID.imm;
			IDEX_SHADOW.OP_Code = 0xC;
			break;
		case 0xD: // ori instruction
		    IDEX_SHADOW.ALU_Src = 1;
		    IDEX_SHADOW.ALU_Op = 3;
		    IDEX_SHADOW.Reg_Dst = 0;
		    IDEX_SHADOW.Reg_Wrt = 1;
		    IDEX_SHADOW.Mem_to_Reg = 0;
		    IDEX_SHADOW.Mem_Read = 0;
		    IDEX_SHADOW.Mem_Wrt = 0;
		    IDEX_SHADOW.Reg_RS_val = reg[IFID.reg_RS];
		    IDEX_SHADOW.Reg_RT_val = reg[IFID.reg_RT];
		    IDEX_SHADOW.reg_RS = IFID.reg_RS;
			IDEX_SHADOW.reg_RT = IFID.reg_RT;
			IDEX_SHADOW.reg_RD = IFID.reg_RD;
			IDEX_SHADOW.PC_Next = IFID.PC_Next;
			IDEX_SHADOW.branch = 0;
			IDEX_SHADOW.sign_ext_imm = (int)IFID.imm;
			IDEX_SHADOW.OP_Code = 0xD;
			break;
		case 0xE: // xori instruction
		    IDEX_SHADOW.ALU_Src = 1;
		    IDEX_SHADOW.ALU_Op = 4;
		    IDEX_SHADOW.Reg_Dst = 0;
		    IDEX_SHADOW.Reg_Wrt = 1;
		    IDEX_SHADOW.Mem_to_Reg = 0;
		    IDEX_SHADOW.Mem_Read = 0;
		    IDEX_SHADOW.Mem_Wrt = 0;
		    IDEX_SHADOW.Reg_RS_val = reg[IFID.reg_RS];
		    IDEX_SHADOW.Reg_RT_val = reg[IFID.reg_RT];
		    IDEX_SHADOW.reg_RS = IFID.reg_RS;
			IDEX_SHADOW.reg_RT = IFID.reg_RT;
			IDEX_SHADOW.reg_RD = IFID.reg_RD;
			IDEX_SHADOW.PC_Next = IFID.PC_Next;
			IDEX_SHADOW.branch = 0;
			IDEX_SHADOW.sign_ext_imm = (int)IFID.imm;
			IDEX_SHADOW.OP_Code = 0xE;
			break;
		// lui instruction -- I need to double check this one - Jake
		case 0xF:
		    IDEX_SHADOW.ALU_Src = 1;
		    IDEX_SHADOW.ALU_Op = 2;
		    IDEX_SHADOW.Reg_Dst = 0;
		    IDEX_SHADOW.Reg_Wrt = 1;
		    IDEX_SHADOW.Mem_to_Reg = 0;
		    IDEX_SHADOW.Mem_Read = 0;
		    IDEX_SHADOW.Mem_Wrt = 0;
		    IDEX_SHADOW.Reg_RS_val = reg[IFID.reg_RS];
		    IDEX_SHADOW.Reg_RT_val = reg[IFID.reg_RT];
		    IDEX_SHADOW.reg_RS = IFID.reg_RS;
			IDEX_SHADOW.reg_RT = IFID.reg_RT;
			IDEX_SHADOW.reg_RD = IFID.reg_RD;
			IDEX_SHADOW.PC_Next = IFID.PC_Next;
			IDEX_SHADOW.branch = 0;
			IDEX_SHADOW.sign_ext_imm = (int)IFID.imm;
			IDEX_SHADOW.OP_Code = 0xF;
			break;
		// lw instruction
		case 0x23:
		    IDEX_SHADOW.ALU_Src = 1;
		    IDEX_SHADOW.ALU_Op = 0;
		    IDEX_SHADOW.Reg_Dst = 0;
		    IDEX_SHADOW.Reg_Wrt = 1;
		    IDEX_SHADOW.Mem_to_Reg = 1;
		    IDEX_SHADOW.Mem_Read = 1;
		    IDEX_SHADOW.Mem_Wrt = 0;
		    IDEX_SHADOW.Reg_RS_val = reg[IFID.reg_RS];
		    IDEX_SHADOW.Reg_RT_val = reg[IFID.reg_RT];
		    IDEX_SHADOW.reg_RS = IFID.reg_RS;
			IDEX_SHADOW.reg_RT = IFID.reg_RT;
			IDEX_SHADOW.reg_RD = IFID.reg_RD;
			IDEX_SHADOW.PC_Next = IFID.PC_Next;
			IDEX_SHADOW.branch = 0;
			IDEX_SHADOW.sign_ext_imm = (int)IFID.imm;
			IDEX_SHADOW.OP_Code = 0x23;
			IDEX_SHADOW.sham = IFID.sham;
			break;
		// lbu instruction
		case 0x24:
			break;
		// lhu instruction
		case 0x25:
			break;
		// sb instruction
		case 0x28:
			break;
		// sh instruction
		case 0x29:
			break;
		// sw instruction
		case 0x2B:
			break;
        default:
            printf("Unknown Instruction: OP Code = 0x%2x and Instruction address = %d\n", IFID.OP_Code, (IFID.PC_Next-1) );
            return -1;

	}
	return 0;
}

void EX()
{
	int ALU_A = IDEX.Reg_RS_val;
	int ALU_B;

	printf("In Execute stage\n");

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
    000      Addition (for load and store)
    001      Subtraction (for beq)
    010      And
    011      Or
    100      XOR
    111      Less than
    */
	// Check to see if R-format
	if(IDEX.OP_Code == 0)
    {
        switch(IDEX.ALU_Op)
        {
            //this is based on the instruction func code
            case(0x0): // shift left logical
                EXMEM_SHADOW.ALU_result = ALU_B << IDEX.sham;
                break;
            case(0x2): // shift right logical
                EXMEM_SHADOW.ALU_result = (unsigned int)ALU_B >> IDEX.sham;
                break;
            case(0x3): // shift right arithmetic
                EXMEM_SHADOW.ALU_result = (int)ALU_B >> IDEX.sham;
                break;
            case(0x4): // shift left logical variable
                EXMEM_SHADOW.ALU_result = ALU_B << ALU_A;
                break;
            case(0x6): // shift right logical variable
                EXMEM_SHADOW.ALU_result = (unsigned int)ALU_B >> ALU_A;
                break;
            case(0x7): // shift right arithmetic variable
                EXMEM_SHADOW.ALU_result = (int)ALU_B >> ALU_A;
                break;
            case(0x20): //add instruction
                EXMEM_SHADOW.ALU_result = (int)ALU_A + (int)ALU_B;
                break;
            case(0x21): //addu instruction
                EXMEM_SHADOW.ALU_result = ALU_A + ALU_B;
                break;
            case(0x22): // subtract instruction
                EXMEM_SHADOW.ALU_result = (int)ALU_A - (int)ALU_B;
                break;
            case(0x23): // subu
                EXMEM_SHADOW.ALU_result = ALU_A - ALU_B;
            case(0x24): //and instruction
                EXMEM_SHADOW.ALU_result = ALU_A & ALU_B;
                break;
            case(0x25): // or instruction
                EXMEM_SHADOW.ALU_result = ALU_A | ALU_B;
                break;
            case(0x26): // xor instruction
                EXMEM_SHADOW.ALU_result = ALU_A ^ ALU_B;
                break;
            case(0x27): //nor instruction
                EXMEM_SHADOW.ALU_result = ~(ALU_A | ALU_B);
                break;
            case(0x2A): //slt instruction
                EXMEM_SHADOW.ALU_result = ( (signed int)ALU_A < (signed int)ALU_B );
                break;
            case(0x2B): //sltu instruction
                EXMEM_SHADOW.ALU_result = (ALU_A < ALU_B);
                break;

        }
    }
    //J instruction
    else if (IDEX.OP_Code == 2 || IDEX.OP_Code == 3)
    {
        EXMEM_SHADOW.branch = IDEX.branch;
        EXMEM_SHADOW.branch_target = (IDEX.PC_Next & 0xF0000000) | (IDEX.sign_ext_imm << 2);
        EXMEM_SHADOW.Reg_RT_val = IDEX.Reg_RT_val;
        EXMEM_SHADOW.PC_Next = IDEX.PC_Next;
        EXMEM_SHADOW.ALU_result = IDEX.PC_Next; // so we can save the addr of the next inst in the WB stage
        EXMEM_SHADOW.Reg_Wrt = IDEX.Reg_Wrt;
        EXMEM_SHADOW.Mem_Read = IDEX.Mem_Read;
        EXMEM_SHADOW.Mem_to_Reg = IDEX.Mem_to_Reg;
        EXMEM_SHADOW.Mem_Wrt = IDEX.Mem_Wrt;

    }
    // lbu, lhu lw, sb, sh, and sw instruction
    else if(IDEX.OP_Code == 0x23 || IDEX.OP_Code == 0x24 || IDEX.OP_Code == 0x28
            ||  IDEX.OP_Code == 0x2B || IDEX.OP_Code == 0x29 || IDEX.OP_Code == 0x25)
    {
        // because mips is byte addressable and our memory is modeled
        // as words.
        // Ex lw $t0, 32($s0) => we want to index a[32/4] => a[8]
        // Ex lb $to, 33($s0) => we want a[33/]4] => a[8] and byte (33 % 4) = byte 1
        // Ex lhu $t0, 2($t0) => we want a[2/4] => a[0] and half word (2 >> 1) % 2 = halfword 1 of word 0
        // ex lhu $t0, 6($t0) => a[6/4] => a[1] and halfword is (6 / 2) = 3 % 2 = halfword 1 of word 1
        EXMEM_SHADOW.ALU_result = ALU_A + (ALU_B >> 2);
        EXMEM_SHADOW.which_byte = ALU_B % 4;
        EXMEM_SHADOW.which_half = (ALU_B >> 1) % 2;
    }
    //I instruction
    else
    {
        switch(IDEX.ALU_Op)
        {
            case(0): //add
                EXMEM_SHADOW.ALU_result = ALU_A + ALU_B;
                break;
            case(1): //subract
                EXMEM_SHADOW.ALU_result = ALU_A - ALU_B;
                break;
            case(2): // bitwise and
                EXMEM_SHADOW.ALU_result = ALU_A & ALU_B;
                break;
            case(3): // bitwise or
                EXMEM_SHADOW.ALU_result = ALU_A | ALU_B;
                break;
            case(4): // bitwise xor
                EXMEM_SHADOW.ALU_result = ALU_A ^ ALU_B;
                break;
            case(7): // set on less than
                EXMEM_SHADOW.ALU_result = ALU_A < ALU_B;
                break;
        }
    }



	EXMEM_SHADOW.branch = IDEX.branch;
	EXMEM_SHADOW.branch_target = IDEX.PC_Next + IDEX.sign_ext_imm;
	EXMEM_SHADOW.Reg_RT_val = IDEX.Reg_RT_val;
	EXMEM_SHADOW.PC_Next = IDEX.PC_Next;
	EXMEM_SHADOW.Reg_Wrt = IDEX.Reg_Wrt;
	EXMEM_SHADOW.Mem_Read = IDEX.Mem_Read;
	EXMEM_SHADOW.Mem_to_Reg = IDEX.Mem_to_Reg;
	EXMEM_SHADOW.Mem_Wrt = IDEX.Mem_Wrt;
	EXMEM_SHADOW.OP_Code = IDEX.OP_Code;

	//Handle Forwarding issues

}

int MEM()
{
	printf("In Memory stage\n");
	int word = 0;

	//write data to memory
	if(EXMEM.Mem_Wrt == 1)
	{
		// wtite to memory
		//memory[EXMEM.ALU_result] = EXMEM.Reg_RT_val
	}

	if(EXMEM.Mem_Read == 1)
	{
		word = memory[EXMEM.ALU_result];
		// lw instruction
		if(EXMEM.OP_Code == 0x23)
        {
            MEMWB_SHADOW.Data_Mem_result = word;
        }
        // lhu instruction
        else if(EXMEM.OP_Code == 0x25)
        {
            if(EXMEM.which_half == 0)
            {
                MEMWB_SHADOW.Data_Mem_result = (word & BIG_END_HALFWORD_0) >> 16;
            }
            else if(EXMEM.which_half == 1)
            {
                MEMWB_SHADOW.Data_Mem_result = (word & BIG_END_HALFWORD_1);
            }
            else
            {
                printf("Error: In lhu instruction, Unknown half word value!\n");
                return -1;
            }
        }
        // lbu instruction
        else if(EXMEM.OP_Code == 0x24)
        {
            if(EXMEM.which_byte == 0)
            {
                MEMWB_SHADOW.Data_Mem_result = (word & BIG_END_BYTE_0) >> 24;
            }
            else if(EXMEM.which_byte == 1)
            {
                MEMWB_SHADOW.Data_Mem_result = (word & BIG_END_BYTE_1) >> 16;
            }
            else if(EXMEM.which_byte == 2)
            {
                MEMWB_SHADOW.Data_Mem_result = (word & BIG_END_BYTE_2) >> 8;
            }
            else if(EXMEM.which_byte == 3)
            {
                MEMWB_SHADOW.Data_Mem_result = word & BIG_END_BYTE_3;
            }
            else //Unknown which byte value
            {
                printf("Error: In lbu instruction, Unknown byte value!\n");
                return -1;
            }
        }

	}
	MEMWB_SHADOW.Reg_Wrt = EXMEM.Reg_Wrt;
	MEMWB_SHADOW.WB_reg = EXMEM.WB_reg;
	MEMWB_SHADOW.ALU_result = EXMEM.ALU_result;
	MEMWB_SHADOW.Mem_to_Reg = EXMEM.Mem_to_Reg;
	return 0;
}

void WB()
{
    printf("In Write Back stage\n");

	//do not write to register $zero
	if( (MEMWB.Reg_Wrt == 1) && (MEMWB.WB_reg != 0) )
	{
		if(MEMWB.Mem_to_Reg == 1)
		{
			reg[MEMWB.WB_reg] = MEMWB.Data_Mem_result;
		}
		else
		{
			reg[MEMWB.WB_reg] = MEMWB.ALU_result;
		}
	}
}

void Update()
{
	IFID = IFID_SHADOW;
	IDEX = IDEX_SHADOW;
	EXMEM = EXMEM_SHADOW;
	MEMWB = MEMWB_SHADOW;
	clock_cycle++;
	printf("Clock Cycle: %d\n", clock_cycle);
}
