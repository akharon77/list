#include <unistd.h>
#include <fcntl.h>
#include "list_debug.h"
#include "list.h"

const char * const COLOR_NODE_EMPTY     = "#56B13A";
const char * const COLOR_NODE_FILLED    = "#C64153";
const char * const COLOR_NODE_ROOT      = "#A4B5B0";
const char * const COLOR_NODE_INFO_HEAD = "#D07B44";
const char * const COLOR_NODE_INFO_TAIL = "#2B8574";
const char * const COLOR_EDGE_FILLED    = "#2F8F66";
const char * const COLOR_EDGE_NEXT      = "#2F8F66";
const char * const COLOR_EDGE_PREV      = "#48afd0";
const char * const COLOR_EDGE_EMPTY     = "#558006";

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

    if (ListGetFree(lst) > ListGetCapacity(lst) || lst->buf[ListGetFree(lst)].prev != -1)
        flags |= ERROR_FREE_INCORR;

    if (flags)
        return flags;

    int32_t cnt_not_empty = 0;
    for (int32_t i = 0; i < ListGetCapacity(lst); ++i)
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

    if (cnt_not_empty - 1 != ListGetSize(lst))
        flags |= ERROR_SIZE_MISMATCH;

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

const char* ListErrorDesc(uint32_t flags)
{
    if (flags & ERROR_POISIONED_STRUCT)
        return "List is poisoned and invalid";
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
        return "Real capacity of list doesn't match to list capacity";
    return "OK";
}

void ListDump(List *lst, int32_t fd_dump)
{
    ASSERT(!isBadPtr(lst));

    uint32_t flags = ListStatus(lst);

    dprintf(fd_dump, "List[%p]\n"
                     "Status: %x, %s\n"
                     "%s at %s in %s(%ld): \n"
                     "{\n\tsize = %ld, \n"
                     "\tcapacity = %ld, \n",
                     lst,
                     flags, ListErrorDesc(flags),
                     lst->info.name, lst->info.funcname, lst->info.filename, lst->info.line,
                     lst->size,
                     lst->cap);

    for (int64_t i = 0; i < lst->cap + 1; ++i)
        dprintf(fd_dump, "\t[%ld] = \n"
                         "\t{\n"
                         "\t\tval: \t%d,\n"
                         "\t\tnext:\t%d,\n"
                         "\t\tprev:\t%d\n"
                         "\t}\n",
                         i, lst->buf[i].val, lst->buf[i].next, lst->buf[i].prev);

    dprintf(fd_dump, "}\n");
}

void ListDumpGraph(List *lst, int32_t fd_dump)
{
    dprintf(fd_dump, "digraph G{rankdir=LR;");

    ListDumpGraphHeaders(lst, fd_dump);

    ListDumpGraphNode(lst, 0, COLOR_NODE_ROOT, fd_dump);
    for (int32_t anch = ListGetCapacity(lst); anch > 0; --anch)
    {
        if (ListIsEmptyNode(lst, anch))
            ListDumpGraphNode (lst, anch, COLOR_NODE_EMPTY, fd_dump);
        else
            ListDumpGraphNode (lst, anch, COLOR_NODE_FILLED, fd_dump);
    }

    dprintf(fd_dump, "}\n");
}

void ListDumpGraphHeaders(List *lst, int32_t fd_dump)
{
    ListDumpGraphInfoNode (DUMP_NODE_HEAD, "head", COLOR_NODE_INFO_HEAD, fd_dump);
    ListDumpGraphEdge     (DUMP_NODE_HEAD, ListGetHead(lst), COLOR_EDGE_FILLED, fd_dump);

    ListDumpGraphInfoNode (DUMP_NODE_TAIL, "tail", COLOR_NODE_INFO_TAIL, fd_dump);
    ListDumpGraphEdge     (DUMP_NODE_TAIL, ListGetTail(lst), COLOR_EDGE_FILLED, fd_dump);

    ListDumpGraphInfoNode (DUMP_NODE_FREE, "free", COLOR_EDGE_EMPTY, fd_dump);
    ListDumpGraphEdge     (DUMP_NODE_FREE, ListGetFree(lst), COLOR_EDGE_EMPTY, fd_dump);
}

void ListDumpGraphInfoNode(int anch, const char *name, const char *fillcolor, int32_t fd_dump)
{
    dprintf(fd_dump, "node%d[shape=invhouse, style=\"filled\", fillcolor=\"%s\", label = \"%s\"];\n",
            anch, fillcolor, name);
}

void ListDumpGraphNode(List *lst, int anch, const char *fillcolor, int32_t fd_dump)
{
    ListDumpGraphNodeRecord (lst, anch, fillcolor, fd_dump);
    ListDumpGraphNodeEdges  (lst, anch, fd_dump);
}

void ListDumpGraphNodeRecord(List *lst, int anch, const char *fillcolor, int32_t fd_dump)
{
    dprintf(fd_dump, "node%d[shape=record, style=\"filled\", fillcolor=\"%s\", label = \"ind: %d | val: %d | {prev: %d | next: %d}\"];\n", 
            anch, fillcolor, anch, ListGetValue(lst, anch), ListGetPrev(lst, anch), ListGetNext(lst, anch));
}

void ListDumpGraphNodeEdges(List *lst, int anch, int32_t fd_dump)
{
    int32_t next = ListGetNext(lst, anch),
            prev = ListGetPrev(lst, anch);

    ListDumpGraphEdge(anch, prev, COLOR_EDGE_PREV, fd_dump);
    ListDumpGraphEdge(anch, next, COLOR_EDGE_NEXT, fd_dump);
}

void ListDumpGraphEdge(int anch1, int anch2, const char *color, int32_t fd_dump)
{
    if (anch1 != -1 && anch2 != -1)
        dprintf(fd_dump, "node%d->node%d[color=\"%s\"];\n",
                anch1, anch2, color);
}

int32_t min(int32_t a, int32_t b)
{
    if (a < b)
        return a;
    else
        return b;
}

int32_t max(int32_t a, int32_t b)
{
    if (a > b)
        return a;
    else
        return b;
}
