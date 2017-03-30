/**
 *
 * ECEN 4593 Computer Organization
 * Justin Barnitz and Jacob Levine
 *
 */

#include "cache.h"

unsigned int program_image[MEMORY_SIZE] = {4000,4000,0,0,0,120,0,0,0,0,
		0x27bdfff0,    // addiu	sp,sp,-16 <load_arrays>
		0xafbe000c,    // sw	s8,12(sp)
		0x03a0f025,    // move	s8,sp
		0xafc40010,    // sw	a0,16(s8)
		0xafc50014,    // sw	a1,20(s8)
		0xafc00004,    // sw	zero,4(s8)
		0x1000001c,    // b	4008bc <load_arrays+0x8c>
		0x00000000,    // nop
		0x8fc20004,    // lw	v0,4(s8)
		0x00021080,    // sll	v0,v0,0x2
		0x8fc30010,    // lw	v1,16(s8)
		0x00621021,    // addu	v0,v1,v0
		0x8fc30004,    // lw	v1,4(s8)
		0x00031880,    // sll	v1,v1,0x2
		0x8fc40010,    // lw	a0,16(s8)
		0x00831821,    // addu	v1,a0,v1
		0x8c640000,    // lw	a0,0(v1)
		0x8fc30004,    // lw	v1,4(s8)
		0x00831821,    // addu	v1,a0,v1
		0xac430000,    // sw	v1,0(v0)
		0x8fc20004,    // lw	v0,4(s8)
		0x00021080,    // sll	v0,v0,0x2
		0x8fc30014,    // lw	v1,20(s8)
		0x00621021,    // addu	v0,v1,v0
		0x8fc30004,    // lw	v1,4(s8)
		0x00031880,    // sll	v1,v1,0x2
		0x8fc40014,    // lw	a0,20(s8)
		0x00831821,    // addu	v1,a0,v1
		0x8c640000,    // lw	a0,0(v1)
		0x8fc30004,    // lw	v1,4(s8)
		0x00831821,    // addu	v1,a0,v1
		0xac430000,    // sw	v1,0(v0)
		0x8fc20004,    // lw	v0,4(s8)
		0x24420001,    // addiu	v0,v0,1
		0xafc20004,    // sw	v0,4(s8)
		0x8fc20004,    // lw	v0,4(s8)
		0x28420064,    // slti	v0,v0,100
		0x1440ffe2,    // bnez	v0,400850 <load_arrays+0x20>
		0x00000000,    // nop
		0x00000000,    // nop
		0x03c0e825,    // move	sp,s8
		0x8fbe000c,    // lw	s8,12(sp)
		0x27bd0010,    // addiu	sp,sp,16
		0x03e00008,    // jr	ra
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x27bdffe8,    // 	addiu	sp,sp,-24 <check_result>
		0xafbe0014,    // sw	s8,20(sp)
		0x03a0f025,    // move	s8,sp
		0xafc40018,    // sw	a0,24(s8)
		0xafc5001c,    // sw	a1,28(s8)
		0xafc00000,    // sw	zero,0(s8)
		0xafc00008,    // sw	zero,8(s8)
		0x1000001d,    // b	400978 <check_result+0x94>
		0x00000000,    // nop
		0x8fc20008,    // lw	v0,8(s8)
		0x00021080,    // sll	v0,v0,0x2
		0x8fc30018,    // lw	v1,24(s8)
		0x00621021,    // addu	v0,v1,v0
		0x8c430000,    // lw	v1,0(v0)
		0x8fc20008,    // lw	v0,8(s8)
		0x00021080,    // sll	v0,v0,0x2
		0x8fc4001c,    // lw	a0,28(s8)
		0x00821021,    // addu	v0,a0,v0
		0x8c420000,    // lw	v0,0(v0)
		0x00621026,    // xor	v0,v1,v0
		0xafc2000c,    // sw	v0,12(s8)
		0x8fc2000c,    // lw	v0,12(s8)
		0x10400003,    // beqz	v0,40094c <check_result+0x68>
		0x00000000,    // nop
		0x24020001,    // li	v0,1
		0xafc20000,    // sw	v0,0(s8)
		0x8fc20008,    // lw	v0,8(s8)
		0x00021080,    // sll	v0,v0,0x2
		0x8fc30018,    // lw	v1,24(s8)
		0x00621021,    // addu	v0,v1,v0
		0x8c420000,    // lw	v0,0(v0)
		0x8fc30004,    // lw	v1,4(s8)
		0x00621021,    // addu	v0,v1,v0
		0xafc20004,    // sw	v0,4(s8)
		0x8fc20008,    // lw	v0,8(s8)
		0x24420001,    // addiu	v0,v0,1
		0xafc20008,    // sw	v0,8(s8)
		0x8fc20008,    // lw	v0,8(s8)
		0x28420064,    // slti	v0,v0,100
		0x1440ffe1,    // bnez	v0,400908 <check_result+0x24>
		0x00000000,    // nop
		0x8fc20000,    // lw	v0,0(s8)
		0x14400003,    // bnez	v0,40099c <check_result+0xb8>
		0x00000000,    // nop
		0x8fc20004,    // lw	v0,4(s8)
		0xafc20000,    // sw	v0,0(s8)
		0x00000000,    // nop
		0x03c0e825,    // move	sp,s8
		0x8fbe0014,    // lw	s8,20(sp)
		0x27bd0018,    // addiu	sp,sp,24
		0x03e00008,    // jr	ra
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x27bdfcb8,    // addiu	sp,sp,-840 <main>
		0xafbf0344,    // sw	ra,836(sp)
		0xafbe0340,    // sw	s8,832(sp)
		0x03a0f025,    // move	s8,sp
//		0x3c1c0042,    // lui	gp,0x42
//		0x279c9010,    // addiu	gp,gp,-28656
//		0xafbc0010,    // sw	gp,16(sp)
//		0x8f828050,    // lw	v0,-32688(gp)
//		0x8c420000,    // lw	v0,0(v0)
//		0xafc2033c,    // sw	v0,828(s8)
		0x27c301ac,    // addiu	v1,s8,428
		0x27c2001c,    // addiu	v0,s8,28
		0x00602825,    // move	a1,v1
		0x00402025,    // move	a0,v0
		0x0c00000a,    // jal	10 <load_arrays>
		0x00000000,    // nop
//		0x8fdc0010,    // lw	gp,16(s8)
		0x27c301ac,    // addiu	v1,s8,428
		0x27c2001c,    // addiu	v0,s8,28
		0x00602825,    // move	a1,v1
		0x00402025,    // move	a0,v0
		0x0c00003c,    // jal	60 <check_result>
		0x00000000,    // nop
//		0x8fdc0010,    // lw	gp,16(s8)
		0x00000000,    // nop
//		0x8f828050,    // lw	v0,-32688(gp)
//		0x8fc3033c,    // lw	v1,828(s8)
//		0x8c420000,    // lw	v0,0(v0)
//		0x10620005,    // beq	v1,v0,400a3c <main+0x88>
		0x00000000,    // nop
//		0x8f82804c,    // lw	v0,-32692(gp)
//		0x0040c825,    // move	t9,v0
//		0x0320f809,    // jalr	t9
		0x00000000,    // nop
		0x03c0e825,    // move	sp,s8
		0x8fbf0344,    // lw	ra,836(sp)
		0x8fbe0340,    // lw	s8,832(sp)
		0x27bd0348,    // addiu	sp,sp,840
		0x00004020,		// add $t0, $zero, $zero
		0x8d090c44,		// lw $t1, 3140($t0)
		0xad090020,		// sw $t1, 32($t0)
		0x8d080420,		//lw $t0, 1056($t0)
		0x8d090c4c,		// lw $t1, 3148($t0)
		0xad09001c,		// sw $t1, 28($t0)
		0x8d08041c,		//lw $t0, 1054($t0)
		0x8d090f90,		//lw $t1, 3984($t0)
		0xad090018,		//sw $t1, 24($t0)
		0x8d080418,		//lw $t0, 1048($t0)
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000008,    // jr	$zero
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000,    // nop
		0x00000000};    // nop

void init_memory()
{
	//memory[0] = 0x01314020; //ADD $t0 $t1 $s1
	//memory[1] = 0x214A0005; //addi $t2 $t2 $0x5
	Initialize_Simulation_Memory();
}

// returns the address of 1st instruction
unsigned int init_i_cache()
{
	//add t0 t1 s1
	i_cache[0] = 0x01314020;
	return i_cache[0];
}

void init_d_cache()
{

}

/*
* MIPS Register Layout
* 0     => $zero
* 1     => $at
* 2-3   => $v0-$v1
* 4-7   => $a0-$a3
* 8-15  => $t0-$t7
* 16-23 => $s0-$s7
* 24-25 => $t8-$t9
* 26-27 => $k0-$k1
* 28    => $gp
* 29    => $sp
* 30    => $fp
* 31    => $ra
*
*/

void init_reg()
{
	for(int i = 0; i < NUM_REGISTERS; i++)
	{
		reg[i] = 0;
	}
	reg[29] = memory[0];
	reg[30] = memory[1];
}

void Initialize_Simulation_Memory(void){

	for (int i=0; i < MEMORY_SIZE; i++){
		memory[i] = program_image[i];
	}
}
