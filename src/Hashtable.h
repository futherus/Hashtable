#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>
#include "../include/List/src/List.h"

struct Hashtable
{
    uint64_t (*hash_func)(const void* key, size_t len) = nullptr;

    List*  data = nullptr;
    size_t size = 0;
};

enum hashtable_err
{
    HASHTABLE_NOERR = 0,
    HASHTABLE_NOTFOUND,
    HASHTABLE_ALREADY_INSERTED,
    HASHTABLE_BAD_ALLOC,
    HASHTABLE_BAD_LIST,
    HASHTABLE_NOT_INIT,
    HASHTABLE_BAD_SIZE,
};

int  hashtable_ctor(Hashtable* tbl, size_t size, uint64_t (*hash_func)(const void* key, size_t len));
void hashtable_dtor(Hashtable* tbl);

void hashtable_dump_init(FILE* dumpstream, void(*print_func)(FILE*, const elem_t*));
void hashtable_dump(Hashtable* tbl, const char* msg = nullptr);

int  hashtable_insert(Hashtable* tbl, const char* key, void* value);
int  hashtable_delete(Hashtable* tbl, const char* key);
int  hashtable_find  (Hashtable* tbl, const char* key, void** retvalue);

int  hashtable_verify(Hashtable* tbl);

#endif // HASHTABLE_H