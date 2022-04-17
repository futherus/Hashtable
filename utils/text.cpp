#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>

#include "text.h"
#include "logs/logs.h"

static void set_index_arr(char* buffer, Index* index_arr, size_t index_arr_size)
{
    assert(buffer && index_arr);

    size_t iter = 0;
    int ch = 0;
    char* first_symbol = nullptr;
    char* last_symbol  = nullptr;

    for(size_t line_num = 0; line_num < index_arr_size; line_num++)
    {
        while(isspace(buffer[iter])) //skips ws until first symbol
            iter++;

        first_symbol = &buffer[iter];

        while(true)
        {
            ch = buffer[iter];

            if(isspace(ch) || ch == '\0')
                break;

            if(!isspace(ch))
                last_symbol = &buffer[iter]; //save last !ws symbol
            iter++;
        }

        index_arr[line_num].begin = first_symbol;
        index_arr[line_num].size  = (size_t) (last_symbol + 1 - first_symbol);

        first_symbol = nullptr;
        last_symbol  = nullptr;
    }
}

static int create_index_arr(Index** dst_index_arr, size_t* dst_index_arr_size, char* buffer)
{
    assert(buffer);
    assert(dst_index_arr_size);

    size_t iter     = 0;
    size_t line_num = 0;
    bool nline_flag = 0;

    while(true)
    {
        int ch = buffer[iter];

        if((isspace(ch) || ch == '\0') && nline_flag == 0)
        {
            nline_flag = 1;
            line_num++;
        }

        if(!isspace(ch))
            nline_flag = 0;

        if(ch == '\0')
            break;

        iter++;
    }

    Index* index_arr = (Index*) calloc(line_num, sizeof(Index));

    if(index_arr == nullptr)
    {
        return 1;
    }

    set_index_arr(buffer, index_arr, line_num);

    *dst_index_arr_size = line_num;
    *dst_index_arr = index_arr;

    return 0;
}

static int create_buffer(char** dst_buffer, size_t* dst_buffer_size, const char* file_name)
{
    LOG$("check file");
    struct stat file_stat = {};
    if(stat(file_name, &file_stat) == -1)
        return 1;

    size_t file_size = (size_t) file_stat.st_size;

    LOG$("calloc");
    char* buffer = nullptr;
    buffer = (char*) calloc(file_size + 1, sizeof(char)); // + 1 for '\0'
    if(buffer == nullptr)
        return 1;

    LOG$("open file");
    errno = 0;
    FILE* istream = fopen(file_name, "r");
    if(errno != 0)
    {
        free(buffer);
        return 1;
    }

    LOG$("read file");
    errno = 0;
    size_t file_read = fread(buffer, sizeof(char), file_size, istream);
    if(errno != 0)
    {
        free(buffer);
        fclose(istream);
        return 1;
    }

    if(fclose(istream) == EOF)
    {
        free(buffer);
        return 1;
    }

    LOG$("realloc");
    buffer = (char*) realloc(buffer, (file_read + 1) * sizeof(char)); //+ 1 for '\0'
    if(buffer == nullptr)
    {
        free(buffer);
        return 1;
    }

    buffer[file_read] = '\0';

    *dst_buffer = buffer;
    *dst_buffer_size = file_read;

    return 0;
}

int text_ctor(Text* text, const char* file_name)
{
    assert(text && file_name);

    char* buffer = nullptr;
    size_t buffer_size = 0;

    LOG$("Creating buffer");
    if(create_buffer(&buffer, &buffer_size, file_name))
        return 1;
    
    LOG$("Creating indecies");
    Index* index_arr = nullptr;
    size_t index_arr_size = 0;
    if(create_index_arr(&index_arr, &index_arr_size, buffer))
    {
        free(buffer);
        return 1;
    }

    text->buffer         = buffer;
    text->index_arr      = index_arr;
    text->buffer_size    = buffer_size;
    text->index_arr_size = index_arr_size;

    return 0;
}

void text_dtor(Text* text)
{
    free(text->index_arr);
    free(text->buffer);
}

static int print_line(char* begin, size_t size, FILE* ostream)
{
    assert(begin && ostream);

    if(fwrite(begin, sizeof(char), size, ostream) != size)
        return 1;

    if(fwrite("\n", sizeof(char), 1, ostream) != 1)
        return 1;

    return 1;
}

int text_print(Text* text, const char* file_name)
{
    assert(text && file_name);

	FILE* ostream = fopen(file_name, "w");
	if(ostream == nullptr)
		return 1;

	for(size_t iter = 0; iter < text->index_arr_size; iter++)
    {
        int msg = print_line(text->index_arr[iter].begin, text->index_arr[iter].size, ostream);
            if(msg != 1)
                return msg;
    }

	if(fclose(ostream) == EOF)
		return 1;

	return 0;
}
