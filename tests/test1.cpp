#include <assert.h>

#include "../src/Hashtable.h"
#include "../utils/logs/logs.h"
#include "../utils/hash.h"
#include "../utils/stats.h"
#include "../utils/text.h"

void print_ht_elem(FILE* stream, const ht_elem_t* elem)
{
    fprintf(stream, "%lg", *elem);
}

static uint64_t hash_test(const void*, size_t)
{
    return 1;
}

int main()
{
    logs_init("test1.html");

    Hashtable ht = {};
    int err = hashtable_ctor(&ht, 256, &hash_test);
    if(err)
        return err;

    Text text = {};
    err = text_ctor(&text, "../tests/test_text.txt");
    if(err)
    {
        hashtable_dtor(&ht);
        return err;
    }

    LOG$("Words amount: %lu\n", text.index_arr_size);

    for(size_t iter = 0; iter < text.index_arr_size; iter++)
    {
        err = hashtable_insert(&ht, text.index_arr[iter].begin, iter);
        if(err)
        {
            hashtable_dtor(&ht);
            text_dtor(&text);

            return err;
        }

        if(iter % 10000 == 0)
        {
            LOG$("Iterations: %lu", iter);
        }
    }

    LOG$("Inserted");

    FILE* stream = fopen("collisions1.csv", "w");
    if(!stream)
        return 1;
    stats_collisions(&ht, stream);
    err = fclose(stream);

    LOG$("Destructors");
    hashtable_dtor(&ht);
    text_dtor(&text);

    return err;
}