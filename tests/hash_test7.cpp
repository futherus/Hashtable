#include <assert.h>

#include "../src/Hashtable.h"
#include "../utils/logs/logs.h"
#include "../src/hash.h"
#include "../utils/stats.h"
#include "../utils/text.h"

// static void print_ht_elem(FILE* stream, const ht_elem_t* elem)
// {
//     fprintf(stream, "%lu", *elem);
// }

int main()
{
    logs_init("hash_test7.html");
    // hashtable_dump_init(logs_get(), nullptr);
    
    Hashtable ht = {};
    int err = hashtable_ctor(&ht, 256, &crc32);
    if(err)
        return err;

    Text text = {};
    err = text_ctor(&text, "../tests/test_collisions.txt");
    if(err)
    {
        hashtable_dtor(&ht);
        return err;
    }

    // err = text_print(&text, "hash_test7_text.txt");
    // if(err)
    // {
    //     text_dtor(&text);
    //     hashtable_dtor(&ht);
    //     return err;
    // }

    LOG$("Words amount: %lu\n", text.index_arr_size);

    struct  // __attribute__((packed))
    {
        char buffer[KEY_SIZE] = {};
        char dummy = 0;
    } buf;

    for(size_t iter = 0; iter < text.index_arr_size; iter++)
    {
        size_t size = text.index_arr[iter].size;
        if(size > KEY_SIZE)
            size = KEY_SIZE;
        
        memcpy(buf.buffer, text.index_arr[iter].begin, size);

        err = hashtable_insert(&ht, buf.buffer, iter);
        if(err && err != HASHTABLE_ALREADY_INSERTED)
        {
            hashtable_dtor(&ht);
            text_dtor(&text);

            return err;
        }

        LOG$("Iteration: %lu, %d", iter, err);
        
        memset(buf.buffer, 0, KEY_SIZE);
    }    
    
    err = 0;

    LOG$("Inserted");

    // hashtable_dump(&ht);

    FILE* stream = fopen("crc32.csv", "w");
    if(!stream)
        return 1;
    stats_collisions(&ht, stream);
    err = fclose(stream);

    LOG$("Destructors");
    hashtable_dtor(&ht);
    text_dtor(&text);

    return err;
}