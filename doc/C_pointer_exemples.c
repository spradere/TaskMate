// Summary for pointer to functions

/* Aliases of pointer :
 * 	(*foo).member <-> foo->member
 * 	(array+i) <-> array[i]
 * 	&array[0] <-> array
 * 	fx() <-> (*fx)()    //useless
 * 	int *bar[x] -> int **bar
*/

/* Incrementation tricky trap
 *	*p++   // same as *(p++): increment pointer, and dereference unincremented address
 * 	*++p   // same as *(++p): increment pointer, and dereference incremented address
 * 	++*p   // same as ++(*p): dereference pointer, and increment the value it points to
 *	(*p)++ // dereference pointer, and post-increment the value it points to
*/

#include <stdio.h>
#include <stdlib.h>

void foo(int b);
void bar(int *b);
void baz(int **b);

int main(void)
{
	int a = 10;
	printf("level 1 coppy\n[%p] = %i\n",&a, a);
	foo(a);
	printf("[%p] = %i\n\n",&a, a);

	int b = 10;
	printf("level 2 pointer\n[%p] = %i\n",&b, b);
	bar(&b);
	printf("[%p] = %i\n\n",&b, b);

	int *c;
	c = malloc(sizeof(c));
	*c = 10;
	printf("level 2 pointer\n[%p] = %i\n",c, *c);
	bar(c);
	printf("[%p] = %i\n\n",c, *c);

	int *d;
	d = malloc(sizeof(d));
	*d = 10;
	printf("level 3 pointer to pointer\n[%p] = %i\n",d, *d);
	baz(&d);
	printf("[%p] = %i\n\n",d, *d);

	int **e;
	e = malloc(sizeof(*e));
	*e = malloc(sizeof(e));
	**e = 10;
	printf("level 3 pointer to pointer\n[%p] = %i\n",*e, **e);
	baz(e);
	printf("[%p] = %i\n\n",*e, **e);

return 0;
}

void foo(int b)
{
	b = 20;
}

void bar(int *b)
{
	*b = 20;
}

void baz(int **b)
{
	*b = malloc(sizeof(b));
	**b = 20;
}
