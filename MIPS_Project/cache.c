/**
 * ECEN 4593 Computer Organization
 * Justin Barnitz and Jacob Levine
 *
 */

#include "cache.h"

void init_memory()
{
	Initialize_Simulation_Memory();
}

// returns the address of 1st instruction
void init_i_cache()
{
	int tmp = 1;
	// number of indexes
	int size = (ICACHE_SIZE / 4) / BLOCK_SIZE;
	int data_size = (ICACHE_SIZE / 4);
	//printf("The number of indexes in i cache = %d\n", size);

	for(int i = 0; i < size; i++)
    {
        i_cache.tag[i] = 0;
        i_cache.valid[i] = 0;
        i_cache.dirty[i] = 0;
        tmp = (tmp << 1) + 1;
    }
    for(int i = 0; i < data_size; i++)
    {
        i_cache.data[i] = 0;
    }

    filling_icache = 0;
    icache_hit_count = 0;
    icache_miss_count = 0;
    mem_handling_icache_req = 0;

    // calculate
    icache_entries = (ICACHE_SIZE / 4);
    i_block_offset_bits = log(BLOCK_SIZE)/log(2);

    // current icache request vars
    icache_req_addr = 0;
    icache_req_tag = 0;
    icache_req_index = 0;
    icache_req_blkoffset = 0;
    icache_req_cache_addr = 0;


    mem_data_valid = 0;
    icache_blocks_filled = 0;
}

void init_d_cache()
{
	// number of indexes
	int size = (DCACHE_SIZE / 4) / BLOCK_SIZE;
	int data_size = (DCACHE_SIZE / 4);
	//printf("The number of indexes in d cache = %d\n", size);

	for(int i = 0; i < size; i++)
    {
        d_cache.tag[i] = 0;
        d_cache.valid[i] = 0;
        d_cache.dirty[i] = 0;
    }
    for(int i = 0; i < data_size; i++)
    {
        d_cache.data[i] = 0;
    }

    for(int i = 0; i < BLOCK_SIZE - 1; i++)
    {
        write_buffer[i] = 0;
    }

    dcache_hit_count = 0;
    dcache_miss_count = 0;
    mem_handling_dcache_req =0;
    write_back_to_occur = 0;
    write_addr = 0;

    // calculate
    dcache_entries = (DCACHE_SIZE / 4);
    d_block_offset_bits = log(BLOCK_SIZE)/log(2);
    d_cache_tag_bits = log(dcache_entries)/log(2);
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

int icache_access(unsigned int address, unsigned int *data)
{
    int icache_data_valid = 0;
    unsigned int mem_data = 0;
    unsigned int icache_tag = 0;
    unsigned int icache_remainder = 0;
    unsigned int cache_addr = 0;

    // calculate
    icache_tag = address / icache_entries;
    icache_remainder = address % icache_entries;
    i_block_index = icache_remainder / BLOCK_SIZE;
    i_block_offset = icache_remainder % BLOCK_SIZE;
    cache_addr = (i_block_index << i_block_offset_bits) + i_block_offset;

    //printf("\nIn icache_access()\n");
    //printf("Adddress = %u\n", address);

    // Cache Hit
    if( (icache_tag == i_cache.tag[i_block_index]) && (i_cache.valid[i_block_index] == 1) )
    {
        //printf("i_cache hit occurred!\n");
        icache_hit_count++;
        *data = i_cache.data[cache_addr];
        icache_data_valid = 1;
    }
    // Cache Miss -> Make a new request to memory
    else if (!filling_icache)
    {
        //printf("i_cache miss occurred. Creating new request\n");
        icache_miss_count++;
        filling_icache = 1;
        // Keep track of the new request address
        icache_req_addr = address;
        // set the address to the first of the block
        icache_req_addr_beginning = (icache_req_addr >> i_block_offset_bits) << i_block_offset_bits;
        icache_req_tag = icache_tag;
        icache_req_index = i_block_index;
        icache_req_blkoffset = i_block_offset;
        icache_req_cache_addr = cache_addr - i_block_offset;
        // pass to memory access the address of the start of the block
        mem_data_valid = memory_access(1, icache_req_addr_beginning, &mem_data, 1);
        i_cache.tag[icache_req_index] = icache_req_tag;
        i_cache.valid[icache_req_index] = 0;
        icache_blocks_filled = 0;
        icache_data_valid = 0;

    }
    // Check early start
    else
    {
       // early start
        if( (i_block_offset < icache_blocks_filled) &&  (icache_tag == icache_req_tag) && (i_block_index == icache_req_index))
        {
            *data = i_cache.data[cache_addr];
            icache_data_valid = 1;
            icache_hit_count++;
        }
        else
        {
            icache_miss_count++;
        }
    }
    return icache_data_valid;
}

int dcache_access(int read, unsigned int address, unsigned int *data)
{
    int dcache_data_valid = 0;
    unsigned int mem_data = 0;
    unsigned int dcache_tag = 0;
    unsigned int dcache_remainder = 0;
    unsigned int cache_addr = 0;

    // calculate
    dcache_tag = address / dcache_entries;
    dcache_remainder = address % dcache_entries;
    d_block_index = dcache_remainder / BLOCK_SIZE;
    d_block_offset = dcache_remainder % BLOCK_SIZE;
    cache_addr = (d_block_index << d_block_offset_bits) + d_block_offset;

    //printf("\nIn dcache_access()\n");
    //printf("Adddress = %u\n", address);
//    printf("dcache_tag = %d\n", dcache_tag);
//    printf("dcache_remainder = %d\n", dcache_remainder);
//    printf("d_block_index = %d\n", d_block_index);
//    printf("d_block_offset = %d\n", d_block_offset);
//    printf("cache_addr = %d\n", cache_addr);

    if(read)
    {
        // Cache hit
        if( (dcache_tag == d_cache.tag[d_block_index]) && (d_cache.valid[d_block_index] == 1) )
        {
            //printf("d_cache hit occurred!\n");
            dcache_hit_count++;
            *data = d_cache.data[cache_addr];
            dcache_data_valid = 1;
        }
        // create new request to memory
        else if(!filling_dcache)
        {
            //printf("d_cache miss occurred on read. Creating new request\n");
            dcache_miss_count++;
            filling_dcache = 1;
            // Keep track of the new request address
            dcache_req_addr = address;
            // set the address to the first of the block
            dcache_req_addr_beginning = (dcache_req_addr >> d_block_offset_bits) << d_block_offset_bits;
            dcache_req_tag = dcache_tag;
            dcache_req_index = d_block_index;
            dcache_req_blkoffset = d_block_offset;
            dcache_req_cache_addr = cache_addr - d_block_offset;

            // If the current block is dirty, save to the write buffer before bring in new block
            if(WRITE_BACK && (d_cache.dirty[dcache_req_index] == 1) && !write_back_to_occur)
            {
                //printf("Data block is dirty. Placing data into write buffer\n");
                for(int i = 0; i < BLOCK_SIZE; i++)
                {
                    //printf("writing %d to write_buffer[%d]\n", d_cache.data[dcache_req_cache_addr + i], i);
                    write_buffer[i] = d_cache.data[dcache_req_cache_addr + i];
                }
                write_back_address = (d_cache.tag[dcache_req_index] << d_cache_tag_bits) + (dcache_req_index << d_block_offset_bits);
                write_back_to_occur = 1;
            }
            // If the write buffer is already full stall the pipe
            else if(WRITE_BACK && write_back_to_occur)
            {
                //printf("Data block is dirty, but write buffer is full, must stall\n");
                return 0;
            }

            // pass to memory access the address of the start of the block
            mem_data_valid = memory_access(1, dcache_req_addr_beginning, &mem_data, 0);
            dcache_read_req = 1;
            d_cache.tag[dcache_req_index] = dcache_req_tag;
            d_cache.valid[dcache_req_index] = 0;
            d_cache.dirty[dcache_req_index] = 0;
            dcache_blocks_filled = 0;
            dcache_data_valid = 0;
        }
    }
    //Write
    else
    {
        // if memory is currently busy, cannot execute write through and have to stall
        if( (!WRITE_BACK) && (filling_dcache || filling_icache) )
        {
            dcache_miss_count++;
            return 0;
        }

        // Cache hit
        if( (dcache_tag == d_cache.tag[d_block_index]) && (d_cache.valid[d_block_index] == 1) )
        {
            //printf("Dcache write hit\n");
            dcache_hit_count++;
            dcache_req_addr = address; // this was cache_addr
            d_cache.data[cache_addr] = *data;
            d_cache.dirty[d_block_index] = 1;
            if(!WRITE_BACK)
            {
                // write through
                //printf("d_cache hit occurred on write-through. Creating new request\n");
                filling_dcache = 1;
                // Keep track of the new request address
                // on a write-through, don't set the address to the first of the block
                dcache_req_addr_beginning = address;
                dcache_req_tag = dcache_tag;
                dcache_req_index = d_block_index;
                dcache_req_blkoffset = d_block_offset;
                dcache_req_cache_addr = cache_addr; // this was cache_addr - d_block_offset;
                // pass to memory access the address of the start of the block
                mem_data_valid = memory_access(0, address, data, 0);
                dcache_read_req = 0;
                d_cache.tag[dcache_req_index] = dcache_req_tag;
                dcache_blocks_filled = 0;
            }
            dcache_data_valid = 1;
        }
        // data is not in cache have to create a read request from main memory
        else if(!filling_dcache)
        {
            //printf("d_cache miss occurred on write. Creating new request\n");
            dcache_miss_count++;
            filling_dcache = 1;
            // Keep track of the new request address
            dcache_req_addr = address;
            // set the address to the first of the block
            dcache_req_addr_beginning = (dcache_req_addr >> d_block_offset_bits) << d_block_offset_bits;
            dcache_req_tag = dcache_tag;
            dcache_req_index = d_block_index;
            dcache_req_blkoffset = d_block_offset;
            dcache_req_cache_addr = cache_addr - d_block_offset;

            // If the current block is dirty, save to the write buffer before bring in new block
            if(WRITE_BACK && (d_cache.dirty[dcache_req_index] == 1) && !write_back_to_occur)
            {
                //printf("Data block is dirty. Placing data into write buffer\n");
                for(int i = 0; i < BLOCK_SIZE; i++)
                {
                    write_buffer[i] = d_cache.data[dcache_req_cache_addr + i];
                }
                write_back_address = (d_cache.tag[dcache_req_index] << d_cache_tag_bits) + (dcache_req_index << d_block_offset_bits);
                write_back_to_occur = 1;
            }
            // If the write buffer is already full stall the pipe
            else if(WRITE_BACK && (d_cache.dirty[dcache_req_index] == 1) && write_back_to_occur)
            {
                //printf("Data block is dirty, but write buffer is full, must stall\n");
                return 0;
            }

            // pass to memory access the address of the start of the block
            mem_data_valid = memory_access(1, dcache_req_addr_beginning, &mem_data, 0);
            dcache_read_req = 1;
            d_cache.tag[dcache_req_index] = dcache_req_tag;
            d_cache.valid[dcache_req_index] = 0;
            dcache_blocks_filled = 0;
            dcache_data_valid = 0;
        }

    }
    return dcache_data_valid;
}

int memory_access(int read, unsigned int address, unsigned int *data, int i_cache_Request)
{
    int data_valid = 0;
    if( i_cache_Request && mem_handling_dcache_req ) { return 0; }
    if( !i_cache_Request && mem_handling_icache_req ) { return 0; }

    // If memory is idle
    if( !mem_handling_icache_req && !mem_handling_dcache_req )
    {
        // Create a new i cache request
        if( i_cache_Request )
        {
            //printf(" In Memory: Creating new Icache request\n");
            mem_counter = 7;
            mem_handling_icache_req = 1;
        }
        // Create a new dcache request
        else
        {
            //printf(" In Memory: Creating new Dcache request\n");
            if(read)
            {
                mem_counter = 8;
            }
            else
            {
                mem_counter = 6;
            }
            mem_handling_dcache_req = 1;
        }
        mem_blksize = BLOCK_SIZE;
        data_valid = 0;
    }
    // Memory is working on Icache
    else if( mem_handling_icache_req )
    {
        if( read )
        {
            // Penalty has expired. Deliver data
            if( mem_counter == 0)
            {
                //printf("Memory Penalty has expired. Delivering Data to Icache\n");
                mem_blksize--;
                *data = memory[address];
                data_valid = 1;
                // Check if all blocks have been filled
                if( mem_blksize == 0 )
                {
                    //printf("Memory has filled all blocks of the request\n");
                    mem_handling_icache_req = 0;
                }
                // If more blocks to be filled, add penalty of 2
                else
                {
                    //printf("Memory adding next block penalty\n");
                    mem_counter = 2;
                }
            }
        }
    }
    else if( mem_handling_dcache_req )
    {
        if( mem_counter == 0)
        {
            if(read)
            {
                //printf("Memory Penalty has expired. Delivering Data to Dcache\n");
                mem_blksize--;
                *data = memory[address];
                data_valid = 1;
                // Check if all blocks have been filled
                if( mem_blksize == 0 )
                {
                    //printf("Memory has filled all blocks of the request\n");
                    mem_handling_dcache_req = 0;
                }
                // If more blocks to be filled, add penalty of 2
                else
                {
                    //printf("Memory adding next block penalty\n");
                    mem_counter = 2;
                }
            }
            else
            {
                // If the req was write_back, it will write the whole block back
                if(WRITE_BACK)
                {
                    //printf("Writing write buffer to memory\n");
                    mem_blksize--;
                    memory[address] = *data;
                    data_valid = 1;
                    if( mem_blksize == 0 )
                    {
                        //printf("Memory has filled all blocks of the request\n");
                        mem_handling_dcache_req = 0;
                    }
                    // If more blocks to be filled, add penalty of 2
                    else
                    {
                        //printf("Memory adding next block penalty\n");
                        mem_counter = 2;
                    }
                }
                else
                {
                    // Write through is only 1 word
                    memory[address] = *data;
                    data_valid = 1;
                    mem_handling_dcache_req = 0;
                    //printf("Write through is complete\n");
                    mem_blksize = 0;
                }
            }
        }
    }
    return data_valid;
}

void mem_update()
{
    if( mem_counter > 0 )
    {
        mem_counter--;
    }
}

void icache_update()
{
    int mem_data_valid = 0;
    unsigned int mem_data;
    unsigned int icache_next_req_addr;
    // If icache has not been checked this cycle
    if( filling_icache )
    {
        //printf("In icache update\n");
        icache_next_req_addr = icache_req_addr_beginning + icache_blocks_filled;
        //printf("Waiting on data for address: %d\n", icache_next_req_addr);

        // pass to memory access the address of the waiting block offset
        mem_data_valid = memory_access(1, icache_next_req_addr, &mem_data, 1);
        // memory has data for i cache
        if(mem_data_valid)
        {
            // fill the correct block
            i_cache.data[icache_req_cache_addr + icache_blocks_filled] = mem_data;
            icache_blocks_filled++;

            // check if all blocks have been filled
            if(icache_blocks_filled == BLOCK_SIZE)
            {
                filling_icache = 0;
                i_cache.valid[icache_req_index] = 1;
            }
        }
    }
}

void dcache_update()
{
    int mem_data_valid = 0;
    unsigned int mem_data = 0;
    unsigned int dcache_next_req_addr = 0;
    // If icache has not been checked this cycle
    if( filling_dcache )
    {
        //printf("In dcache update\n");
        dcache_next_req_addr = dcache_req_addr_beginning + dcache_blocks_filled;
        //printf("Waiting on data for address: %d\n", dcache_next_req_addr);

        if(dcache_read_req)
        {
            // pass to memory access the address of the waiting block offset
            mem_data_valid = memory_access(1, dcache_next_req_addr, &mem_data, 0);
            // memory has data for d cache
            if(mem_data_valid)
            {
                // fill the correct block
                d_cache.data[dcache_req_cache_addr + dcache_blocks_filled] = mem_data;
                dcache_blocks_filled++;

                // check if all blocks have been filled
                if(dcache_blocks_filled == BLOCK_SIZE)
                {
                    filling_dcache = 0;
                    d_cache.valid[dcache_req_index] = 1;
                    // reset blocks filled to zero incase a write back need to occur
                    dcache_blocks_filled = 0;
                    // handle write back
                    if(WRITE_BACK && write_back_to_occur)
                    {
                        //printf("In dcache update, setting up write back\n");
                        //write_back_to_occur = 0;
                        dcache_read_req = 0;
                        filling_dcache = 1;
                        //write_back_happing = 1;
                    }
                }
            }
        }
        else
        {
            // This is write through, only one word will be written to memory
            if(!WRITE_BACK)
            {
                mem_data = d_cache.data[dcache_req_cache_addr];
                // pass to memory access the address of the waiting block offset
                mem_data_valid = memory_access(0, dcache_req_addr_beginning + dcache_blocks_filled, &mem_data, 0);
                // memory has data for i cache
                if(mem_data_valid)
                {
                    filling_dcache = 0;
                    d_cache.valid[dcache_req_index] = 1;
                }
            }
            // this is write back, must write full block to memory
            else
            {
                // grab data from write buffer
                mem_data = write_buffer[dcache_blocks_filled];
                // write to mem
                mem_data_valid = memory_access(0, write_back_address + dcache_blocks_filled, &mem_data, 0);

                if(mem_data_valid)
                {
                    dcache_blocks_filled++;

                    // check if all blocks have been written
                    if(dcache_blocks_filled == BLOCK_SIZE)
                    {
                        filling_dcache = 0;
                        write_back_to_occur = 0;
                    }
                }
            }

        }
    }
}

int RMW_dcache(unsigned int address)
{
    unsigned int cache_data = 0;
    unsigned int tag = 0;
    unsigned int remainder = 0;
    unsigned int cache_addr = 0;
    unsigned int index = 0;
    unsigned int offset = 0;

    // calculate
    tag = address / dcache_entries;
    remainder = address % dcache_entries;
    index = remainder / BLOCK_SIZE;
    offset = remainder % BLOCK_SIZE;
    cache_addr = (index << d_block_offset_bits) + offset;

    // Check cache
    if( tag == d_cache.tag[index] && d_cache.valid[index] )
    {
        cache_data = d_cache.data[cache_addr];
    }
    else
    {
        cache_data = memory[address];
    }
    return cache_data;
}

// Program 1
#ifdef RUN_PROG_1
unsigned int program_image[MEMORY_SIZE] = {
0x00000bb8,	//	$sp = 3000
0x00000bb8,	//	$fp = 3000
0x00000000,
0x00000000,
0x00000000,
0x0000008c,	//	$pc = 140
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x18c00008,   // 	blez	a2,400854 <copy_array+0x24>
0x00801025,   // 	move	v0,a0
0x00063080,   // 	sll	a2,a2,0x2
0x00862021,   // 	addu	a0,a0,a2
0x8c430000,   // 	lw	v1,0(v0)
0xaca30000,   // 	sw	v1,0(a1)
0x24420004,   // 	addiu	v0,v0,4
0x1444fffc,   // 	bne	v0,a0,400840 <copy_array+0x10>
0x24a50004,   // 	addiu	a1,a1,4
0x03e00008,   // 	jr	ra
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x1ca00018,   // 	bgtz	a1,4008c0 <bubble_sort+0x64>
0x00005825,   // 	move	t3,zero
0x03e00008,   // 	jr	ra
0x00001025,   // 	move	v0,zero
0x8c660000,   // 	lw	a2,0(v1)
0x8c670004,   // 	lw	a3,4(v1)
0x00e6402a,   // 	slt	t0,a3,a2
0x11000004,   // 	beqz	t0,40088c <bubble_sort+0x30>
0x00000000,   // 	nop
0xac670000,   // 	sw	a3,0(v1)
0xac660004,   // 	sw	a2,4(v1)
0x240a0001,   // 	li	t2,1
0x24630004,   // 	addiu	v1,v1,4
0x1469fff6,   // 	bne	v1,t1,40086c <bubble_sort+0x10>
0x00000000,   // 	nop
0x004c1021,   // 	addu	v0,v0,t4
0x10000002,   // 	b	4008a8 <bubble_sort+0x4c>
0x00aa580a,   // 	movz	t3,a1,t2
0x00a05825,   // 	move	t3,a1
0x256b0001,   // 	addiu	t3,t3,1
0x0165182a,   // 	slt	v1,t3,a1
0x14600006,   // 	bnez	v1,4008cc <bubble_sort+0x70>
0x01ab6023,   // 	subu	t4,t5,t3
0x03e00008,   // 	jr	ra
0x8c000820,		//0x00000000,   // 	nop
0x00001025,   // 	move	v0,zero
0x24adffff,   // 	addiu	t5,a1,-1
0x01ab6023,   // 	subu	t4,t5,t3
0x1980fff5,   // 	blez	t4,4008a4 <bubble_sort+0x48>
0x00801825,   // 	move	v1,a0
0x000c4880,   // 	sll	t1,t4,0x2
0x00894821,   // 	addu	t1,a0,t1	0x8c190812,	//		flush memory location for testing
0x1000ffe3,   // 	b	40086c <bubble_sort+0x10>
0x00005025,   // 	move	t2,zero
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x28a20002,   // 	slti	v0,a1,2
0x14400029,   // 	bnez	v0,400990 <insertion_sort+0xac>
0x248e0004,   // 	addiu	t6,a0,4
0x0080c025,   // 	move	t8,a0
0x24190004,   // 	li	t9,4
0x00001025,   // 	move	v0,zero
0x10000016,   // 	b	400958 <insertion_sort+0x74>
0x240f0001,   // 	li	t7,1
0xac690000,   // 	sw	t1,0(v1)
0x24420001,   // 	addiu	v0,v0,1
0x104d000c,   // 	beq	v0,t5,400940 <insertion_sort+0x5c>
0xacc80000,   // 	sw	t0,0(a2)
0x006c1821,   // 	addu	v1,v1,t4
0x00831821,   // 	addu	v1,a0,v1
0x8ce8fffc,   // 	lw	t0,-4(a3)
0x00cb3021,   // 	addu	a2,a2,t3
0x24e7fffc,   // 	addiu	a3,a3,-4
0x8ce9fffc,   // 	lw	t1,-4(a3)
0x0109502a,   // 	slt	t2,t0,t1
0x1540fff4,   // 	bnez	t2,400904 <insertion_sort+0x20>
0x00863021,   // 	addu	a2,a0,a2
0x10000003,   // 	b	400948 <insertion_sort+0x64>
0x25ef0001,   // 	addiu	t7,t7,1
0x01a01025,   // 	move	v0,t5
0x25ef0001,   // 	addiu	t7,t7,1
0x25ce0004,   // 	addiu	t6,t6,4
0x27180004,   // 	addiu	t8,t8,4
0x10af0010,   // 	beq	a1,t7,400994 <insertion_sort+0xb0>
0x27390004,   // 	addiu	t9,t9,4
0x19e0fffa,   // 	blez	t7,400944 <insertion_sort+0x60>
0x01c01825,   // 	move	v1,t6
0x8dc80000,   // 	lw	t0,0(t6)
0x8f090000,   // 	lw	t1,0(t8)
0x0109382a,   // 	slt	a3,t0,t1
0x10e0fff5,   // 	beqz	a3,400944 <insertion_sort+0x60>
0x03003025,   // 	move	a2,t8
0x004f6821,   // 	addu	t5,v0,t7
0x01c03825,   // 	move	a3,t6
0x272cfffc,   // 	addiu	t4,t9,-4
0x018e6023,   // 	subu	t4,t4,t6
0x272bfff8,   // 	addiu	t3,t9,-8
0x1000ffde,   // 	b	400904 <insertion_sort+0x20>
0x01785823,   // 	subu	t3,t3,t8
0x00001025,   // 	move	v0,zero
0x03e00008,   // 	jr	ra
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x18c0000e,   // 	blez	a2,4009dc <check_result+0x3c>
0x00801825,   // 	move	v1,a0
0x00063080,   // 	sll	a2,a2,0x2
0x00862021,   // 	addu	a0,a0,a2
0x00001025,   // 	move	v0,zero
0x24060001,   // 	li	a2,1
0x8c670000,   // 	lw	a3,0(v1)
0x8ca80000,   // 	lw	t0,0(a1)
0x00e83826,   // 	xor	a3,a3,t0
0x00c7100b,   // 	movn	v0,a2,a3
0x24630004,   // 	addiu	v1,v1,4
0x1464fffa,   // 	bne	v1,a0,4009b8 <check_result+0x18>
0x24a50004,   // 	addiu	a1,a1,4
0x03e00008,   // 	jr	ra
0x00000000,   // 	nop
0x03e00008,   // 	jr	ra
0x00001025,   // 	move	v0,zero
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x27bdf7f8,   // 	addiu	sp,sp,-2056
0xafbf0804,   // 	sw	ra,2052(sp)
0xafb30800,   // 	sw	s3,2048(sp)
0xafb207fc,   // 	sw	s2,2044(sp)
0xafb107f8,   // 	sw	s1,2040(sp)
0xafb007f4,   // 	sw	s0,2036(sp)
0x8c420000,   // 	lw	v0,0(v0)
0xafa207ec,   // 	sw	v0,2028(sp)
0x240600fa,   // 	li	a2,250
0x27b10404,   // 	addiu	s1,sp,1028
0x02202825,   // 	move	a1,s1
0x27b0001c,   // 	addiu	s0,sp,28
0x02002025,   // 	move	a0,s0
0x0c00000a,   // 	jal	10 <copy_array>
0x00000000,   // 	nop
0x240500fa,   // 	li	a1,250
0x02002025,   // 	move	a0,s0
0x0c00001e,   // 	jal	30 <bubble_sort>
0x00000000,   // 	nop
0x00409025,   // 	move	s2,v0
0x240500fa,   // 	li	a1,250
0x02202025,   // 	move	a0,s1
0x0c000046,   // 	jal 70 <insertion_sort>
0x00000000,   // 	nop
0x00409825,   // 	move	s3,v0
0x240600fa,   // 	li	a2,250
0x02202825,   // 	move	a1,s1
0x02002025,   // 	move	a0,s0
0x0c000078,   // 	jal	120 <check_result>
0x00000000,   // 	nop
0x0240a020,	  // 	move $s32 to $s4 to be stored into result memory location
0x0272902a,   // 	slt	s2,s3,s2
0x16400003,   // 	bnez	s2,400a90 <main+0xac>
0x3c03000f,   // 	lui	v1,0xf
0x24634240,   // 	addiu	v1,v1,16960
0x00431021,   // 	addu	v0,v0,v1
0xac14001c,	  //	sw $s4, 28($zero)	store number of passes through bubble sort
0xac130020,	  //	sw $s3, 32($zero) 	store number of passes through insertion sort
0xac020024,	  //	sw $v0, 36($zero)	if 0, the two arrays are equal, 1 they are not equal
0x8c0804b0,	  //	lw $t0, 1200($zero)	Using the data in memory location 300 to verify sort
0xac080018,	  //	sw $t0, 24($zero)
0x8c001018,	  //	flush cache to memory to view via memory
0x8c00101c,	  //	flush cache to memory to view via memory
0x8c001020,	  //	flush cache to memory to view via memory
0x8c001024,	  //	flush cache to memory to view via memory
0x00000000,	  //	nop
0x00000000,	  //	nop
0x00000000,	  //	nop
0x00000000,	  //	nop
0x00000008,   // 	jr	to PC = 0, indicating end of program
0x00000000,	  //	nop
0x00000000,	  //	nop
0x00000000,	  //	nop
0x00000000,	  //	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x0a000000,
0x00000d00,    // 	nop
0x00b00000,    // 	nop
0x000000e0,
0x00030000,    // 	nop
0x0f000000,    // 	nop
0x0000f000,
0xffffffff,
0x0a0a0000,
0x0f0f0f0f,
0xf0f0f0f0,
0x00010020,
0xf0000000,
0x0aa0bab0,
0xa5a5a5a5,
0x5a5a5a5a,
0x0acccbaa,
0x00000000,
0xffffffff,
0xf034a04a,
0xaaaaaaaa,
0xbbbbbbbb,
0xcccccccc,
0xdddddddd,
0xeeeeeeee,
0x99999999,
0x88888888,
0x77777777,
0x66666666,
0x55555555,
0x44444444,
0x33333333,
0x22222222,
0x11111111,
0x00000001,
0x00000010,
0x00000100,
0x00001000,
0x00010000,
0x00100000,
0x01000000,
0x10000000,
0x00000002,
0x00000020,
0x00000200,
0x00002000,
0x00020000,
0x00200000,
0x02000000,
0x20000000,
0x00000003,
0x00000030,
0x00000300,
0x00003000,
0x00030000,
0x00300000,
0x03000000,
0x30000000,
0x00000004,
0x00000040,
0x00000400,
0x00004000,
0x00040000,
0x00400000,
0x04000000,
0x40000000,
0x00000005,
0x00000050,
0x00000500,
0x00005000,
0x00050000,
0x00500000,
0x05000000,
0x50000000,
0x00000006,
0x00000060,
0x00000600,
0x00006000,
0x00060000,
0x00600000,
0x06000000,
0x60000000,
0x00000007,
0x00000070,
0x00000700,
0x00007000,
0x00070000,
0x00700000,
0x07000000,
0x70000000,
0x00000008,
0x00000080,
0x00000800,
0x00008000,
0x00080000,
0x00800000,
0x08000000,
0x80000000,
0x00000009,
0x00000090,
0x00000900,
0x00009000,
0x00090000,
0x00900000,
0x09000000,
0x90000000,
0x0000000a,
0x000000a0,
0x00000a00,
0x0000a000,
0x000a0000,
0x00a00000,
0x0a000000,
0xa0000000,
0x0000000b,
0x000000b0,
0x00000b00,
0x0000b000,
0x000b0000,
0x00b00000,
0x0b000000,
0xb0000000,
0x0000000c,
0x000000c0,
0x00000c00,
0x0000c000,
0x000c0000,
0x00c00000,
0x0c000000,
0xc0000000,
0x0000000d,
0x000000d0,
0x00000d00,
0x0000d000,
0x000d0000,
0x00d00000,
0x0d000000,
0xd0000000,
0x0000000e,
0x000000e0,
0x00000e00,
0x0000e000,
0x000e0000,
0x00e00000,
0x0e000000,
0xe0000000,
0x0000000f,
0x000000f0,
0x00000f00,
0x0000f000,
0x000f0000,
0x00f00000,
0x0f000000,
0xf0000000,
0x00000011,
0x00001100,
0x00110000,
0x22000000,
0x00000022,
0x00002200,
0x00220000,
0x22000000,
0x00000033,
0x00003300,
0x00330000,
0x33000000,
0x00000044,
0x00004400,
0x00440000,
0x44000000,
0x00000055,
0x00005500,
0x00550000,
0x55000000,
0x00000066,
0x00006600,
0x00660000,
0x66000000,
0x00000077,
0x00007700,
0x00770000,
0x77000000,
0x00000088,
0x00008800,
0x00880000,
0x88000000,
0x00000099,
0x00009900,
0x00990000,
0x99000000,
0x000000aa,
0x0000aa00,
0x00aa0000,
0xaa000000,
0x000000bb,
0x0000bb00,
0x00bb0000,
0xbb000000,
0x000000cc,
0x0000cc00,
0x00cc0000,
0xcc000000,
0x000000dd,
0x0000dd00,
0x00dd0000,
0xdd000000,
0x000000ee,
0x0000ee00,
0x00ee0000,
0xee000000,
0x000000ff,
0x0000ff00,
0x00ff0000,
0xff000000,
0x00000101,
0x00001010,
0x00010100,
0x00101000,
0x01010000,
0x30300000,
0x00000303,
0x00003030,
0x00030300,
0x00303000,
0x03030000,
0x30300000,
0x00000202,
0x00002020,
0x00020200,
0x00202000,
0x02020000,
0x20200000,
0x00000505,
0x00005050,
0x00050500,
0x00505000,
0x05050000,
0x50500000,
0x00000606,
0x00006060,
0x00060600,
0x00606000,
0x06060000,
0x60600000,
0x00000f0f,
0x0000f0f0,
0x000f0f00,
0x00f0f000,
0x0f0f0000,
0x0000e000,
};
#endif




// Program 2 
#ifdef RUN_PROG_2
unsigned int program_image[MEMORY_SIZE] = {
0x00000898,	// $sp = 2200
0x00000898,	// $fp = 2200
0x00000320,	// data segment pointer used by the program
0x00000000,
0x00000000,
0x00000078,	// $pc = 120
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x80a70000,   // 	lb	a3,0(a1)
0x10e0001e,   // 	beqz	a3,4008b0 <ceasar_encrypt+0x80>
0x24a50001,   // 	addiu	a1,a1,1
0x30e200ff,   // 	andi	v0,a3,0xff
0x2443ff9f,   // 	addiu	v1,v0,-97
0x306300ff,   // 	andi	v1,v1,0xff
0x2c63001a,   // 	sltiu	v1,v1,26
0x10600009,   // 	beqz	v1,400874 <ceasar_encrypt+0x44>
0x2443ffbf,   // 	addiu	v1,v0,-65
0x00441021,   // 	addu	v0,v0,a0
0x304200ff,   // 	andi	v0,v0,0xff
0x7c023c20,   // 	seb	a3,v0
0x28e3007b,   // 	slti	v1,a3,123
0x1460000d,   // 	bnez	v1,40089c <ceasar_encrypt+0x6c>
0x2442ffe6,   // 	addiu	v0,v0,-26
0x1000000b,   // 	b	40089c <ceasar_encrypt+0x6c>
0x7c023c20,   // 	seb	a3,v0
0x306300ff,   // 	andi	v1,v1,0xff
0x2c63001a,   // 	sltiu	v1,v1,26
0x10600007,   // 	beqz	v1,40089c <ceasar_encrypt+0x6c>
0x00441021,   // 	addu	v0,v0,a0
0x304300ff,   // 	andi	v1,v0,0xff
0x7c033c20,   // 	seb	a3,v1
0x28e2005b,   // 	slti	v0,a3,91
0x14400002,   // 	bnez	v0,40089c <ceasar_encrypt+0x6c>
0x2463ffe6,   // 	addiu	v1,v1,-26
0x7c033c20,   // 	seb	a3,v1
0xa0c70000,   // 	sb	a3,0(a2)
0x80a70000,   // 	lb	a3,0(a1)
0x24c60001,   // 	addiu	a2,a2,1
0x14e0ffe4,   // 	bnez	a3,40083c <ceasar_encrypt+0xc>
0x24a50001,   // 	addiu	a1,a1,1
0x03e00008,   // 	jr	ra
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x80a70000,   // 	lb	a3,0(a1)
0x10e0001e,   // 	beqz	a3,400938 <ceasar_decrypt+0x80>
0x24a50001,   // 	addiu	a1,a1,1
0x30e200ff,   // 	andi	v0,a3,0xff
0x2443ff9f,   // 	addiu	v1,v0,-97
0x306300ff,   // 	andi	v1,v1,0xff
0x2c63001a,   // 	sltiu	v1,v1,26
0x10600009,   // 	beqz	v1,4008fc <ceasar_decrypt+0x44>
0x2443ffbf,   // 	addiu	v1,v0,-65
0x00441023,   // 	subu	v0,v0,a0
0x304200ff,   // 	andi	v0,v0,0xff
0x7c023c20,   // 	seb	a3,v0
0x28e30061,   // 	slti	v1,a3,97
0x1060000d,   // 	beqz	v1,400924 <ceasar_decrypt+0x6c>
0x2442001a,   // 	addiu	v0,v0,26
0x1000000b,   // 	b	400924 <ceasar_decrypt+0x6c>
0x7c023c20,   // 	seb	a3,v0
0x306300ff,   // 	andi	v1,v1,0xff
0x2c63001a,   // 	sltiu	v1,v1,26
0x10600007,   // 	beqz	v1,400924 <ceasar_decrypt+0x6c>
0x00441023,   // 	subu	v0,v0,a0
0x304300ff,   // 	andi	v1,v0,0xff
0x7c033c20,   // 	seb	a3,v1
0x28e20041,   // 	slti	v0,a3,65
0x10400002,   // 	beqz	v0,400924 <ceasar_decrypt+0x6c>
0x2463001a,   // 	addiu	v1,v1,26
0x7c033c20,   // 	seb	a3,v1
0xa0c70000,   // 	sb	a3,0(a2)
0x80a70000,   // 	lb	a3,0(a1)
0x24c60001,   // 	addiu	a2,a2,1
0x14e0ffe4,   // 	bnez	a3,4008c4 <ceasar_decrypt+0xc>
0x24a50001,   // 	addiu	a1,a1,1
0x03e00008,   // 	jr	ra
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x80860000,   // 	lb	a2,0(a0)
0x10c0000f,   // 	beqz	a2,400984 <compare_string+0x44>
0x00a04025,   // 	move	t0,a1
0x24840001,   // 	addiu	a0,a0,1
0x00003825,   // 	move	a3,zero
0x24020001,   // 	li	v0,1
0x81030000,   // 	lb	v1,0(t0)
0x00661826,   // 	xor	v1,v1,a2
0x0003100b,   // 	movn	v0,zero,v1
0x24e70001,   // 	addiu	a3,a3,1
0x00e01825,   // 	move	v1,a3
0x80860000,   // 	lb	a2,0(a0)
0x25080001,   // 	addiu	t0,t0,1
0x14c0fff8,   // 	bnez	a2,400958 <compare_string+0x18>
0x24840001,   // 	addiu	a0,a0,1
0x10000004,   // 	b	400990 <compare_string+0x50>
0x00a32821,   // 	addu	a1,a1,v1
0x00001825,   // 	move	v1,zero
0x24020001,   // 	li	v0,1
0x00a32821,   // 	addu	a1,a1,v1
0x80a30000,   // 	lb	v1,0(a1)
0x03e00008,   // 	jr	ra
0x0003100b,   // 	movn	v0,zero,v1
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x27bdfc50,   // 	addiu	sp,sp,-944
0xafbf03ac,   // 	sw	ra,940(sp)
0xafb203a8,   // 	sw	s2,936(sp)
0xafb103a4,   // 	sw	s1,932(sp)
0xafb003a0,   // 	sw	s0,928(sp)
0x8e020000,   // 	lw	v0,0(s0)
0xafa2039c,   // 	sw	v0,924(sp)
0x27b20144,   // 	addiu	s2,sp,324
0x02403025,   // 	move	a2,s2
0x3c020000,   // 	lui	v0,0x00
0x8c050008,	  //	lw $a1, 8($zero)	0x8c450008,   // 	lw	a1,8(v0)
0x24040005,   // 	li	a0,5
0x0c00000a,   // 	jal	10 <ceasar_encrypt>
0x00000000,   // 	nop
0x27b10270,   // 	addiu	s1,sp,624
0x02203025,   // 	move	a2,s1
0x02402825,   // 	move	a1,s2
0x24040005,   // 	li	a0,5
0x0c000032,   // 	jal	50 <ceasar_decrypt>
0x00000000,   // 	nop
0x02202825,   // 	move	a1,s1
0x8c040008,	  //	lw $a0, 8($zero) 0x27a40018,   // 	addiu	a0,sp,24
0x0c00005a,   // 	jal	90 <compare_string>
0x00000000,   // 	nop
0x8fa4039c,   // 	lw	a0,924(sp)
0x8e030000,   // 	lw	v1,0(s0)
0x10830004,   // 	beq	a0,v1,400a34 <main+0x98>
0x8fbf03ac,   // 	lw	ra,940(sp)
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x8c080384,   // 	lw $t0, 900($zero)
0x8c090690,   // 	lw $t1, 1680($zero)
0x8c0a07bc,   // 	lw $t2, 1980($zero)
0xac08001c,   // 	sw $t0, 28($zero)
0xac090020,   // 	sw $t1, 32($zero)
0xac0a0024,   // 	sw $t2, 36($zero)
0xac020018,   // 	sw $v0, 24($zero)
0x8c08041c,   // 	lw $t0, 1052($zero)
0x8c090420,   // 	lw $t1, 1056($zero)
0x8c0a0424,   // 	lw $t2, 1060($zero)
0x8c080418,   // 	lw $t0, 1048($zero)
0x8fb203a8,   // 	lw	s2,936(sp)
0x8fb103a4,   // 	lw	s1,932(sp)
0x8fb003a0,   // 	lw	s0,928(sp)
0x00000008,   // 	jr	to PC = $zero to end program
0x27bd03b0,   // 	addiu	sp,sp,944
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x00000000,   // 	nop
0x44757269,   // 	Duri
0x6e672061,   // 	ng a
0x20666577,   // 	 few
0x20646179,   // 	 day
0x73206861,   // 	s ha
0x6c74206e,   // 	lt n
0x65617220,   // 	ear
0x5665736f,   // 	Veso
0x6e74696f,   // 	ntio
0x20666f72,   // 	 for
0x20746865,   // 	 the
0x2070726f,   // 	 pro
0x76697369,   // 	visi
0x6f6e206f,   // 	on o
0x6620636f,   // 	f co
0x726e2061,   // 	rn a
0x6e64206f,   // 	nd o
0x74686572,   // 	ther
0x20737570,   // 	 sup
0x706c6965,   // 	plie
0x73206120,   // 	s a
0x70616e69,   // 	pani
0x63206172,   // 	c ar
0x6f736520,   // 	ose
0x66726f6d,   // 	from
0x20696e71,   // 	 inq
0x75697269,   // 	uiri
0x6573206d,   // 	es m
0x61646520,   // 	ade
0x6279206f,   // 	by o
0x75722074,   // 	ur t
0x726f6f70,   // 	roop
0x7320616e,   // 	s an
0x64207265,   // 	d re
0x6d61726b,   // 	mark
0x73207574,   // 	s ut
0x74657265,   // 	tere
0x64206279,   // 	d by
0x20476175,   // 	 Gau
0x6c732061,   // 	ls a
0x6e642074,   // 	nd t
0x72616465,   // 	rade
0x72732077,   // 	rs w
0x686f2061,   // 	ho a
0x66666972,   // 	ffir
0x6d656420,   // 	med
0x74686174,   // 	that
0x20746865,   // 	 the
0x20476572,   // 	 Ger
0x6d616e73,   // 	mans
0x20776572,   // 	 wer
0x65206d65,   // 	e me
0x6e206f66,   // 	n of
0x2061206d,   // 	 a m
0x69676874,   // 	ight
0x79206672,   // 	y fr
0x616d6520,   // 	ame
0x616e6420,   // 	and
0x616e2069,   // 	an i
0x6e637265,   // 	ncre
0x6469626c,   // 	dibl
0x65207661,   // 	e va
0x6c6f7572,   // 	lour
0x20616e64,   // 	and
0x20736b69,   // 	ski
0x6c6c2061,   // 	ll a
0x74206172,   // 	t ar
0x6d73204a,   // 	ms J
0x756c6975,   // 	uliu
0x73204361,   // 	s Ca
0x65736172,   // 	esar
0x00000000,   // 	nop
};
#endif

