#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "list_debug.h"
#include "list.h"

const char* const COLOR_NODE_EMPTY     = "#56B13A";
const char* const COLOR_NODE_FILLED    = "#C64153";
const char* const COLOR_NODE_ROOT      = "#A4B5B0";
const char* const COLOR_NODE_INFO_HEAD = "#D07B44";
const char* const COLOR_NODE_INFO_TAIL = "#2B8574";
const char* const COLOR_EDGE_FILLED    = "#2F8F66";
const char* const COLOR_EDGE_NEXT      = "#2F8F66";
const char* const COLOR_EDGE_PREV      = "#48afd0";
const char* const COLOR_EDGE_EMPTY     = "#558006";

void ListLogInit()
{
    int32_t fd_dump = ListGetLogFd();
    dprintf(fd_dump, "<pre>");
}

void ListLogClose()
{
    int32_t fd_dump = ListGetLogFd();
    close(fd_dump);
}

void ListLog(List *lst)
{
    int32_t fd_dump = ListGetLogFd();

    ListDump(lst, fd_dump);
    ListDumpGraph(lst, fd_dump);

    dprintf(fd_dump, "<hr>\n<h2>----------------------------------------------------------------------------------------<h2>");
}

int32_t ListGetLogFd()
{
    static int32_t fd_dump = creat("./dump/list_log.html", S_IRWXU);
    return fd_dump;
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

    int32_t poisoned = 0;

    for (int32_t i = 0; i < sizeof(List) / sizeof(int64_t); ++i)
        if (*((int64_t*)(lst) + i) == POISON)
            ++poisoned;

    if (poisoned > POISIONED_PERC * sizeof(List))
        flags |= ERROR_POISIONED_STRUCT;

    if (lst->size < 0)
        flags |= ERROR_SIZE_NEG;

    if (lst->cap  < 0)
        flags |= ERROR_CAP_NEG;

    if (isBadPtr(lst->buf))
        flags |= ERROR_BUF_BAD_PTR;

    if (flags)
        return flags;

    int32_t cnt_not_empty = 0;
    for (int32_t i = 1; i < lst->cap + 1; ++i)
    {
        if (lst->buf[i].prev != -1)
        {
            ++cnt_not_empty;
            if (ListGetPrev(lst, ListGetNext(lst, i)) != i ||
                ListGetNext(lst, ListGetPrev(lst, i)) != i)
                flags |= ERROR_COMM_VIOL;
        }
        else if (lst->buf[i].next != -1 && lst->buf[lst->buf[i].next].prev != -1)
        {
            flags |= ERROR_FREE_VIOL;
        }
    }

    if (cnt_not_empty != ListGetSize(lst))
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
    const char *fd_dump_graph_filename = "./dump/.list_dump_graph.txt";
    static int32_t cnt = 0;

    int32_t fd_dump_graph = creat(fd_dump_graph_filename, S_IRWXU);

    dprintf(fd_dump_graph, "digraph G{\nsplines=ortho;\nranksep=1;\noverlap=voronoi;\n");
    dprintf(fd_dump_graph, "{rank=min;\n");
    ListDumpGraphInfoNode (DUMP_NODE_HEAD, "head", COLOR_NODE_INFO_HEAD, fd_dump_graph);
    ListDumpGraphInfoNode (DUMP_NODE_TAIL, "tail", COLOR_NODE_INFO_TAIL, fd_dump_graph);
    ListDumpGraphInfoNode (DUMP_NODE_FREE, "free", COLOR_NODE_EMPTY,     fd_dump_graph);
    dprintf(fd_dump_graph, "}\n");

    dprintf(fd_dump_graph, "{rank=same;\n");
    ListDumpGraphNodeRecord(lst, 0, COLOR_NODE_ROOT, fd_dump_graph);
    for (int32_t anch = 1; anch < lst->cap + 1; ++anch)
        if (ListIsEmptyNode(lst, anch))
            ListDumpGraphNodeRecord(lst, anch, COLOR_NODE_EMPTY, fd_dump_graph);
        else
            ListDumpGraphNodeRecord(lst, anch, COLOR_NODE_FILLED, fd_dump_graph);
    dprintf(fd_dump_graph, "}\n");

    for (int32_t anch = 0; anch < lst->cap; ++anch)
        ListDumpGraphEdge(anch, anch + 1, "#00000000", 100, fd_dump_graph);
    
    for (int32_t anch = 0; anch < ListGetCapacity(lst) + 1; ++anch)
        ListDumpGraphNodeEdges(lst, anch, fd_dump_graph);

    ListDumpGraphEdge(DUMP_NODE_HEAD, ListGetHead(lst), COLOR_EDGE_NEXT, 0, fd_dump_graph);
    ListDumpGraphEdge(DUMP_NODE_TAIL, ListGetTail(lst), COLOR_EDGE_NEXT, 0, fd_dump_graph);
    ListDumpGraphEdge(DUMP_NODE_FREE, ListGetFree(lst), COLOR_EDGE_NEXT, 0, fd_dump_graph);

    dprintf(fd_dump_graph, "{rank=max;nodemax[style=invis];} node0->nodemax[color=\"#00000000\"];");
    dprintf(fd_dump_graph, "}\n");

    close(fd_dump_graph);

    char cmd[256] = "";
    sprintf(cmd, "dot %s -o ./dump/list_graph%d.svg -Tsvg", 
                 fd_dump_graph_filename, cnt);
    system(cmd);

    dprintf(fd_dump, "<img src=\"list_graph%d.svg\" height = 300>\n",
                     cnt);

    ++cnt;
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
    dprintf(fd_dump, "node%d[shape=record, style=\"filled\", fillcolor=\"%s\", label = \"{ind: %d | val: %d | {prev: %d | next: %d}}\", width=2, height=1, fixedsize=true];\n", 
            anch, fillcolor, anch, ListGetValue(lst, anch), ListGetPrev(lst, anch), ListGetNext(lst, anch));
}

void ListDumpGraphNodeEdges(List *lst, int anch, int32_t fd_dump)
{
    int32_t next = ListGetNext(lst, anch),
            prev = ListGetPrev(lst, anch);

    ListDumpGraphEdge(anch, next, COLOR_EDGE_NEXT, 0, fd_dump);
    ListDumpGraphEdge(anch, prev, COLOR_EDGE_PREV, 0, fd_dump);
}

void ListDumpGraphEdge(int anch1, int anch2, const char *color, int32_t weight, int32_t fd_dump)
{
    if (anch1 != -1 && anch2 != -1)
        dprintf(fd_dump, "node%d->node%d[color=\"%s\", weight=%d, penwidth=3, minlen=3];\n",
                anch1, anch2, color, weight);
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
