#include <stdlib.h>

#include "statistics.h"

void *
vec_heap_p_reserve_exact (void *vec, int reserve MEM_STAT_DECL)
{
	return malloc(10000);
}
