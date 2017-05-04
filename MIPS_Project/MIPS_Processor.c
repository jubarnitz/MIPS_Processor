/**
 *
 * ECEN 4593 Computer Organization
 * Justin Barnitz and Jacob Levine
 *
 */

#include "MIPS_Processor.h"

int main()
{

	clock_cycle = 0;
	stall_pipe = 0;
	data_hazard = 0;
	reset_mem_penalty_count = 0;

	/*----------  Init State Elements   ----------*/
	printf("Starting Processor\n");

	Flush_IF_ID(IFID_ptr);
	Flush_ID_EX(IDEX_ptr);
	Flush_EX_MEM(EXMEM_ptr);
	Flush_MEM_WB(MEMWB_ptr);

	init_memory();
	init_reg();

	PC.pc = memory[5];
	init_i_cache();
	init_d_cache();

	/*----------  Execute MIPS program  ----------*/


	// When the pc counter goes to zero, the program has completed
	while(PC.pc > 1)
	{
		IF();
		WB();
		ID();
		EX();
		MEM();
		Update();
// clock cycle == 1772 finished copy_array
// clock cycle ==  1810 start bubble sort
// clock cycle == 554622
//		if(clock_cycle >= 466)
//        { printf("stop!!!"); }

	}
	// for Program 1, prints in decimal
	//printf("memory[6] = %d\n", memory[6]);
	//printf("memory[7] = %d\n", memory[7]);
	//printf("memory[8] = %d\n", memory[8]);
	//printf("memory[9] = %d\n", memory[9]);

    // for Program 2, prints 7, 8, & 9 in hex
	printf("memory[6] = %d\n", memory[6]);
	printf("memory[7] = %#010x\n", memory[7]);
	printf("memory[8] = %#010x\n", memory[8]);
	printf("memory[9] = %#010x\n", memory[9]);


}

void IF()
{
    int data_valid;
    unsigned int instr = 0;
    printf("In Instruction Fetch stage\n");
    // Branch Logic
	PC.pc_src = IDEX.branch;
	if(PC.pc_src)
	{
		// go to instruction from branch
		PC.pc = IDEX.branch_target;
	}
    printf("PC.pc = %d\n", PC.pc);

    if (ICACHE_ON)
    {
        data_valid = icache_access(PC.pc, &instr);
        if(!data_valid)
        {
            printf("\nStalll\n");
            stall_pipe = 1;
        }
    }
    else
    {
        instr = memory[PC.pc];
    }
	//instr = memory[PC.pc];

	printf("\ninstruction = 0x%08x\n", instr);

//	if (instr != 0)
//	{ printf("new instruction\n"); }

    if(clock_cycle == 258)
    {
        printf("Hold up\n");
    }
    if(instr == 0x80a70000)
    { printf("start function\n"); }

    if(instr == 0x8c050008)
    { printf("lw\n"); }

    if(instr == 65011720)
    { printf("jr instruction\n"); }

    if(instr == 0x18c00008)
    { printf("\n\nStart 0f copy function\n\n"); }

    if(instr == 0x1ca00018)
    { printf("\n\nStart 0f bubble function\n\n"); }

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
	//TODO: add data hazard detection for jr instr
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
			IDEX_SHADOW.I_format = 0;
			// jr instruction
			if(IFID.func == 0x8)
            {
                IDEX_SHADOW.branch = 1;
                IDEX_SHADOW.branch_target = IDEX_SHADOW.Reg_RS_val;
                printf("ID(): jr instruction, jumping to : %d\n", IDEX_SHADOW.branch_target);
            }
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
                    IDEX_SHADOW.I_format = 1;
                    // Branch forward logic
                    if( (EXMEM.Reg_Wrt) && (EXMEM.WB_reg == IFID.reg_RS) && !(EXMEM.Mem_Read) )
                    {
                        IDEX_SHADOW.Reg_RS_val = EXMEM.ALU_result;
                        printf("ID(): bltz => Forwarding data from EXMEM, new result = %d\n", IDEX_SHADOW.Reg_RS_val);
                    }

                    if (IDEX_SHADOW.Reg_RS_val < 0)
                    {
                        IDEX_SHADOW.branch = 1;
                        IDEX_SHADOW.branch_target = IFID.PC_Next + IDEX_SHADOW.sign_ext_imm;
                    }
                    else
                        IDEX_SHADOW.branch = 0;
                    break;
                default:
                    printf("Error: OpCode 1 instruction not implemented.  IFID.reg_RT = %d\n", IFID.reg_RT);
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
			IDEX_SHADOW.I_format = 0;
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
			IDEX_SHADOW.I_format = 1;
            // Branch forward logic
            if( (EXMEM.Reg_Wrt) && (EXMEM.WB_reg == IFID.reg_RS) && !(EXMEM.Mem_Read) )
            {
                IDEX_SHADOW.Reg_RS_val = EXMEM.ALU_result;
                printf("ID(): beq => Forwarding RS data from EXMEM, new result = %d\n", IDEX_SHADOW.Reg_RS_val);
            }
            if( (EXMEM.Reg_Wrt) && (EXMEM.WB_reg == IFID.reg_RT) && !(EXMEM.Mem_Read) )
            {
                IDEX_SHADOW.Reg_RT_val = EXMEM.ALU_result;
                printf("ID(): beq => Forwarding RT data from EXMEM, new result = %d\n", IDEX_SHADOW.Reg_RT_val);
            }

            if (IDEX_SHADOW.Reg_RS_val == IDEX_SHADOW.Reg_RT_val)
            {
                IDEX_SHADOW.branch = 1;
                IDEX_SHADOW.branch_target = IFID.PC_Next + IDEX_SHADOW.sign_ext_imm;
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
			IDEX_SHADOW.I_format = 1;
           // Branch forward logic
            if( (EXMEM.Reg_Wrt) && (EXMEM.WB_reg == IFID.reg_RS) && !(EXMEM.Mem_Read) )
            {
                IDEX_SHADOW.Reg_RS_val = EXMEM.ALU_result;
                printf("ID(): bne => Forwarding RS data from EXMEM, new result = %d\n", IDEX_SHADOW.Reg_RS_val);
            }
            if( (EXMEM.Reg_Wrt) && (EXMEM.WB_reg == IFID.reg_RT) && !(EXMEM.Mem_Read) )
            {
                IDEX_SHADOW.Reg_RT_val = EXMEM.ALU_result;
                printf("ID(): bne => Forwarding RT data from EXMEM, new result = %d\n", IDEX_SHADOW.Reg_RT_val);
            }

            if (IDEX_SHADOW.Reg_RS_val != IDEX_SHADOW.Reg_RT_val)
            {
                IDEX_SHADOW.branch = 1;
                IDEX_SHADOW.branch_target = IFID.PC_Next + IDEX_SHADOW.sign_ext_imm;
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
            IDEX_SHADOW.I_format = 1;
            // Branch forward logic
            if( (EXMEM.Reg_Wrt) && (EXMEM.WB_reg == IFID.reg_RS) && !(EXMEM.Mem_Read) )
            {
                IDEX_SHADOW.Reg_RS_val = EXMEM.ALU_result;
                printf("ID(): blez => Forwarding RS data from EXMEM, new result = %d\n", IDEX_SHADOW.Reg_RS_val);
            }

            if (IDEX_SHADOW.Reg_RS_val <= 0)
            {
                IDEX_SHADOW.branch = 1;
                IDEX_SHADOW.branch_target = IFID.PC_Next + IDEX_SHADOW.sign_ext_imm;
            }
            else
                IDEX_SHADOW.branch = 0;
			break;
		// bgtz instruction
		case 0x7:
            printf("bgtz   rs > 0\n");
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
            //printf(" RS value = %d\n", IDEX_SHADOW.Reg_RS_val);
            //printf("IFID.imm = %d\n", IFID.imm);
            IDEX_SHADOW.sign_ext_imm = (int)IFID.imm;
            //printf("IDEX_SHADOW.sign_ext_imm = %d\n", IDEX_SHADOW.sign_ext_imm);
            IDEX_SHADOW.PC_Next = IFID.PC_Next;
            IDEX_SHADOW.I_format = 1;
            // Branch forward logic
            if( (EXMEM.Reg_Wrt) && (EXMEM.WB_reg == IFID.reg_RS) && !(EXMEM.Mem_Read) )
            {
                IDEX_SHADOW.Reg_RS_val = EXMEM.ALU_result;
                printf("ID(): bgtz => Forwarding RS data from EXMEM, new result = %d\n", IDEX_SHADOW.Reg_RS_val);
            }

            if (IDEX_SHADOW.Reg_RS_val > 0)
            {

                IDEX_SHADOW.branch = 1;
                printf("IFID.PC_Next = %d\n", IFID.PC_Next);
                IDEX_SHADOW.branch_target = IFID.PC_Next + IDEX_SHADOW.sign_ext_imm;
                printf("bgtz => we are jumping to : %d\n", IDEX_SHADOW.branch_target);
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
			IDEX_SHADOW.I_format = 1;
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
			IDEX_SHADOW.I_format = 1;
			if(IFID.OP_Code == 0xA)
			{
				IDEX_SHADOW.OP_Code = 0; // set this to 0 so we can reuse the R-format slt
                IDEX_SHADOW.ALU_Op = 0x2A; // R-format slt
                IDEX_SHADOW.sign_ext_imm = (int)IFID.imm;
                IDEX_SHADOW.Reg_RT_val = IDEX_SHADOW.sign_ext_imm;
            }
			else if(IFID.OP_Code == 0xB)
			{
				IDEX_SHADOW.OP_Code = 0; // set this to 0 so we can reuse the R-format sltu
                IDEX_SHADOW.ALU_Op = 0x2B; // R-format sltu
                IDEX_SHADOW.sign_ext_imm = (unsigned int)IFID.imm;
                IDEX_SHADOW.Reg_RT_val = IDEX_SHADOW.sign_ext_imm;
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
			IDEX_SHADOW.I_format = 1;
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
			IDEX_SHADOW.I_format = 1;
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
			IDEX_SHADOW.I_format = 1;
			break;
		// lui instruction
		case 0xF:
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
			IDEX_SHADOW.sign_ext_imm = (int)IFID.imm << 16;
			IDEX_SHADOW.OP_Code = IFID.OP_Code;
			IDEX_SHADOW.I_format = 1;
			break;
        // seb instruction
        case 0x1F:
            IDEX_SHADOW.ALU_Src = 0;
		    IDEX_SHADOW.ALU_Op = 0;
		    IDEX_SHADOW.Reg_Dst = 1;
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
			IDEX_SHADOW.OP_Code = IFID.OP_Code;
			IDEX_SHADOW.I_format = 0;
			break;
        case 0x20: // lb instruction
		case 0x23: // lw instruction
		case 0x24: // lbu instruction
		case 0x25: // lhu instruction
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
			IDEX_SHADOW.OP_Code = IFID.OP_Code;
			IDEX_SHADOW.sham = IFID.sham;
			IDEX_SHADOW.I_format = 1;
			break;
		case 0x28: // sb instruction
		case 0x29: // sh instruction
		case 0x2B: // sw instruction
		    IDEX_SHADOW.ALU_Src = 1;
		    IDEX_SHADOW.ALU_Op = 0;
		    IDEX_SHADOW.Reg_Dst = 0;
		    IDEX_SHADOW.Reg_Wrt = 0;
		    IDEX_SHADOW.Mem_to_Reg = 0;
		    IDEX_SHADOW.Mem_Read = 0;
		    IDEX_SHADOW.Mem_Wrt = 1;
		    IDEX_SHADOW.Reg_RS_val = reg[IFID.reg_RS];
		    IDEX_SHADOW.Reg_RT_val = reg[IFID.reg_RT];
		    IDEX_SHADOW.reg_RS = IFID.reg_RS;
			IDEX_SHADOW.reg_RT = IFID.reg_RT;
			IDEX_SHADOW.reg_RD = IFID.reg_RD;
			IDEX_SHADOW.PC_Next = IFID.PC_Next;
			IDEX_SHADOW.branch = 0;
			IDEX_SHADOW.sign_ext_imm = (int)IFID.imm;
			IDEX_SHADOW.OP_Code = IFID.OP_Code;
			IDEX_SHADOW.sham = IFID.sham;
			IDEX_SHADOW.I_format = 1;
			break;
        default:
            printf("Unknown Instruction: OP Code = 0x%2x and Instruction address = %d\n", IFID.OP_Code, (IFID.PC_Next-1) );
            exit(-1);

	}
	//determine Write Register for data hazard detection
	if(IDEX_SHADOW.Reg_Dst == 1)
	{
		IDEX_SHADOW.WB_reg = IDEX_SHADOW.reg_RD;
		printf("ID(): WB_reg = RD = %d\n", IDEX_SHADOW.WB_reg);
	}
	else
	{
		IDEX_SHADOW.WB_reg = IDEX_SHADOW.reg_RT;
		printf("ID(): WB_reg = RT = %d\n", IDEX_SHADOW.WB_reg);
	}

	//Hazard Detection Unit

    // if the instruction following a load is accessing the write reg.
    if( IDEX_SHADOW.Mem_Read &&
       ( (IDEX_SHADOW.reg_RT == IFID_SHADOW.reg_RS)
        || (IDEX_SHADOW.reg_RT == IFID_SHADOW.reg_RT) ) )
    {
        //stall the pipeline
        //prevent pc from incrementing
        PC.pc = PC.pc - 1;
        data_hazard = 1;
        // flush
        Flush_IF_ID(&IFID_SHADOW);
        printf("\nID(): Load Data Hazard Stalling\n");
    }

    // I format instructions: addi, addiu, andi, xori, beq, bne, bgtz, bltz, blez, lb, lbu, lhu, lui, lw, ori, slti, sltiu, sb, sh, sw,
    // Check for data dependence upon previous instruction from branch
    if( (( (EXMEM.Mem_Read) && ((EXMEM.WB_reg == IFID.reg_RT) || (EXMEM.WB_reg == IFID.reg_RS)) && (EXMEM.WB_reg != 0x00) )
        || ( (IDEX.Mem_Read) && ((IDEX.WB_reg == IFID.reg_RT) || (IDEX.WB_reg == IFID.reg_RS)) && (IDEX.WB_reg != 0x00) )
        || ( (IDEX.OP_Code == 0x00) && ((IDEX.WB_reg == IFID.reg_RT) || (IDEX.WB_reg == IFID.reg_RS)) && (IDEX.WB_reg != 0x00) )
        || ( (IDEX.I_format) && !(IDEX.Mem_Wrt) && ((IDEX.WB_reg == IFID.reg_RT) || (IDEX.WB_reg == IFID.reg_RS)) && (IDEX.WB_reg != 0x00) ))
        && ( (IFID.OP_Code == 0x1) || (IFID.OP_Code == 0x4) || (IFID.OP_Code == 0x5) || (IFID.OP_Code == 0x6) || (IFID.OP_Code == 0x7) )
       )
    {
        //stall
        printf("\nID(): Branch Data Hazard Stalling \n\n");
        PC.pc = PC.pc -1;
        IFID_SHADOW = IFID;
        Flush_ID_EX(&IDEX_SHADOW);
        data_hazard = 1;
    }
	return 0;
}

void EX()
{
	int ALU_A = IDEX.Reg_RS_val;
	int ALU_B = IDEX.Reg_RT_val;
	// base case if forwarding does not apply
	EXMEM_SHADOW.Reg_RT_val = IDEX.Reg_RT_val;

	printf("In Execute stage\n");

	//Forwarding Logic from the EXMEM Register
	if( EXMEM.Reg_Wrt && (EXMEM.WB_reg != 0) && (EXMEM.WB_reg == IDEX.reg_RS) )
    {
        ALU_A = EXMEM.ALU_result;
        printf("EX(): Forwarding from EXMEM to RS: ALU A = %d\n", ALU_A);
    }
    if( EXMEM.Reg_Wrt && (EXMEM.WB_reg != 0) && (EXMEM.WB_reg == IDEX.reg_RT) )
    {
        ALU_B = EXMEM.ALU_result;
        printf("EX(): Forwarding from EXMEM to RT: ALU B = %d\n", ALU_B);
        if(IDEX.OP_Code == 0x28 || IDEX.OP_Code == 0x29 || IDEX.OP_Code == 0x25)
        {
            EXMEM_SHADOW.Reg_RT_val = EXMEM.ALU_result;
        }
    }
    //Forwarding Logic from the MEMWB Register
    if ( MEMWB.Reg_Wrt && (MEMWB.WB_reg != 0)
        && !( EXMEM.Reg_Wrt && (EXMEM.WB_reg != 0) && (EXMEM.WB_reg == IDEX.reg_RS) )
        && (MEMWB.WB_reg == IDEX.reg_RS) )
    {
        if(MEMWB.Mem_to_Reg)
        {
            ALU_A = MEMWB.Data_Mem_result;
        }
        else
        {
            ALU_A = MEMWB.ALU_result;
        }
        printf("EX(): Forwarding from MEMWB to RS: ALU A = %d\n", ALU_A);
    }
    if ( MEMWB.Reg_Wrt && (MEMWB.WB_reg != 0)
        && !( EXMEM.Reg_Wrt && (EXMEM.WB_reg != 0) && (EXMEM.WB_reg == IDEX.reg_RT) )
        && (MEMWB.WB_reg == IDEX.reg_RT) )
    {
        if(MEMWB.Mem_to_Reg)
        {
            ALU_B = MEMWB.Data_Mem_result;
        }
        else
        {
            ALU_B = MEMWB.ALU_result;
        }
        EXMEM_SHADOW.Reg_RT_val = MEMWB.Data_Mem_result;
        printf("EX(): Forwarding from MEMWB to RT: ALU B = %d and RT_val = %d\n", ALU_A, EXMEM_SHADOW.Reg_RT_val);
    }

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
            case(0x8): // jr instruction leave black
                break;
            case(0xA):
                if(ALU_B == 0)
                {
                    EXMEM_SHADOW.ALU_result = ALU_A;
                }
                else
                {
                    EXMEM_SHADOW.ALU_result = reg[EXMEM_SHADOW.WB_reg];
                }
                break;
            case(0xB):
                if(ALU_B != 0)
                {
                    EXMEM_SHADOW.ALU_result = ALU_A;
                }
                else
                {
                    EXMEM_SHADOW.ALU_result = reg[EXMEM_SHADOW.WB_reg];
                }
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
                break;
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
                if((signed int)ALU_A < (signed int)EXMEM_SHADOW.Reg_RT_val)
                {
                    EXMEM_SHADOW.ALU_result = 1;
                }
                else
                {
                    EXMEM_SHADOW.ALU_result = 0;
                }
                break;
            case(0x2B): //sltu instruction
                if(ALU_A < EXMEM_SHADOW.Reg_RT_val)
                {
                    EXMEM_SHADOW.ALU_result = 1;
                }
                else
                {
                    EXMEM_SHADOW.ALU_result = 0;
                }
                break;
            default:
                printf("In EX(), Unknown func code for R format => %d\n", IDEX.ALU_Op );
                exit(-1);
        }
    }
    //J instruction
    else if (IDEX.OP_Code == 2 || IDEX.OP_Code == 3)
    {
        EXMEM_SHADOW.ALU_result = IDEX.PC_Next + 1; // so we can save the addr of the next inst in the WB stage
    }
    // seb instruction (sign-extend byte)
    else if (IDEX.OP_Code == 0x1F)
    {
        char byte_of_RT = ALU_B; // get just a byte of RT
        EXMEM_SHADOW.ALU_result = byte_of_RT; // now sign extend it
        EXMEM_SHADOW.Reg_RT_val = byte_of_RT;
    }
    // lb, lbu, lhu lw, sb, sh, and sw instruction
    else if(IDEX.OP_Code == 0x20 || IDEX.OP_Code == 0x23 || IDEX.OP_Code == 0x24 || IDEX.OP_Code == 0x28
            ||  IDEX.OP_Code == 0x2B || IDEX.OP_Code == 0x29 || IDEX.OP_Code == 0x25)
    {
        // because mips is byte addressable and our memory is modeled
        // as words.
        // Ex lw $t0, 32($s0) => we want to index a[32/4] => a[8]
        // Ex lb $to, 33($s0) => we want a[33/]4] => a[8] and byte (33 % 4) = byte 1
        // Ex lhu $t0, 2($t0) => we want a[2/4] => a[0] and half word (2 >> 1) % 2 = halfword 1 of word 0
        // ex lhu $t0, 6($t0) => a[6/4] => a[1] and halfword is (6 / 2) = 3 % 2 = halfword 1 of word 1
        EXMEM_SHADOW.ALU_result = (ALU_A >> 2) + (ALU_B >> 2);
        printf("In EX() load/store access = memory[ %d ]\n ", EXMEM_SHADOW.ALU_result);
        EXMEM_SHADOW.which_byte = (ALU_A + ALU_B) % 4;
        EXMEM_SHADOW.which_half = ((ALU_A + ALU_B) >> 1) % 2;
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
            default:
                printf("In EX(), failed I instr ALU op");
                exit(-1);
        }
    }



	EXMEM_SHADOW.branch = IDEX.branch;
	EXMEM_SHADOW.branch_target = IDEX.branch_target;

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
	int data_valid = 0;

	//write data to memory
	if(EXMEM.Mem_Wrt == 1)
	{
		unsigned int write_val;
		// wtite to memory
		word = EXMEM.Reg_RT_val;
		//memory[EXMEM.ALU_result] = EXMEM.Reg_RT_val
		// sw instruction
		if(EXMEM.OP_Code == 0x2B)
        {
            printf("In mem! EXMEM.ALU_result = %d\n", EXMEM.ALU_result);
            //memory[EXMEM.ALU_result] = word;
            //printf("after store word\n");
            write_val = word;
        }
        else if(EXMEM.OP_Code == 0x29)
        {
            if(EXMEM.which_half == 0)
            {
                //memory[EXMEM.ALU_result] = memory[EXMEM.ALU_result] & ~(BIG_END_HALFWORD_0);
                //memory[EXMEM.ALU_result] = (word << 16) | memory[EXMEM.ALU_result];
                write_val = memory[EXMEM.ALU_result] & ~(BIG_END_HALFWORD_0);
                write_val = (word << 16) | memory[EXMEM.ALU_result];
            }
            else if(EXMEM.which_half == 1)
            {
                //memory[EXMEM.ALU_result] = memory[EXMEM.ALU_result] & ~(BIG_END_HALFWORD_1);
                //memory[EXMEM.ALU_result] = (word) | memory[EXMEM.ALU_result];
                write_val = memory[EXMEM.ALU_result] & ~(BIG_END_HALFWORD_1);
                write_val = (word) | memory[EXMEM.ALU_result];
            }
            else
            {
                printf("Error: In sh instruction, Unknown half word value!\n");
                exit(-1);
            }

        }
        else if(EXMEM.OP_Code == 0x28)
        {
            if(EXMEM.which_byte == 0)
            {
                // zero out byte 0
                //memory[EXMEM.ALU_result] = memory[EXMEM.ALU_result] & ~(BIG_END_BYTE_0);
                // shift value to correct position and or with the memory value
                //memory[EXMEM.ALU_result] = (word << 24) | memory[EXMEM.ALU_result];
                write_val = memory[EXMEM.ALU_result] & ~(BIG_END_BYTE_0);
                write_val = (word << 24) | memory[EXMEM.ALU_result];
            }
            else if(EXMEM.which_byte == 1)
            {
                //memory[EXMEM.ALU_result] = memory[EXMEM.ALU_result] & ~(BIG_END_BYTE_1);
                //memory[EXMEM.ALU_result] = (word << 16) | memory[EXMEM.ALU_result];
                write_val = memory[EXMEM.ALU_result] & ~(BIG_END_BYTE_1);
                write_val = (word << 16) | memory[EXMEM.ALU_result];
            }
            else if(EXMEM.which_byte == 2)
            {
                //memory[EXMEM.ALU_result] = memory[EXMEM.ALU_result] & ~(BIG_END_BYTE_2);
                //memory[EXMEM.ALU_result] = (word << 8) | memory[EXMEM.ALU_result];
                write_val = memory[EXMEM.ALU_result] & ~(BIG_END_BYTE_2);
                write_val = (word << 8) | memory[EXMEM.ALU_result];
            }
            else if(EXMEM.which_byte == 3)
            {
                //memory[EXMEM.ALU_result] = memory[EXMEM.ALU_result] & ~(BIG_END_BYTE_3);
                //memory[EXMEM.ALU_result] = word | memory[EXMEM.ALU_result];
                write_val = memory[EXMEM.ALU_result] & ~(BIG_END_BYTE_3);
                write_val = word | memory[EXMEM.ALU_result];
            }
            else
            {
                printf("Error: In sb instruction, Unknown byte value!\n");
                exit(-1);
            }
        }
        if(DCACHE_ON)
        {
            //write to d_cache
            data_valid = dcache_access(0, EXMEM.ALU_result, &write_val);
            if(!data_valid)
            {
                stall_pipe = 1;
            }
        }
        else
        {
            memory[EXMEM.ALU_result] = write_val;
        }
	}
    // Read from mem
	if(EXMEM.Mem_Read == 1)
	{
        if (DCACHE_ON)
        {
            data_valid = dcache_access(1, EXMEM.ALU_result, &word);
        }
        else
        {
            data_valid = 1;
            word = memory[EXMEM.ALU_result];
        }

        if(!data_valid)
        {
                stall_pipe = 1;
        }
        else
        {
            //word = memory[EXMEM.ALU_result];
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
                    exit(-1);
                }
            }
            // lb and lbu instruction
            else if(EXMEM.OP_Code == 0x20 || EXMEM.OP_Code == 0x24)
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
                    exit(-1);
                }
                // sign-extend the result if lb instruction
                if (EXMEM.OP_Code == 0x20)
                {
                    char c = MEMWB_SHADOW.Data_Mem_result; // get just the one byte so we can sign-extend it
                    MEMWB_SHADOW.Data_Mem_result = c; // this sign-extends the byte
                }
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
    /*
    if (mem_handling_dcache_req && mem_penalty_count == main_memory_penalty)
    {
        main_memory_penalty = 0;
        mem_penalty_count = 0;
        filling_d_cache = 0;
        d_cache.valid[d_block_index] = 1;
        d_cache.dirty[d_block_index] = 0;
        mem_handling_dcache_req = 0;
        reset_mem_penalty_count = 1;
        // Add in write back penalty
        if( (WRITE_BACK) && (write_back_to_occur) )
        {
            printf("\n Write Back occurring\n");
            write_back_to_occur = 0;
            // at write penalty
            main_memory_penalty += 6;
            if(DCACHE_BLOCK_SIZE > 1)
            {
                for(int i = 0; i < DCACHE_BLOCK_SIZE - 1; i++)
                {
                    main_memory_penalty += 2;
                }
            }
            //write data to memory
            for(int i = 0; i < DCACHE_BLOCK_SIZE -1; i++)
            {
                memory[write_addr + i] = d_cache.data[write_addr + i];
            }
            mem_handling_dcache_req = 1;
        }
    }

    if(mem_handling_write_req && (mem_penalty_count == main_memory_penalty))
    {
        main_memory_penalty = 0;
        mem_penalty_count = 0;
        mem_handling_write_req = 0;
        //this is wrong block_index
        d_cache.dirty[d_block_index] = 0;
        reset_mem_penalty_count = 1;
    }
*/
    mem_update();
    icache_update();
    dcache_update();

	if(stall_pipe)
    {
        IFID = IFID;
        IDEX = IDEX;
        EXMEM = EXMEM;
        MEMWB = MEMWB;
        if(data_hazard)
        {
            data_hazard = 0; // if there's a data hazard, pc has already been decremented
        } else {
            PC.pc -= 1; // only decrement pc if no data hazard
        }
        stall_pipe = 0;
    }
    else
    {
        IFID = IFID_SHADOW;
        IDEX = IDEX_SHADOW;
        EXMEM = EXMEM_SHADOW;
        MEMWB = MEMWB_SHADOW;
        data_hazard = 0;
    }
	clock_cycle++;
	//main_memory_penalty--;
	if(reset_mem_penalty_count)
	{
	    reset_mem_penalty_count = 0;
	}
	else
    {
        mem_penalty_count++;
    }

	printf("v0 = %#010x\n", reg[2]);
	printf("v1 = %#010x\n", reg[3]);
	printf("a0 = %d\n", reg[4]);
	printf("a1 = %d\n", reg[5]);
	printf("a2 = %d\n", reg[6]);
    printf("a3 = %d\n\n", reg[7]);

    printf("t0 = %d\n", reg[8]);
    printf("t1 = %d\n", reg[9]);
    printf("t2 = %d\n", reg[10]);
    printf("t3 = %d\n", reg[11]);
    printf("t4 = %d\n", reg[12]);
    printf("t5 = %d\n\n", reg[13]);

    printf("ra = %d\n", reg[31]);
	printf("Clock Cycle: %d\n", clock_cycle);
	printf("The Program Counter = %d\n\n", PC.pc);
	printf("-------------------------------------------------\n");
}
