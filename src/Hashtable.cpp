#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Hashtable.h"

#define ASSERT(CONDITION__, ERROR__)                                                  \
    do                                                                                \
    {                                                                                 \
        if(!(CONDITION__))                                                            \
        {                                                                             \
            fprintf(stderr, "\x1b[31;1mERROR:\x1b[0m %s\n", #ERROR__);                \
                                                                                      \
            return ERROR__;                                                           \
        }                                                                             \
    } while(0)                                                                        \

int hashtable_ctor(Hashtable* tbl, size_t size, uint32_t (*hash_func)(const void* data))
{
    assert(tbl && hash_func);
    assert(size > 0);

    tbl->data = (List*) calloc(size, sizeof(List));
    ASSERT(tbl->data, HASHTABLE_BAD_ALLOC);

    for(size_t iter = 0; iter < size; iter++)
        ASSERT(!list_ctor(&tbl->data[iter]), HASHTABLE_BAD_LIST);

    tbl->hash_func = hash_func;
    tbl->size      = size;
    
    return HASHTABLE_NOERR;
}

void hashtable_dtor(Hashtable* tbl)
{
    assert(tbl);

    if(tbl->data)
    {
        for(size_t iter = 0; iter < tbl->size; iter++)
        {
            list_dtor(&tbl->data[iter]);
        }
        
        free(tbl->data);
    }

    tbl->hash_func = nullptr;
    tbl->data      = nullptr;
    tbl->size      = 0;
    
    return;
}

int hashtable_verify(Hashtable* tbl)
{
    assert(tbl);

    assert(tbl->data && tbl->hash_func);
    assert(tbl->size > 0);

    for(size_t iter = 0; iter < tbl->size; iter++)
        ASSERT(!list_verify(&tbl->data[iter]), HASHTABLE_BAD_LIST);

    return HASHTABLE_NOERR;
}

int hashtable_insert(Hashtable* tbl, const char* key, ht_elem_t value)
{
    assert(tbl && key);
    assert(tbl->hash_func && tbl->size);

    uint32_t hash = tbl->hash_func(key);

    List* list  = &tbl->data[hash % tbl->size];  // change to shift?
    int tmp_pos = LIST_HEADER_POS;    
    int mask    = 0;
    char* ptr   = nullptr;
    
    // __m256i avx_key = _mm256_lddqu_si256((const __m256i*) key);
    __m256i avx_key;
    asm
    (
        "vlddqu %[key], %[avx_key]\n\t"
        :[avx_key] "=x"  (avx_key)
        :[key]     "m"   (*key)
    );
    
    // do 
    // {
    //     tmp_pos = list->node_arr[tmp_pos].next;
    //     mask    = _mm256_movemask_epi8(_mm256_cmpeq_epi8(avx_key, _mm256_load_si256(&list->node_arr[tmp_pos].data.key)));
    // }
    // while(tmp_pos > LIST_HEADER_POS && mask != -1);
    do
    {
        tmp_pos = list->node_arr[tmp_pos].next;
        ptr = (char*) &list->node_arr[tmp_pos].data.key;

        asm
        (
            "vmovdqa %[key], %%ymm0\n\t"
            "vpcmpeqb %[avx_key], %%ymm0, %%ymm0\n\t"
            "vpmovmskb %%ymm0, %[mask]\n\t"
            :[mask]    "=r" (mask)
            :[avx_key] "x"  (avx_key),
             [key]     "m"  (*ptr)
            : "ymm0"
        );
    }
    while(tmp_pos > LIST_HEADER_POS && mask != -1);
    
    ASSERT(tmp_pos >= LIST_HEADER_POS, HASHTABLE_BAD_LIST); // tmp_pos < -1 -> error code

    if(tmp_pos != LIST_HEADER_POS)
        return HASHTABLE_ALREADY_INSERTED;
    
    list_elem_t elem = {};
    elem.obj = value;

    // elem.key = avx_key;
    asm
    (
        "vmovdqu %[avx_key], %[key]\n\t"
        :[key]     "=m" (elem.key)
        :[avx_key] "x"  (avx_key)
    );

    tmp_pos = list_push_back(list, elem);
    ASSERT(tmp_pos >= LIST_HEADER_POS, HASHTABLE_BAD_LIST);

    return HASHTABLE_NOERR;
}

int hashtable_delete(Hashtable* tbl, const char* key)
{
    assert(tbl && key);
    assert(tbl->hash_func && tbl->size);

    uint32_t hash = tbl->hash_func(key);

    List* list  = &tbl->data[hash % tbl->size];  // change to shift?
    int tmp_pos = list->node_arr[LIST_HEADER_POS].next;
    int mask    = 0;
    char* ptr   = nullptr;

    // __m256i avx_key = _mm256_lddqu_si256((const __m256i*) key);
    __m256i avx_key;
    asm
    (
        "vlddqu %[key], %[avx_key]\n\t"
        :[avx_key] "=x"  (avx_key)
        :[key]     "m"   (*key)
    );

    // do 
    // {
    //     tmp_pos = list->node_arr[tmp_pos].next;
    //     mask    = _mm256_movemask_epi8(_mm256_cmpeq_epi8(avx_key, _mm256_load_si256(&list->node_arr[tmp_pos].data.key)));
    // }
    // while(tmp_pos > LIST_HEADER_POS && mask != -1);
    do
    {
        tmp_pos = list->node_arr[tmp_pos].next;
        ptr = (char*) &list->node_arr[tmp_pos].data.key;

        asm
        (
            "vmovdqa %[key], %%ymm0\n\t"
            "vpcmpeqb %[avx_key], %%ymm0, %%ymm0\n\t"
            "vpmovmskb %%ymm0, %[mask]\n\t"
            :[mask]    "=r" (mask)
            :[avx_key] "x"  (avx_key),
             [key]     "m"  (*ptr)
            : "ymm0"
        );
    }
    while(tmp_pos > LIST_HEADER_POS && mask != -1);
    
    ASSERT(tmp_pos >= LIST_HEADER_POS, HASHTABLE_BAD_LIST); // tmp_pos < -1 -> error code

    if(tmp_pos == LIST_HEADER_POS)
        return HASHTABLE_NOTFOUND;

    tmp_pos = list_delete(list, tmp_pos);
    ASSERT(tmp_pos >= LIST_HEADER_POS || tmp_pos == LIST_EMPTY, HASHTABLE_BAD_LIST); // tmp_pos < -1 -> error code

    return HASHTABLE_NOERR;
}

int hashtable_find(Hashtable* tbl, const char* key, ht_elem_t* retvalue)
{
    assert(tbl && key && retvalue);
    assert(tbl->hash_func && tbl->size);

    uint32_t hash = tbl->hash_func(key);

    List* list = &tbl->data[hash % tbl->size];  // change to shift?
    int tmp_pos = list->node_arr[LIST_HEADER_POS].next;
    int mask    = 0;
    char* ptr   = nullptr;

    // __m256i avx_key = _mm256_lddqu_si256((const __m256i*) key);
    __m256i avx_key;
    asm
    (
        "vlddqu %[key], %[avx_key]\n\t"
        :[avx_key] "=x"  (avx_key)
        :[key]     "m"   (*key)
    );

    // do 
    // {
    //     tmp_pos = list->node_arr[tmp_pos].next;
    //     mask    = _mm256_movemask_epi8(_mm256_cmpeq_epi8(avx_key, _mm256_load_si256(&list->node_arr[tmp_pos].data.key)));
    // }
    // while(tmp_pos > LIST_HEADER_POS && mask != -1);
    do
    {
        tmp_pos = list->node_arr[tmp_pos].next;
        ptr = (char*) &list->node_arr[tmp_pos].data.key;

        asm
        (
            "vmovdqa %[key], %%ymm0\n\t"
            "vpcmpeqb %[avx_key], %%ymm0, %%ymm0\n\t"
            "vpmovmskb %%ymm0, %[mask]\n\t"
            :[mask]    "=r" (mask)
            :[avx_key] "x"  (avx_key),
             [key]     "m"  (*ptr)
            : "ymm0"
        );
    }
    while(tmp_pos > LIST_HEADER_POS && mask != -1);
    
    ASSERT(tmp_pos >= LIST_HEADER_POS, HASHTABLE_BAD_LIST); // tmp_pos < -1 -> error code

    if(tmp_pos == LIST_HEADER_POS)
        return HASHTABLE_NOTFOUND;
    
    *retvalue = list->node_arr[tmp_pos].data.obj;

    return HASHTABLE_NOERR;
}
