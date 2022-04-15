#include <assert.h>
#include <immintrin.h>

#include "../src/Hashtable.h"
#include "../utils/logs/logs.h"
#include "../src/hash.h"
#include "../utils/stats.h"
#include "../utils/text.h"

static void print_ht_elem(FILE* stream, const ht_elem_t* elem)
{
    fprintf(stream, "%lu", *elem);
}

int main()
{
    logs_init("test1.html");

    Hashtable ht = {};
    int err = hashtable_ctor(&ht, 256, &crc32);
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

    char buffer[KEY_SIZE] = {};

    for(size_t iter = 0; iter < text.index_arr_size; iter++)
    {
        size_t size = text.index_arr[iter].size;
        if(size > KEY_SIZE)
            size = KEY_SIZE;
        
        memcpy(buffer, text.index_arr[iter].begin, size);

        err = hashtable_insert(&ht, buffer, iter);
        if(err && err != HASHTABLE_ALREADY_INSERTED)
        {
            hashtable_dtor(&ht);
            text_dtor(&text);

            return err;
        }

        LOG$("Insertion: %lu, %s (%lu), %d", iter, buffer, text.index_arr[iter].size, err);
        
        memset(buffer, 0, KEY_SIZE);
    }

    LOG$("Inserted");

    for(size_t iter = 0; iter < text.index_arr_size; iter++)
    {
        size_t size = text.index_arr[iter].size;
        if(size > KEY_SIZE)
            size = KEY_SIZE;
        
        memcpy(buffer, text.index_arr[iter].begin, size);

        err = hashtable_delete(&ht, buffer);
        LOG$("Deletion: %lu, %s (%lu), %d", iter, buffer, text.index_arr[iter].size, err);
        if(err && err != HASHTABLE_NOTFOUND)
        {
            hashtable_dtor(&ht);
            text_dtor(&text);

            return err;
        }

        
        memset(buffer, 0, KEY_SIZE);
    }

    err = 0;

    // FILE* stream = fopen("collisions1.csv", "w");
    // if(!stream)
    //     return 1;
    // stats_collisions(&ht, stream);
    // err = fclose(stream);

    LOG$("Destructors");
    hashtable_dtor(&ht);
    text_dtor(&text);

    LOG$("End of main");
    return err;
}