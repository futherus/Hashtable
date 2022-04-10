#include <stdio.h>
#include <assert.h>

#include "Hashtable.h"

static FILE* DUMP_STREAM = nullptr;
static void (*PRINT_ELEM)(FILE* dumpstream, const elem_t* elem) = nullptr;

#define PRINT(format, ...) fprintf(stream, format, ##__VA_ARGS__)

static const char DATA_IS_NULL_MSG[] = "\n                                                                                            "
                                       "\n  DDDDDD      A    TTTTTTTTT    A         IIIII   SSSSS     N    N  U     U  L      L       "
                                       "\n  D     D    A A       T       A A          I    S          NN   N  U     U  L      L       "
                                       "\n  D     D   A   A      T      A   A         I     SSSS      N N  N  U     U  L      L       "
                                       "\n  D     D  AAAAAAA     T     AAAAAAA        I         S     N  N N  U     U  L      L       "
                                       "\n  DDDDDD  A       A    T    A       A     IIIII  SSSSS      N   NN   UUUUU   LLLLLL LLLLLL  "
                                       "\n                                                                                            ";

static const char HFILL[] = "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";

static void print_list(List* list, FILE* stream)
{
    assert(list && stream);

    if(list->node_arr == nullptr)
        return;

    int pos = list_front(list);

    while(pos != LIST_HEADER_POS)
    {
        PRINT(" `");
        PRINT_ELEM(stream, &list->node_arr[pos].data);
        PRINT("`");

        pos = list->node_arr[pos].next;
    }

    PRINT("\n");
}

void hashtable_dump(Hashtable* tbl, const char* msg)
{
    assert(tbl);
    assert(DUMP_STREAM);
    FILE* stream = DUMP_STREAM;

    PRINT("<span class = \"title\">%s</span>\n", HFILL);
    
    if(msg)
        PRINT("<span class = \"title\"> %s </span>\n", msg);
    
    if(tbl->data == nullptr)
    {
        PRINT("<span class = \"error\">%s\n</span>", DATA_IS_NULL_MSG);
        PRINT("<span class = \"title\">\n%s\n</span>", HFILL);
        return;
    }

    PRINT("size: %5lu\n", tbl->size);

    for(size_t iter = 0; iter < tbl->size; iter++)
    {
        PRINT("\n0x%.5lx -> ", iter);
        print_list(&tbl->data[iter], stream);
    }
    
    PRINT("<span class = \"title\">\n%s\n</span>", HFILL);
}
    
#undef PRINT

void hashtable_dump_init(FILE* dumpstream, void(*print_elem)(FILE*, const elem_t*))
{
    assert(dumpstream);

    PRINT_ELEM = print_elem;

    list_dump_init(dumpstream, print_elem);

    DUMP_STREAM = dumpstream;
}
