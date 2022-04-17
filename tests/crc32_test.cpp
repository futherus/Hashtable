#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "../utils/text.h"
#include "../src/hash.h"

int main()
{
    Text text = {};
    
    int err = text_ctor(&text, "../tests/test_collisions.txt");
    if(err)
    {
        return err;
    }

    char buffer[32] = {};

    uint32_t out = 0;
    for(size_t iter = 0; iter < text.index_arr_size; iter++)
    {
        size_t size = text.index_arr[iter].size;
        if(size > 32)
            size = 32;
        
        memcpy(buffer, text.index_arr[iter].begin, size);

        out += crc32(buffer);
    }

    printf("crc32: %d\n", out);
}