#ifndef LIST_H
#define LIST_H

#include <stdint.h>
#include "list_struct.h"

void     ListCtor         (List *lst, int32_t size);

int32_t  ListInsertBefore (List *lst, int32_t val, int32_t anch);
int32_t  ListInsertAfter  (List *lst, int32_t val, int32_t anch);
void     ListErase        (List *lst, int32_t anch);

int32_t  ListPushBack     (List *lst, int32_t val);
int32_t  ListPushFront    (List *lst, int32_t val);

void     ListPopBack      (List *lst);
void     ListPopFront     (List *lst);

int32_t  ListGetHead      (List *lst);
int32_t  ListGetTail      (List *lst);
int32_t  ListGetNext      (List *lst, int32_t anch);
int32_t  ListGetPrev      (List *lst, int32_t anch);

#endif  // LIST_H
 
