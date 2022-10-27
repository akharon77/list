#ifndef LIST_H
#define LIST_H

#include <stdint.h>

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

struct Node
{
    int32_t val;
    int32_t next;
    int32_t prev;
};

struct List
{
    int32_t  size;
    Node    *buf;

    int32_t  head;
    int32_t  tail;
    int32_t  free;
};

void ListCtor             (List *lst, int32_t size);

int32_t  ListInsertBefore (List *lst, int32_t val, int32_t anch);
int32_t  ListInsertAfter  (List *lst, int32_t val, int32_t anch);
void     ListErase        (List *lst, int32_t anch);

int32_t  ListPushBack     (List *lst, int32_t val);
int32_t  ListPushFront    (List *lst, int32_t val);

void     ListPopBack      (List *lst);
void     ListPopFront     (List *lst);

int32_t ListGetHead       (List *lst);
int32_t ListGetTail       (List *lst);
int32_t ListGetNext       (List *lst, int32_t anch);
int32_t ListGetPrev       (List *lst, int32_t anch);

#endif  // LIST_H
 
