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

#define POOL_SLAB16 0
#define POOL_SLAB32 1
#define POOL_SLAB64 2
#define POOL_SLAB_COUNT 3

// slab 16 bits mask
#define SLAB_INDEX_BITN 6
#define SLAB_INDEX_SHIFT 0
#define SLAB_COUNT_BITN 6
#define SLAB_COUNT_SHIFT 6
#define SLAB_ID_BITN 4
#define SLAB_ID_SHIFT 12

#define SLAB_INDEX_MASK ((1U << SLAB_INDEX_BITN) - 1) << SLAB_INDEX_SHIFT
#define SLAB_COUNT_MASK ((1U << SLAB_COUNT_BITN) - 1) << SLAB_COUNT_SHIFT
#define SLAB_ID_MASK ((1U << SLAB_ID_BITN) - 1) << SLAB_ID_SHIFT

// slab pseudo functions todo change to functions auto find related slab size
//#define slabGetPtr(pool, index)  (void *)&(pool.slabs[index][0])
//#define slabGetFirst(group) (group & SLAB_POS_MASK)
//#define slabGetSize(group) ((group & SLAB_COUNT_MASK) >> SLAB_COUNT_SHIFT) * SLAB_SIZE


// todo add slabFree()
// pool.slab16.mem[index]
// pool.slab32.slab_size


typedef struct
{
	uint64_t bitmaps[POOL_SLAB_COUNT];
	const uint8_t slabs_count[POOL_SLAB_COUNT];
	const uint8_t slabs_size[POOL_SLAB_COUNT];

	uint8_t slabs16[SLAB16_COUNT][SLAB16_SIZE];
	uint8_t slabs32[SLAB32_COUNT][SLAB32_SIZE];
	uint8_t slabs64[SLAB64_COUNT][SLAB64_SIZE];

} slabs_pool_t;


int slabFindFree(slabs_pool_t *pool, uint8_t slabn, uint8_t count)
{
    uint64_t mask = (1ULL << count) - 1;  // mask count bits=1

    for (int i = 0; i <= (pool->slabs_count[slabn] - count); i++)
    {
        if ( ((pool->bitmaps[slabn] >> i) & mask) == 0 )
            return i;  // find
    }
    return -1; // not find
}

uint16_t slabAlloc(slabs_pool_t *pool, uint8_t slabn, uint16_t size)
{
    //if(size > SLAB_GROUP_SIZE_MAX){ return -1;}

    uint16_t count = (size + pool->slabs_size[slabn] -1) / pool->slabs_size[slabn];
    uint8_t index = slabFindFree( pool, slabn, count);


    if (index >= 0)
    {
        printf("[slabAlloc] count = %i pos = %i\n",count, index);
        pool->bitmaps[slabn] |= ((1ULL << count) - 1) << index;

        uint16_t output =( (slabn << SLAB_ID_SHIFT) & SLAB_ID_MASK) |
			( (count << SLAB_COUNT_SHIFT) & SLAB_COUNT_MASK) |
			( (index << SLAB_INDEX_SHIFT) & SLAB_INDEX_MASK);

        printf("[slabAlloc] output = 0x%04x\n",output);
        return output;
    }
    return -1;
}

void expandPtr(const uint16_t ptr)
{

	uint16_t index = ( ptr & SLAB_INDEX_MASK) >> SLAB_INDEX_SHIFT;
	uint16_t count = ( ptr & SLAB_COUNT_MASK) >> SLAB_COUNT_SHIFT;
	uint16_t id = (ptr & SLAB_ID_MASK) >> SLAB_ID_SHIFT;

	printf("[expandPtr] id = %i count = %i index = %i\n", id, count, index);
}


int main(void)
{

	slabs_pool_t pool =
	{
		.slabs_count = {SLAB16_COUNT, SLAB32_COUNT, SLAB64_COUNT},
		.slabs_size = {SLAB16_SIZE, SLAB32_SIZE, SLAB64_SIZE}
	};

	// init
	for(int i = 0; i < POOL_SLAB_COUNT;i++)
	{
		pool.bitmaps[i] = 0;
	}

	// test

	printf("test mask : \n\t0x%04x\n\t0x%04x\n\t0x%04x\n", SLAB_INDEX_MASK, SLAB_COUNT_MASK, SLAB_ID_MASK);


	pool.bitmaps[POOL_SLAB32] = 0x0f0f55aa; // dummy data for test purpose

	uint16_t group = 0;
	printf("pool bitmap = %lx\n\n", pool.bitmaps[POOL_SLAB32]);

	group = slabAlloc(&pool, POOL_SLAB32, 4*SLAB32_SIZE);
	//printf("test alloc 1 4*32 : pos = %i %p\n", slabGetFirst(group),slabGetPtr(pool, group));
	printf("pool bitmap = %lx\n", pool.bitmaps[POOL_SLAB32]);
	expandPtr(group);

	printf("\n");

	group = slabAlloc(&pool, POOL_SLAB32, 4*SLAB32_SIZE);
	//printf("test alloc 2 4*32 : pos = %i %p\n", slabGetFirst(group),slabGetPtr(pool, group));
	printf("pool bitmap = %lx\n", pool.bitmaps[POOL_SLAB32]);
	expandPtr(group);

	return 0;
}
