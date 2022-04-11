#include <assert.h>

#include "stats.h"

void stats_collisions(Hashtable* tbl, FILE* stream)
{
    assert(tbl && stream);

    fprintf(stream, "collisions\n");
    for(size_t iter = 0; iter < tbl->size; iter++)
    {
        fprintf(stream, "%d\n", tbl->data[iter].size);
    }
}