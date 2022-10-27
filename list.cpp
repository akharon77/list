#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "list_debug.h"

const uint32_t ERROR_SIZE_NEG      = 1 << 0,
               ERROR_BUF_BAD_PTR   = 1 << 1,
               ERROR_COMM_VIOL     = 1 << 2,
               ERROR_ELEMS         = 1 << 3,
               ERROR_FREE_INCORR   = 1 << 4,

               ERROR_CAP_NEG       = 1 << 6,
               ERROR_SIZE_MISMATCH = 1 << 7,
               ERROR_CAP_MISMATCH  = 1 << 8;

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
                .val   = -1,
                .next  = i + 1,
                .prev  = i - 1,
                .empty = true
            };

    lst->buf[0] = 
        {
            .val   = 0,
            .next  = 0,
            .prev  = 0,
            .empty = false
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

    int32_t anch_prev = lst->buf[anch].prev;

    lst->buf[npos].val  = 
        {
            .val   = val
            .next  = anch,
            .prev  = anch_prev,
            .empty = false
        };

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
            .val   = -1,
            .next  = lst->free,
            .prev  = -1,
            .empty = true 
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
    ASSERT(lst != NULL);

    uint32_t flags = 0;

    if (lst->size < 0)
        flags |= ERROR_SIZE_NEG;
    if (lst->cap  < 0)
        flags |= ERROR_CAP_NEG;

    if (isBadPtr(lst->buf))
        flags |= ERROR_BUF_BAD_PTR;

    if (flags)
        return flags;

    if (!lst->buf[lst->free].empty)
        flags |= ERROR_FREE_INCORR;

    if (sizeof(lst->buf) / sizeof(Node) != lst->cap)
        flags |= ERROR_CAP_MISMATCH;

    int32_t cnt_not_empty = 0;
    for (int32_t i = 0; i < lst->size + 1; ++i)
    {
        if (!lst->buf[i].empty)
        {
            ++cnt_not_empty;
            if (ListGetPrev(lst, ListGetNext(lst, i)) != i ||
                ListGetNext(lst, ListGetPrev(lst, i)) != i)
                flags |= ERROR_COMM_VIOL;
        }
    }

    if (cnt_not_empty != lst->size)
        flags |= ERROR_SIZE_MISMATCH;

    if (flags)
        return flags;
}

bool isBadPtr(void *ptr)
{
    if (ptr == NULL)
        return true;

    int nullfd = open("/dev/random", O_WRONLY);
    bool res = write(nullfd, (char*) ptr, 1) < 0;

    close(nullfd);
    return res;
}

const char* ListErrorDesc(List *lst)
{
    uint32_t flags = ListStatus(lst);

    if (flags & ERROR_SIZE_NEG)
        return "Size of list is negative";
    if (flags & ERROR_BUF_BAD_PTR)
        return "Bad pointer to the elements of the list";
    if (flags & ERROR_COMM_VIOL)
        return "Broken ring structure between list items";
    if (flags & ERROR_ELEMS)
        return "Elements are wrong";
    if (flags & ERROR_FREE_INCORR)
        return "Bad pointer to the free section of list";
    if (flags & ERROR_CAP_NEG)
        return "Capacity of list is negative";
    if (flags & ERROR_SIZE_MISMATCH)
        return "Real size of list doesn't match to list size";
    if (flags & ERROR_CAP_MISMATCH)
        return "Real capacity of list doesn't mathc to list capacity";
}
