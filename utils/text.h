#ifndef TEXT_H
#define TEXT_H

#include <stddef.h>

struct Index
{
    char*  begin = nullptr;
    size_t size  = 0;
};

struct Text
{
    size_t buffer_size  = 0;
    char* buffer        = nullptr;

    size_t index_arr_size = 0;
    Index* index_arr      = nullptr;
};

int text_ctor(Text*, char* infile_name);

void text_dtor(Text*);

#endif // TEXT_H
