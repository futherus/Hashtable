#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>
#include "../include/List/src/List.h"
#include "hash.h"

struct Hashtable
{
    uint32_t (*hash_func)(const void* key) = nullptr;

    List*  data   = nullptr;
    size_t size   = 0;

    size_t elem_count = 0;
};

enum hashtable_err
{
    HASHTABLE_NOERR             = 0,
    HASHTABLE_NOTFOUND          = 1,
    HASHTABLE_ALREADY_INSERTED  = 2,
    HASHTABLE_BAD_ALLOC         = 3,
    HASHTABLE_BAD_LIST          = 4,
};

int  hashtable_ctor(Hashtable* tbl, size_t size, uint32_t (*hash_func)(const void* key));
void hashtable_dtor(Hashtable* tbl);

int  hashtable_insert (Hashtable* tbl, const char* key, ht_elem_t value);
int  hashtable_delete (Hashtable* tbl, const char* key);
int  hashtable_find   (Hashtable* tbl, const char* key, ht_elem_t* retvalue);
int  hashtable_visitor(Hashtable* tbl, void(*func)(const ht_elem_t*));

void hashtable_dump_init(FILE* dumpstream, void(*print_func)(FILE*, const ht_elem_t*));
void hashtable_dump(Hashtable* tbl, const char* msg = nullptr);

int  hashtable_verify(Hashtable* tbl);

#endif // HASHTABLE_H