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

// slab pseudo functions
#define slabPtrGetIndex(ptr) ( ptr & SLAB_INDEX_MASK) >> SLAB_INDEX_SHIFT
#define slabPtrGetCount(ptr) ( ptr & SLAB_COUNT_MASK) >> SLAB_COUNT_SHIFT
#define slabPtrGetId(ptr) (ptr & SLAB_ID_MASK) >> SLAB_ID_SHIFT

#define slabPtrPrint(ptr) printf("[slabPtrPrint] id = %i count = %i index = %i\n", slabPtrGetId(ptr), slabPtrGetCount(ptr), slabPtrGetIndex(ptr))
#define slabPtrGetPtr(ptr) (void *)pool.slabs_ptr[slabPtrGetId(ptr)] + (pool.slabs_size[slabPtrGetId(ptr)] * slabPtrGetIndex(ptr))
#define slabPtrGetSize(ptr) pool.slabs_size[slabPtrGetId(ptr)] * slabPtrGetCount(ptr)



typedef struct
{
	uint64_t bitmaps[POOL_SLAB_COUNT];
	const uint8_t slabs_count[POOL_SLAB_COUNT];
	const uint8_t slabs_size[POOL_SLAB_COUNT];
	const uint8_t *slabs_ptr[POOL_SLAB_COUNT];

	uint8_t slabs16[SLAB16_COUNT][SLAB16_SIZE];
	uint8_t slabs32[SLAB32_COUNT][SLAB32_SIZE];
	uint8_t slabs64[SLAB64_COUNT][SLAB64_SIZE];

} slabs_pool_t;


uint8_t slabFindFree(slabs_pool_t *pool, uint8_t slabn, uint8_t count)
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
    if(size == 0 ){ return 0;}
    if(size > pool->slabs_size[slabn] * 4){return 0;}

    uint16_t count = (size + pool->slabs_size[slabn] -1) / pool->slabs_size[slabn]; // rounded up blocks
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
    return 0;
}


int main(void)
{

	// init
	slabs_pool_t pool =
	{
		.slabs_count = {SLAB16_COUNT, SLAB32_COUNT, SLAB64_COUNT},
		.slabs_size = {SLAB16_SIZE, SLAB32_SIZE, SLAB64_SIZE},
		.slabs_ptr = {pool.slabs16, pool.slabs32, pool.slabs64}
	};

	for(int i = 0; i < POOL_SLAB_COUNT;i++)
	{
		pool.bitmaps[i] = 0;
	}

	// test

	printf("test mask : \n\t0x%04x\n\t0x%04x\n\t0x%04x\n", SLAB_INDEX_MASK, SLAB_COUNT_MASK, SLAB_ID_MASK);

	pool.bitmaps[POOL_SLAB32] = 0x0f0f55aa; // dummy data for test purpose

	printf("slabs32 bitmap = %lx\n\n", pool.bitmaps[POOL_SLAB32]);

	uint16_t group1 = slabAlloc(&pool, POOL_SLAB32, 4*SLAB32_SIZE);
	//printf("test alloc 1 4*32 : pos = %i %p\n", slabGetFirst(group),slabGetPtr(pool, group));
	printf("slabs32 bitmap = %lx\n", pool.bitmaps[POOL_SLAB32]);
	slabPtrPrint(group1);
	printf("group1 ptr,size 0x%p,0x%x\n", slabPtrGetPtr(group1), slabPtrGetSize(group1));

	printf("\n");

	uint16_t group2 = slabAlloc(&pool, POOL_SLAB32, 4*SLAB32_SIZE);
	//printf("test alloc 2 4*32 : pos = %i %p\n", slabGetFirst(group),slabGetPtr(pool, group));
	printf("slabs32 bitmap = %lx\n", pool.bitmaps[POOL_SLAB32]);
	slabPtrPrint(group2);
	printf("group2 ptr,size 0x%p,0x%x\n", slabPtrGetPtr(group2), slabPtrGetSize(group2));

	uint8_t *ptr2;
	uint8_t *ptr1;

	ptr1 = slabPtrGetPtr(group1);
	ptr2 = slabPtrGetPtr(group2);

	printf("\ndiff group2 group1 : 0x%lx \n", ptr2-ptr1);
	return 0;
}
