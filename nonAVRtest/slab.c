#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define SLAB_COUNT 64
#define SLAB_SIZE 32

// todo add slabFree()

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

	// !! do not compile !!
	/*uint8_t *test_group = slab16Alloc(pool_16);
	*test_group = slab32GroupAlloc(pool_32, size);
	uint8_t *test_data = slabGetPtr(test_group);

	uint8_t **test;
	test = slabAlloc(16);
	*test = slabAlloc(32);
	slabGroupAlloc(test, 128);

	uint8_t group = slabAlloc(128);
	uint8_t *p_data = slabGetPtr(group);

	use(test->pointer);

	(&pool.slabs[index][0])*/

	typedef struct
	{
		const uint8_t a;
		const uint8_t b;
	} const_test_t;

	const_test_t foo;

	uint8_t *wa = &foo.b;
	*wa=8;

	printf("const test b=%i\n",foo.b);
	// init
	pool.mem_barrier_hight = &pool.slabs[SLAB_COUNT-1][SLAB_SIZE-1];
	pool.mem_barrier_low = &pool.slabs[0][0];
	pool.bitmap = 0;

	// test
	printf("memory barrier low : %p hight = %p diff = %i\n", pool.mem_barrier_low,
			pool.mem_barrier_hight, (pool.mem_barrier_hight-pool.mem_barrier_low));

	pool.bitmap = 0x0f0f55aa;

	slab_t **slab;

	slab=malloc(sizeof(*slab));
	*slab=malloc(sizeof(slab));

	int test = slabAlloc(&pool, slab, 4);
	printf("test alloc 1 4n: pos = %i %p\n", (*slab)->slab_first,(*slab)->pointer);

	test = slabAlloc(&pool, slab, 4);
	printf("test alloc 2 4n: pos = %i %p\n", (*slab)->slab_first, (*slab)->pointer);
	return 0;
}
