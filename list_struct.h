#ifndef LIST_STRUCT_H
#define LIST_STRUCT_H

struct Node
{
    int32_t val;

    int32_t next;
    int32_t prev;
};

struct List
{
    int32_t  size;
    int32_t  cap;

    Node    *buf;

    int32_t  free;
};

#endif  // LIST_STRUCT_H
