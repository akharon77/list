#ifndef LIST_H
#define LIST_H

#include <stdint.h>
#include "list_struct.h"

void        ListCtor         (List *lst, int32_t size);

int32_t     ListInsertBefore (List *lst, int32_t val, int32_t anch);
int32_t     ListInsertAfter  (List *lst, int32_t val, int32_t anch);
void        ListErase        (List *lst, int32_t anch);

int32_t     ListPushBack     (List *lst, int32_t val);
int32_t     ListPushFront    (List *lst, int32_t val);

void        ListPopBack      (List *lst);
void        ListPopFront     (List *lst);

int32_t     ListGetValue     (List *lst, int32_t anch);
int32_t     ListGetSize      (List *lst);
int32_t     ListGetCapacity  (List *lst);

int32_t     ListGetHead      (List *lst);
int32_t     ListGetTail      (List *lst);
int32_t     ListGetFree      (List *lst);
     
int32_t     ListGetNext      (List *lst, int32_t anch);
int32_t     ListGetPrev      (List *lst, int32_t anch);

bool        ListIsEmptyNode  (List *lst, int anch);

void        ListPrint        (List *lst);

uint32_t    ListStatus       (List *lst);
const char* ListErrorDesc    (List *lst);

void        ListDumpGraph           (List *lst);

void        ListDumpGraphHeaders    (List *lst);
void        ListDumpGraphInfoNode   (int anch, const char *name, const char *fillcolor);

void        ListDumpGraphNode       (List *lst, int anch, const char *fillcolor, const char *color);

void        ListDumpGraphNodeRecord (List *lst, int anch, const char *fillcolor);
void        ListDumpGraphNodeEdges  (List *lst, int anch, const char *color);

void        ListDumpGraphEdge       (int anch1, int anch2, const char *color);

bool        isBadPtr(void *ptr);

#endif  // LIST_H
 
