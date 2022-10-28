#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "list_debug.h"

const int32_t  DUMP_HEAD = -2;
const int32_t  DUMP_TAIL = -3;
const int32_t  DUMP_FREE = -1;

const int32_t  ROOT      =  0;

const uint32_t ERROR_SIZE_NEG      = 1 << 0;
const uint32_t ERROR_BUF_BAD_PTR   = 1 << 1;
const uint32_t ERROR_COMM_VIOL     = 1 << 2;
const uint32_t ERROR_ELEMS         = 1 << 3;
const uint32_t ERROR_FREE_INCORR   = 1 << 4;
const uint32_t ERROR_FREE_VIOL     = 1 << 5;
const uint32_t ERROR_CAP_NEG       = 1 << 6;
const uint32_t ERROR_SIZE_MISMATCH = 1 << 7;
const uint32_t ERROR_CAP_MISMATCH  = 1 << 8;

const char * const COLOR_NODE_EMPTY     = "#56B13A";
const char * const COLOR_NODE_FILLED    = "#C64153";
const char * const COLOR_NODE_INFO_HEAD = "#D07B44";
const char * const COLOR_NODE_INFO_TAIL = "#2B8574";
const char * const COLOR_EDGE_FILLED    = "#2F8F66";
const char * const COLOR_EDGE_EMPTY     = "#558006";

void ListCtor(List *lst, int32_t size)
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

    lst->buf[ROOT] = 
        {
            .val   = 0,
            .next  = ROOT,
            .prev  = ROOT
        };
}

void ListDtor(List *lst)
{
    ASSERT(lst != NULL);
    
    lst->size = -1;
    lst->cap  = -1;
    lst->free = -1;
    free(lst->buf);
}

int32_t ListInsertBefore(List *lst, int32_t val, int32_t anch)
{
    ASSERT(lst != NULL);
    ASSERT(anch < lst->cap + 1 && anch > -1);

    int32_t npos = lst->free;
    lst->free = lst->buf[lst->free].next;

    int32_t anch_prev = lst->buf[anch].prev;

    lst->buf[npos].val  = 
        {
            .val   = val
            .next  = anch,
            .prev  = anch_prev
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
}

int32_t ListPushBack(List *lst, int32_t val)
{
    ASSERT(lst != NULL);

    ListInsertBefore(lst, val, ROOT);
}

int32_t ListPushFront(List *lst, int32_t val)
{
    ASSERT(lst != NULL);

    ListInsertAfter(lst, val, ROOT);
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

    return ListGetNext(lst, ROOT);
}

int32_t ListGetTail(List *lst)
{
    ASSERT(lst != NULL);

    return ListGetPrev(lst, ROOT);
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

int32_t ListGetValue(List *lst, int32_t anch)
{
    ASSERT(lst != NULL);

    return lst->buf[anch].val;
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

    if (lst->buf[lst->free].prev != -1)
        flags |= ERROR_FREE_INCORR;

    if (sizeof(lst->buf) / sizeof(Node) != lst->cap)
        flags |= ERROR_CAP_MISMATCH;

    int32_t cnt_not_empty = 0;
    for (int32_t i = 0; i < lst->size + 1; ++i)
    {
        if (lst->buf[i].prev != -1)
        {
            ++cnt_not_empty;
            if (ListGetPrev(lst, ListGetNext(lst, i)) != i ||
                ListGetNext(lst, ListGetPrev(lst, i)) != i)
                flags |= ERROR_COMM_VIOL;
        }
        else if (lst->buf[lst->buf[i].next].prev != -1)
        {
            flags |= ERROR_FREE_VIOL;
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
    if (flags & ERROR_FREE_VIOL)
        return "Broken structure of free elements of list";
    if (flags & ERROR_CAP_NEG)
        return "Capacity of list is negative";
    if (flags & ERROR_SIZE_MISMATCH)
        return "Real size of list doesn't match to list size";
    if (flags & ERROR_CAP_MISMATCH)
        return "Real capacity of list doesn't mathc to list capacity";
}

void ListDumpGraph(List *lst)
{
    dprintf(fd_dump, "digraph G{rankdir=LR;");

    ListDumpGraphHeaders(lst);

    for (int32_t anch = 1; i < lst->size + 1; ++i)
    {
        if (ListIsEmptyNode(lst, anch))
        {
            ListDumpGraphValNode(lst, anch, COLOR_NODE_EMPTY);
            ListDumpGraphValNodeEdges(lst
        }
        else
        {
            ListDumpGraphValNode(lst, anch, COLOR_NODE_FILLED);
        }
    }

    dprintf(fd_dump, "}");
}

void ListDumpGraphHeaders(List *lst)
{
    ListDumpMakeInfoNode("head", 
}

void ListDumpGraphInfoNode(const char *name, const char *fillcolor)
{
    dprintf(fd_dump, "node#%s [shape=invhouse, style=\"filled\", fillcolor=%s, label = \"%s\"",
            name, fillcolor, name);
}

void ListDumpGraphNode(List *lst, int anch, const char *fillcolor, const char *color)
{
    ListDumpGraphNodeRecord (lst, anch, fillcolor);
    ListDumpGraphNodeEdges  (lst, anch, color);
}

void ListDumpGraphNodeRecord(List *lst, int anch, const char *fillcolor)
{
    dprintf(fd_dump, "node#%d [shape=record, style=\"filled\", fillcolor=%s, label = \"{ind: %d | val: %d | {prev: %d | next: %d}}\"];", 
            anch, fillcolor, anch, ListGetValue(lst, anch), ListGetPrev(lst, anch), ListGetNext(lst, anch));
}

void ListDumpGraphNodeEdges(List *lst, int anch, const char *color)
{
    int32_t next = ListGetNext(lst, anch),
            prev = ListGetPrev(lst, anch);

    if (prev != -1)
        ListDumpGraphEdge(lst, anch, next, color);
    ListDumpGraphEdge(lst, prev, anch, color);
}

void ListDumpGraphEdge(List *lst, int anch1, int anch2, const char *color)
{
    dprintf(fd_dump, "node#%d->node#%d[color=\"%s\"]", anch1, anch2);
}

