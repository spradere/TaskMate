#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// slab size & count
#define SLAB16_COUNT 64
#define SLAB16_SIZE 16
#define SLAB32_COUNT 64
#define SLAB32_SIZE 32
#define SLAB64_COUNT 32
#define SLAB64_SIZE 64

// slab 16 bits mask
#define SLAB_INDEX_BITN 6
#define SLAB_INDEX_SHIFT 0
#define SLAB_COUNT_BITN 6
#define SLAB_COUNT_SHIFT 6
#define SLAB_POOLID_BITN 4
#define SLAB_POOLID_SHIFT 12

#define SLAB_INDEX_MASK ((1U << SLAB_INDEX_BITN) - 1) << SLAB_INDEX_SHIFT
#define SLAB_COUNT_MASK ((1U << SLAB_COUNT_BITN) - 1) << SLAB_COUNT_SHIFT
#define SLAB_POOLID_MASK ((1U << SLAB_POOLID_BITN) - 1) << SLAB_POOLID_SHIFT

// old def
#define SLAB_SIZE 32
#define SLAB_COUNT 64

#define SLAB_GROUP_SIZE_MAX 256
#define SLAB_POS_MASK 0x00ff
#define SLAB_COUNT_MASK 0xff00
#define SLAB_COUNT_SHIFT 8

// slab pseudo functions todo change to functions auto find related slab size
#define slabGetPtr(pool, index)  (void *)&(pool.slabs[index][0])
#define slabGetFirst(group) (group & SLAB_POS_MASK)
#define slabGetSize(group) ((group & SLAB_COUNT_MASK) >> SLAB_COUNT_SHIFT) * SLAB_SIZE


// todo add slabFree()
// pool.slab16.mem[index]
// pool.slab32.slab_size

typedef struct
{
	uint8_t slabs[SLAB_COUNT][SLAB_SIZE];
	uint8_t *mem_barrier_low;
	uint8_t *mem_barrier_hight;
	uint64_t bitmap;
} slab_pool_t;


int slabFindFree(uint64_t bitmap, uint8_t n)
{
    uint64_t mask = (1ULL << n) - 1;  // mask n bits=1

    for (int i = 0; i <= SLAB_COUNT - n; i++)
    {
        if ( ((bitmap >> i) & mask) == 0 )
            return i;  // find
    }

    return -1; // not find
}

uint16_t slabAlloc(slab_pool_t *pool, uint16_t size)
{
    if(size > SLAB_GROUP_SIZE_MAX){ return -1;}

    uint16_t count = (size + SLAB_SIZE -1) / SLAB_SIZE;
    uint8_t pos = slabFindFree( pool->bitmap, count);


    if (pos >= 0)
    {
        printf("[slabAlloc] count = %i pos = %i\n",count, pos);
        pool->bitmap |= ((1ULL << count) - 1) << pos;

        uint16_t output = count;
        output = (output << SLAB_COUNT_SHIFT) | pos;
        return output;
    }
    return -1;
}


int main(void)
{

	slab_pool_t pool;

	// init
	pool.mem_barrier_hight = &pool.slabs[SLAB_COUNT-1][SLAB_SIZE-1];
	pool.mem_barrier_low = &pool.slabs[0][0];
	pool.bitmap = 0;

	// test

	printf("test mask : \n\t0x%04x\n\t0x%04x\n\t0x%04x\n", SLAB_INDEX_MASK, SLAB_SIZE_MASK, SLAB_POOLID_MASK);

	printf("memory barrier low : %p hight = %p diff = %li\n", pool.mem_barrier_low,
			pool.mem_barrier_hight, (pool.mem_barrier_hight-pool.mem_barrier_low));

	pool.bitmap = 0x0f0f55aa; // dummy data for test purpose

	uint16_t group = 0;
	printf("pool bitmap = %lx\n\n", pool.bitmap);

	group = slabAlloc(&pool, 4*SLAB_SIZE);
	printf("test alloc 1 4*32 : pos = %i %p\n", slabGetFirst(group),slabGetPtr(pool, group));
	printf("pool bitmap = %lx\n\n", pool.bitmap);

	group = slabAlloc(&pool, 4*SLAB_SIZE);
	printf("test alloc 2 4*32 : pos = %i %p\n", slabGetFirst(group),slabGetPtr(pool, group));
	printf("pool bitmap = %lx\n\n", pool.bitmap);

	return 0;
}
