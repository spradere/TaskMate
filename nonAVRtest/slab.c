#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define SLAB_COUNT 64
#define SLAB_SIZE 32


typedef struct
{
	uint8_t slabs[SLAB_COUNT][SLAB_SIZE];
	uint8_t *mem_barrier_low;
	uint8_t *mem_barrier_hight;
	uint64_t bitmap;
} slab_pool_t;


typedef struct
{
	uint8_t *pointer;
	uint8_t slab_first;
	uint8_t slab_count;

}slab_t;


int slabFindFree(uint64_t bitmap, int n)
{
    if (n < 1 || n > 4){return -1;}

	// todo change 64 -> 8 ?
    uint64_t mask = (1ULL << n) - 1;  // mask n bits=1

    for (int i = 0; i <= 64 - n; i++)
    {
        if ( ((bitmap >> i) & mask) == 0 )
            return i;  // find
    }

    return -1; // not find
}

int slabAlloc(slab_pool_t *pool, slab_t **slab, int n)
{
    int pos = slabFindFree( pool->bitmap, n);
    if (pos >= 0)
    {
        printf("[slabAlloc] pos = %i\n",pos);
        pool->bitmap |= ((1ULL << n) - 1) << pos;

        (*slab)->pointer = &(pool->slabs[pos][0]);
		(*slab)->slab_first = pos;
        (*slab)->slab_count = n;
        return pos;
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
	printf("memory barrier low : %p hight = %p diff = %i\n", pool.mem_barrier_low,
			pool.mem_barrier_hight, (pool.mem_barrier_hight-pool.mem_barrier_low));

	pool.bitmap = 0x0f0f55aa;

	slab_t **slab;

	slab=malloc(sizeof(slab));
	*slab=malloc(sizeof(*slab));

	int test = slabAlloc(&pool, slab, 4);
	printf("test alloc 1 4n: pos = %i\n", (*slab)->slab_first);

	test = slabAlloc(&pool, slab, 4);
	printf("test alloc 2 4n: pos = %i\n", (*slab)->slab_first);
	return 0;
}
