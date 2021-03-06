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

int hashtable_ctor(Hashtable* tbl, size_t size, uint32_t (*hash_func)(const void* key))
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
            list_dtor(&tbl->data[iter]);
        
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

    List* list = &tbl->data[hash % tbl->size];
    list_elem_t tmp = {};
    int tmp_pos = list_front(list, &tmp);
    
    while(tmp_pos > LIST_HEADER_POS && memcmp(key, tmp.key, KEY_SIZE) != 0)
        tmp_pos = list_next(list, tmp_pos, &tmp);

    ASSERT(tmp_pos >= LIST_HEADER_POS, HASHTABLE_BAD_LIST); // tmp_pos < -1 -> error code

    if(tmp_pos != LIST_HEADER_POS)
        return HASHTABLE_ALREADY_INSERTED;
    
    list_elem_t elem = {};
    elem.obj = value;
    memcpy(elem.key, key, KEY_SIZE);

    tmp_pos = list_push_back(list, elem);
    ASSERT(tmp_pos >= LIST_HEADER_POS, HASHTABLE_BAD_LIST);

    return HASHTABLE_NOERR;
}

int hashtable_delete(Hashtable* tbl, const char* key)
{
    assert(tbl && key);
    assert(tbl->hash_func && tbl->size);

    uint32_t hash = tbl->hash_func(key);

    List* list = &tbl->data[hash % tbl->size];
    list_elem_t tmp = {};
    int tmp_pos = list_front(list, &tmp);

    while(tmp_pos > LIST_HEADER_POS && memcmp(key, tmp.key, KEY_SIZE) != 0)
        tmp_pos = list_next(list, tmp_pos, &tmp);

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

    List* list = &tbl->data[hash % tbl->size];
    list_elem_t tmp = {};
    int tmp_pos = list_front(list, &tmp);

    while(tmp_pos > LIST_HEADER_POS && memcmp(key, tmp.key, KEY_SIZE) != 0)
        tmp_pos = list_next(list, tmp_pos, &tmp);

    ASSERT(tmp_pos >= LIST_HEADER_POS, HASHTABLE_BAD_LIST); // tmp_pos < -1 -> error code

    if(tmp_pos == LIST_HEADER_POS)
        return HASHTABLE_NOTFOUND;
    
    *retvalue = tmp.obj;

    return HASHTABLE_NOERR;
}
