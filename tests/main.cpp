#include <assert.h>

#include "../src/Hashtable.h"
#include "../utils/logs/logs.h"
#include "../utils/hash.h"
#include "../utils/stats.h"

void print_ht_elem(FILE* stream, const ht_elem_t* elem)
{
    fprintf(stream, "%lg", *elem);
}

int main()
{
    logs_init("test_main.html");
    hashtable_dump_init(logs_get(), &print_ht_elem);

    Hashtable ht = {};
    hashtable_ctor(&ht, 1024, &qhashfnv1_64);

    int err = 0;
    char buffer[20] = {};
    for(int iter = 10; iter < 1024*16 && !err; iter++)
    {
        sprintf(buffer, "Hello %d", iter);
        hashtable_insert(&ht, buffer, iter);
    }

    ht_elem_t val = 0;
    err = hashtable_find(&ht, "Hello 10", &val);
    //LOG$("find: %d, %p", err, val);

    hashtable_dump(&ht, "Dump");

    FILE* stream = fopen("collisions.csv", "w");
    assert(stream);
    stats_collisions(&ht, stream);
    fclose(stream);

    hashtable_dtor(&ht);

    return 0;
}