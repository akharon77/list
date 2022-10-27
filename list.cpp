#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

void ListCtor(List *lst, int32_t size)
{
    ASSERT(lst  != NULL);
    ASSERT(size > 0);

    lst->size = size;

    lst->free = 1;

    lst->buf = (Node*) calloc(size + 1, sizeof(Node));

    ASSERT(lst->buf != NULL);

    for (int32_t i = 1; i < size + 1; ++i)
        lst->buf[i] = 
            {
                .val  = -1,
                .next = i + 1,
                .prev = i - 1
            };

    lst->buf[0] = 
        {
            .val  = 0,
            .next = 0,
            .prev = 0 
        };
}

void ListDtor(List *lst)
{
    ASSERT(lst != NULL);
    
    free(lst->buf);
}

int32_t ListInsertBefore(List *lst, int32_t val, int32_t anch)
{
    ASSERT(lst != NULL);
    ASSERT(anch < lst->size + 1 && anch > -1);

    int32_t npos = lst->free;
    lst->free = lst->buf[lst->free].next;

    lst->buf[npos].val  = val;

    int32_t anch_prev = lst->buf[anch].prev;

    lst->buf[npos].next = anch;
    lst->buf[npos].prev = anch_prev;

    lst->buf[anch_prev].next = npos;
    lst->buf[anch].prev      = npos;

    return npos;
}

int32_t ListInsertAfter(List *lst, int32_t val, int32_t anch)
{
    return ListInsertBefore(lst, val, lst->buf[anch].next);
}

void ListErase(List *lst, int32_t anch)
{
    ASSERT(lst != NULL);
    ASSERT(anch < lst->size + 1 && anch > -1);

    lst->buf[lst->free].prev = anch;

    int32_t prev_anch = lst->buf[anch].prev,
            next_anch = lst->buf[anch].next;

    lst->buf[prev_anch].next = next_anch;
    lst->buf[next_anch].prev = prev_anch;

    lst->buf[anch] = 
        {
            .val = -1,
            .next = lst->free,
            .prev = -1
        };

    lst->free = anch;
}

int32_t ListPushBack(List *lst, int32_t val)
{
    ASSERT(lst != NULL);

    ListInsertBefore(lst, val, 0);
}

int32_t ListPushFront(List *lst, int32_t val)
{
    ASSERT(lst != NULL);

    ListInsertAfter(lst, val, 0);
}

void ListPopBack(List *lst)
{
    ASSERT(lst != NULL);

    ListErase(lst, ListGetTail(lst));
}

void ListPopFront(List *lst)
{
    ASSERT(lst != NULL);

    ListErase(lst, ListGetHead(lst));
}

int32_t ListGetHead(List *lst)
{
    ASSERT(lst != NULL);

    return lst->buf[0].next;
}

int32_t ListGetTail(List *lst)
{
    ASSERT(lst != NULL);

    return lst->buf[0].prev;
}

int32_t ListGetNext(List *lst, int32_t anch)
{
    ASSERT(lst != NULL);

    return lst->buf[anch].next;
}

int32_t ListGetPrev(List *lst, int32_t anch)
{
    ASSERT(lst != NULL);

    return lst->buf[anch].prev;
}

void ListPrint(List *lst)
{
    ASSERT(lst != NULL);

    int v = ListGetHead(lst);
    while (v != 0)
    {
        printf("anch = %d, val = %d\n", v, lst->buf[v].val);
        v = ListGetNext(lst, v);
    }
    printf("\n");
}

uint32_t ListStatus(List *lst)
{
    
}
