#include <immintrin.h>

#include "Hashtable.h"

uint32_t crc32(const void* data)
{
    const uint32_t* buf = (const uint32_t*) data;

    uint32_t hash = 0;
    for(size_t iter = 0; iter < KEY_SIZE / sizeof(uint32_t); iter++)
        hash = _mm_crc32_u32(hash, *(buf + iter));

    return hash;
}