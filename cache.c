/**
 *
 * ECEN 4593 Computer Organization
 * Justin Barnitz and Jacob Levine
 *
 */

#include "cache.h"


void init_memory()
{

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
	// todo -> init sp, gp and ra
}