#ifndef LIST_H
#define LIST_H

#include <stdint.h>
#include "list_struct.h"
#include "list_debug.h"

#ifdef DEBUG
    #define ListCtor(lst, size, line, name, funcname, filename) \
            ListCtor_(lst, size,                                \
                      __LINE__,                                 \
                      #lst,                                     \
                      __PRETTY_FUNCTION__,                      \
                      __FILE__)
#else
    #define ListCtor(lst, size) \
            ListCtor_(lst, size)
#endif

void        ListCtor_               (List *lst, int32_t size 
                                     ON_DEBUG
                                     (
                                     ,int32_t     line,
                                      const char *name,
                                      const char *funcname,
                                      const char *filename
                                     ));

void        ListDtor                (List *lst);

int32_t     ListInsertBefore        (List *lst, int32_t val, int32_t anch);
int32_t     ListInsertAfter         (List *lst, int32_t val, int32_t anch);
void        ListErase               (List *lst, int32_t anch);

int32_t     ListPushBack            (List *lst, int32_t val);
int32_t     ListPushFront           (List *lst, int32_t val);

void        ListPopBack             (List *lst);
void        ListPopFront            (List *lst);

int32_t     ListGetValue            (List *lst, int32_t anch);
int32_t     ListGetSize             (List *lst);
int32_t     ListGetCapacity         (List *lst);

int32_t     ListGetHead             (List *lst);
int32_t     ListGetTail             (List *lst);
int32_t     ListGetFree             (List *lst);
     
int32_t     ListGetNext             (List *lst, int32_t anch);
int32_t     ListGetPrev             (List *lst, int32_t anch);

void        ListRealloc             (List *lst, int32_t new_cap, bool linear);
int32_t     ListGetNewCapacity      (List *lst, int32_t new_size);

bool        ListIsEmptyNode         (List *lst, int anch);

void        ListFillStructurePosion (List *lst);

#endif  // LIST_H
 
