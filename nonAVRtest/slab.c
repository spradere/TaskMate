#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// slab size & count
#define SLAB16_COUNT 64
#define SLAB16_SIZE 16
#define SLAB16_ID 0

#define SLAB32_COUNT 64
#define SLAB32_SIZE 32
#define SLAB32_ID 1

#define SLAB64_COUNT 32
#define SLAB64_SIZE 64
#define SLAB64_ID 2

#define SLAB_POOL_COUNT 3


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

// slab pseudo functions
#define slabGetIndex(group) (( group & SLAB_INDEX_MASK) >> SLAB_INDEX_SHIFT)
#define slabGetCount(group) (( group & SLAB_COUNT_MASK) >> SLAB_COUNT_SHIFT)
#define slabGetId(group) ((group & SLAB_ID_MASK) >> SLAB_ID_SHIFT)

#define slabPrintGroup(group) printf("[slabPrintGroup] id = %i count = %i index = %i\n", slabGetId(group), slabGetCount(group), slabGetIndex(group))
#define slabPrintBitmap(group) printf("[slabPrintBitmap] id = %i bitmap = %016lx\n",slabGetId(group), pool.bitmaps[slabGetId(group)])
#define slabGetPtr(group) ((void *)pool.slabs_ptr[slabGetId(group)] + (pool.slabs_size[slabGetId(group)] * slabGetIndex(group)))
#define slabGetSize(group) (pool.slabs_size[slabGetId(group)] * slabGetCount(group))


typedef struct
{
	uint64_t bitmaps[SLAB_POOL_COUNT];
	const uint8_t slabs_count[SLAB_POOL_COUNT];
	const uint8_t slabs_size[SLAB_POOL_COUNT];
	const uint8_t *slabs_ptr[SLAB_POOL_COUNT];

	uint8_t slabs16[SLAB16_COUNT][SLAB16_SIZE];
	uint8_t slabs32[SLAB32_COUNT][SLAB32_SIZE];
	uint8_t slabs64[SLAB64_COUNT][SLAB64_SIZE];

} slabs_pool_t;


uint8_t slabFindFree(slabs_pool_t *pool, uint8_t slab_id, uint8_t count)
{
    uint64_t mask = (1ULL << count) - 1;  // mask count bits=1

    for (int i = 0; i <= (pool->slabs_count[slab_id] - count); i++)
    {
        if ( ((pool->bitmaps[slab_id] >> i) & mask) == 0 )
            return i;  // find
    }
    return -1; // not find
}

void slabFree(slabs_pool_t *pool, uint16_t group)
{
	uint64_t mask = ((1ULL << slabGetCount(group) ) -1) << slabGetIndex(group) ;
	pool->bitmaps[slabGetId(group)] &= ~mask;
}

uint16_t slabAlloc(slabs_pool_t *pool, uint8_t slab_id, uint16_t size)
{
    if(size == 0 ){ return 0;}
    if(size > pool->slabs_size[slab_id] * 4){return 0;}

    uint16_t count = (size + pool->slabs_size[slab_id] -1) / pool->slabs_size[slab_id]; // rounded up blocks
    uint8_t index = slabFindFree( pool, slab_id, count);

    if (index >= 0)
    {
        printf("[slabAlloc] count = %i pos = %i\n",count, index);
        pool->bitmaps[slab_id] |= ((1ULL << count) - 1) << index;

        uint16_t output =( (slab_id << SLAB_ID_SHIFT) & SLAB_ID_MASK) |
			( (count << SLAB_COUNT_SHIFT) & SLAB_COUNT_MASK) |
			( (index << SLAB_INDEX_SHIFT) & SLAB_INDEX_MASK);
        return output;
    }
    return 0;
}


int main(void)
{

	// init
	slabs_pool_t pool =
	{
		.slabs_count = {SLAB16_COUNT, SLAB32_COUNT, SLAB64_COUNT},
		.slabs_size = {SLAB16_SIZE, SLAB32_SIZE, SLAB64_SIZE},
		.slabs_ptr = {(uint8_t*)pool.slabs16, (uint8_t*)pool.slabs32, (uint8_t*)pool.slabs64}
	};

	for(int i = 0; i < SLAB_POOL_COUNT;i++)
	{
		pool.bitmaps[i] = 0;
	}

	// test
	pool.bitmaps[SLAB32_ID] = 0x0f0f55aa; // dummy data for test purpose
	printf("dummy bitmap = %016lx\n\n", pool.bitmaps[SLAB32_ID]);

	uint16_t group1 = slabAlloc(&pool, SLAB32_ID, 4*SLAB32_SIZE);
	slabPrintBitmap(group1);
	slabPrintGroup(group1);
	printf("group1 ptr,size %p,0x%x\n", slabGetPtr(group1), slabGetSize(group1));

	uint16_t group2 = slabAlloc(&pool, SLAB32_ID, 4*SLAB32_SIZE);
	slabPrintBitmap(group2);
	slabPrintGroup(group2);
	printf("group2 ptr,size %p,0x%x\n", slabGetPtr(group2), slabGetSize(group2));

	uint8_t *ptr2;
	uint8_t *ptr1;

	ptr1 = slabGetPtr(group1);
	ptr2 = slabGetPtr(group2);

	printf("\ndiff ptr2 ptr1 : 0x%lx \n", ptr2-ptr1);

	for(int i = 0; i<slabGetSize(group1); i++)
	{
		ptr1[i]=0;
		*(ptr1+i)=0;
	}

	slabFree(&pool, group2);
	slabPrintBitmap(group2);
	slabFree(&pool, group1);
	slabPrintBitmap(group1);
	return 0;
}
