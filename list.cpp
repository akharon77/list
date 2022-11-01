#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "list.h"
#include "list_debug.h"

const int32_t  ROOT      =  0;

void        ListCtor_          (List *lst, int32_t size 
                                ON_DEBUG
                                (
                                ,int32_t line,
                                 const char *name,
                                 const char* funcname,
                                 const char *filename
                                ))
{
    ASSERT(lst  != NULL);
    ASSERT(size > 0);

    lst->size = 0;
    lst->cap  = size;

    lst->free = 1;

    lst->buf = (Node*) calloc(size + 1, sizeof(Node));

    ASSERT(lst->buf != NULL);

    for (int32_t i = 0; i < size + 1; ++i)
        lst->buf[i] = 
            {
                .val   = -1,
                .next  = i + 1,
                .prev  = -1
            };

    lst->buf[size].next = -1;

    lst->buf[ROOT] = 
        {
            .val   = 0,
            .next  = ROOT,
            .prev  = ROOT
        };

    ON_DEBUG
    (
        lst->info =
            {
                .line     = line,
                .name     = name + 1,
                .funcname = funcname,
                .filename = filename
            };
    )

    ListOk(lst);
    ListLog(lst);
}

void ListDtor(List *lst)
{
    ASSERT(lst != NULL);
    ListOk(lst);
    ListLog(lst);

    free(lst->buf);
    ListFillStructurePosion(lst);
}

int32_t ListInsertBefore(List *lst, int32_t val, int32_t anch)
{
    ASSERT(lst != NULL);
    ASSERT(anch < lst->cap + 1 && anch > -1);
    ListOk(lst);

    if (anch != ROOT)
        lst->is_linearized = false;

    if (lst->size + 1 > lst->cap)
        ListRealloc(lst, lst->cap * 2, false);

    int32_t npos = lst->free;
    lst->free = lst->buf[lst->free].next;

    int32_t anch_prev = lst->buf[anch].prev;

    lst->buf[npos] = 
        {
            .val   = val,
            .next  = anch,
            .prev  = anch_prev
        };

    lst->buf[anch_prev].next = npos;
    lst->buf[anch].prev      = npos;
    
    ++lst->size;

    ListOk(lst);
    ListLog(lst);

    return npos;
}

void ListLinearize(List *lst)
{
    ASSERT(lst != NULL);
    ListOk(lst);

    Node *new_buf = (Node*) calloc(lst->cap + 1, sizeof(Node));
    ASSERT(new_buf != NULL);

    new_buf[ROOT] = 
        {
            .val   = 0,
            .next  = 1,
            .prev  = lst->size
        };

    int32_t vertex = ListGetHead(lst), i = 1;
    while (vertex != ROOT)
    {
        new_buf[i] = 
            {
                .val  = ListGetValue(lst, vertex),
                .next = i + 1,
                .prev = i - 1
            };

        vertex = ListGetNext(lst, vertex);
        ++i;
    }

    free(lst->buf);

    for (int32_t anch = lst->size + 1; anch < lst->cap + 1; ++anch)
        new_buf[anch] = 
            {
                .val  = -1,
                .next = anch + 1 <= lst->cap ? anch + 1 : -1,
                .prev = -1
            };

    new_buf[lst->size].next = ROOT;

    lst->buf = new_buf;
    if (lst->size == lst->cap)
        lst->free = -1;
    else
        lst->free = lst->size + 1;

    ListOk (lst);
    ListLog(lst);
}

void ListRealloc(List *lst, int32_t new_cap, bool linear)
{
    ASSERT(lst != NULL);
    ListOk(lst);

    if (linear || new_cap < lst->cap)
        ListLinearize(lst);

    Node *new_buf = (Node*) realloc(lst->buf, (new_cap + 1) * sizeof(Node));

    ASSERT(new_buf != NULL);
    
    for (int32_t anch = lst->cap + 1; anch < new_cap + 1; ++anch)
        new_buf[anch] = 
            {
                .val  = -1,
                .next = anch + 1 <= new_cap ? anch + 1 : -1,
                .prev = -1
            };

    lst->buf  = new_buf;

    if (lst->size == new_cap)
        lst->free = -1;
    else
        lst->free = lst->cap + 1;

    lst->cap  = new_cap;

    ListOk(lst);
    ListLog(lst);
}

int32_t ListInsertAfter(List *lst, int32_t val, int32_t anch)
{
    return ListInsertBefore(lst, val, lst->buf[anch].next);
}

void ListErase(List *lst, int32_t anch)
{
    ASSERT(lst != NULL);
    ASSERT(anch < lst->size + 1 && anch > -1);
    ListOk(lst);

    if (lst->size < lst->cap / 4)
        ListRealloc(lst, lst->size, true);

    if (anch != ListGetTail(lst))
        lst->is_linearized = false;

    int32_t prev_anch = lst->buf[anch].prev,
            next_anch = lst->buf[anch].next;

    lst->buf[prev_anch].next = next_anch;
    lst->buf[next_anch].prev = prev_anch;

    lst->buf[anch] = 
        {
            .val   = -1,
            .next  = lst->free,
            .prev  = -1
        };

    lst->free = anch;

    --lst->size;

    ListOk(lst);
    ListLog(lst);
}

int32_t ListPushBack(List *lst, int32_t val)
{
    ListInsertBefore(lst, val, ROOT);
}

int32_t ListPushFront(List *lst, int32_t val)
{
    ListInsertAfter(lst, val, ROOT);
}

void ListPopBack(List *lst)
{
    ListErase(lst, ListGetTail(lst));
}

void ListPopFront(List *lst)
{
    ListErase(lst, ListGetHead(lst));
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

int32_t ListGetHead(List *lst)
{
    ASSERT(lst != NULL);

    return ListGetNext(lst, ROOT);
}

int32_t ListGetTail(List *lst)
{
    ASSERT(lst != NULL);

    return ListGetPrev(lst, ROOT);
}

int32_t ListGetValue(List *lst, int32_t anch)
{
    ASSERT(lst != NULL);

    return lst->buf[anch].val;
}

int32_t ListGetFree(List *lst)
{
    ASSERT(lst != NULL);

    return lst->free;
}

int32_t ListGetSize(List *lst)
{
    ASSERT(lst != NULL);

    return lst->size;
}

int32_t ListGetCapacity(List *lst)
{
    ASSERT(lst != NULL);

    return lst->cap;
}

bool ListIsEmptyNode(List *lst, int anch)
{
    ASSERT(lst != NULL);

    return lst->buf[anch].prev == -1;
}

void ListFillStructurePosion(List *lst)
{
    ASSERT(lst != NULL);
    ListOk (lst);
    ListLog(lst);

    for (int32_t i = 0; i < sizeof(List) / sizeof(int64_t); ++i)
        *((int64_t*)(lst) + i) = POISON;
}

