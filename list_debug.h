#ifndef LIST_DEBUG_H
#define LIST_DEBUG_H

#include <stdio.h>
#include "list_struct.h"

#define ASSERT(expr)                                        \
do {                                                        \
    if (!(expr))                                            \
        fprintf(stderr,                                     \
                "Assertion failed. Expression: " #expr "\n" \
                "File: %s, line: %d\n"                      \
                "Function: %s\n",                           \
                __FILE__, __LINE__,                         \
                __PRETTY_FUNCTION__);                       \
} while(0)

#ifdef DEBUG
    #define ON_DEBUG(...) __VA_ARGS__
#else
    #define ON_DEBUG(...)
#endif

#define ListOk(lst)                                                          \
do                                                                           \
{                                                                            \
    int32_t fd_dump = ListGetLogFd();                                        \
    dprintf(fd_dump, "<h2>ListOk() is called from function: %s, file: %s(%d)</h2>", \
                      __PRETTY_FUNCTION__, __FILE__, __LINE__);              \
    uint32_t flags = ListStatus(lst);                                        \
    dprintf(fd_dump, "%s\n", ListErrorDesc(flags));                          \
    ASSERT(ListStatus(lst) == 0);                                            \
}                                                                            \
while(0)

const double   POISIONED_PERC         = 0.7;

const uint32_t ERROR_SIZE_NEG         = 1 << 0;
const uint32_t ERROR_BUF_BAD_PTR      = 1 << 1;
const uint32_t ERROR_COMM_VIOL        = 1 << 2;
const uint32_t ERROR_ELEMS            = 1 << 3;
const uint32_t ERROR_FREE_INCORR      = 1 << 4;
const uint32_t ERROR_FREE_VIOL        = 1 << 5;
const uint32_t ERROR_CAP_NEG          = 1 << 6;
const uint32_t ERROR_SIZE_MISMATCH    = 1 << 7;
const uint32_t ERROR_CAP_MISMATCH     = 1 << 8;
const uint32_t ERROR_POISIONED_STRUCT = 1 << 9;

extern const char * const COLOR_NODE_EMPTY;
extern const char * const COLOR_NODE_FILLED;
extern const char * const COLOR_NODE_ROOT;
extern const char * const COLOR_NODE_INFO_HEAD;
extern const char * const COLOR_NODE_INFO_TAIL;
extern const char * const COLOR_EDGE_FILLED;
extern const char * const COLOR_EDGE_NEXT;
extern const char * const COLOR_EDGE_PREV;
extern const char * const COLOR_EDGE_EMPTY;

const int32_t  DUMP_NODE_HEAD = 2147483645;
const int32_t  DUMP_NODE_TAIL = 2147483646;
const int32_t  DUMP_NODE_FREE = 2147483647;

const int64_t  POISON         = 0xDEADBEEF1451DE1ll;

int32_t     ListGetLogFd            ();
void        ListLog                 (List *lst);
void        ListLogInit             ();
void        ListLogClose            ();
void        ListPrint               (List *lst);

uint32_t    ListStatus              (List *lst);
const char* ListErrorDesc           (uint32_t flags);

void        ListDump                (List *lst, int32_t fd_dump);
void        ListDumpGraph           (List *lst, int32_t fd_dump);

void        ListDumpGraphInfoNode   (int anch, const char *name, const char *fillcolor, int32_t fd_dump);

void        ListDumpGraphNode       (List *lst, int anch, const char *fillcolor, int32_t fd_dump);

void        ListDumpGraphNodeRecord (List *lst, int anch, const char *fillcolor, int32_t fd_dump);
void        ListDumpGraphNodeEdges  (List *lst, int anch, int32_t fd_dump);

void        ListDumpGraphEdge       (int anch1, int anch2, const char *color, int32_t weight, int32_t fd_dump);

bool        isBadPtr                (void *ptr);
int32_t     min                     (int32_t a, int32_t b);
int32_t     max                     (int32_t a, int32_t b);

#endif  // LIST_DEBUG_H

