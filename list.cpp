#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "list.h"
#include "list_debug.h"

const int32_t  DUMP_NODE_HEAD = 2147483645;
const int32_t  DUMP_NODE_TAIL = 2147483646;
const int32_t  DUMP_NODE_FREE = 2147483647;

const int32_t  ROOT      =  0;

const int32_t  fd_dump = 1;  // TODO: fix

const uint32_t ERROR_SIZE_NEG      = 1 << 0;
const uint32_t ERROR_BUF_BAD_PTR   = 1 << 1;
const uint32_t ERROR_COMM_VIOL     = 1 << 2;
const uint32_t ERROR_ELEMS         = 1 << 3;
const uint32_t ERROR_FREE_INCORR   = 1 << 4;
const uint32_t ERROR_FREE_VIOL     = 1 << 5;
const uint32_t ERROR_CAP_NEG       = 1 << 6;
const uint32_t ERROR_SIZE_MISMATCH = 1 << 7;
const uint32_t ERROR_CAP_MISMATCH  = 1 << 8;

// TODO: border colors, background colors
const char * const COLOR_NODE_EMPTY     = "#56B13A";
const char * const COLOR_NODE_FILLED    = "#C64153";
const char * const COLOR_NODE_ROOT      = "#A4B5B0";
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

    // TODO: realloc
    // if (lst->size + 1 > lst->cap)
    //     ListRealloc(lst, ListGetNewCapacity(lst));

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

    --lst->size;
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

int32_t ListGetFree(List *lst)
{
    return lst->free;
}

int32_t ListGetSize(List *lst)
{
    return lst->size;
}

int32_t ListGetCapacity(List *lst)
{
    return lst->cap;
}

bool ListIsEmptyNode(List *lst, int anch)
{
    return lst->buf[anch].prev == -1;
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

    if (ListGetSize(lst) < 0)
        flags |= ERROR_SIZE_NEG;
    if (lst->cap  < 0)
        flags |= ERROR_CAP_NEG;

    if (isBadPtr(lst->buf))
        flags |= ERROR_BUF_BAD_PTR;

    if (flags)
        return flags;

    if (lst->buf[ListGetFree(lst)].prev != -1)
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

    if (cnt_not_empty != ListGetSize(lst))
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

    for (int32_t anch = ListGetCapacity(lst); anch > 0; --anch)
    {
        if (ListIsEmptyNode(lst, anch))
            ListDumpGraphNode (lst, anch, COLOR_NODE_EMPTY,  COLOR_EDGE_EMPTY);
        else
            ListDumpGraphNode (lst, anch, COLOR_NODE_FILLED, COLOR_EDGE_FILLED);
    }
    ListDumpGraphNode(lst, 0, COLOR_NODE_ROOT, COLOR_EDGE_FILLED);

    dprintf(fd_dump, "}\n");
}

void ListDumpGraphHeaders(List *lst)
{
    ListDumpGraphInfoNode (DUMP_NODE_HEAD, "head", COLOR_NODE_INFO_HEAD);
    ListDumpGraphEdge     (DUMP_NODE_HEAD, ListGetHead(lst), COLOR_EDGE_FILLED);

    ListDumpGraphInfoNode (DUMP_NODE_TAIL, "tail", COLOR_NODE_INFO_TAIL);
    ListDumpGraphEdge     (DUMP_NODE_TAIL, ListGetTail(lst), COLOR_EDGE_FILLED);

    ListDumpGraphInfoNode (DUMP_NODE_FREE, "free", COLOR_EDGE_EMPTY);
    ListDumpGraphEdge     (DUMP_NODE_FREE, ListGetFree(lst), COLOR_EDGE_EMPTY);
}

void ListDumpGraphInfoNode(int anch, const char *name, const char *fillcolor)
{
    dprintf(fd_dump, "node%d[shape=invhouse, style=\"filled\", fillcolor=\"%s\", label = \"%s\"];\n",
            anch, fillcolor, name);
}

void ListDumpGraphNode(List *lst, int anch, const char *fillcolor, const char *color)
{
    ListDumpGraphNodeRecord (lst, anch, fillcolor);
    ListDumpGraphNodeEdges  (lst, anch, color);
}

void ListDumpGraphNodeRecord(List *lst, int anch, const char *fillcolor)
{
    dprintf(fd_dump, "node%d[shape=record, style=\"filled\", fillcolor=\"%s\", label = \"ind: %d | val: %d | {prev: %d | next: %d}\"];\n", 
            anch, fillcolor, anch, ListGetValue(lst, anch), ListGetPrev(lst, anch), ListGetNext(lst, anch));
}

void ListDumpGraphNodeEdges(List *lst, int anch, const char *color)
{
    int32_t next = ListGetNext(lst, anch),
            prev = ListGetPrev(lst, anch);

    if (prev != -1)
        ListDumpGraphEdge(anch, prev, color);
    if (next != ListGetCapacity(lst) + 1)
        ListDumpGraphEdge(anch, next, color);
}

void ListDumpGraphEdge(int anch1, int anch2, const char *color)
{
    dprintf(fd_dump, "node%d->node%d[color=\"%s\"];\n",
            anch1, anch2, color);
}

