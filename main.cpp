#include "list.h"

int main()
{
    List lst = {};
    ListCtor(&lst, 10);

    for (int i = 0; i < 5; ++i)
        ListPushBack(&lst, i);

    ListPrint(&lst);

    ListInsertBefore(&lst, 56, 3);
    ListInsertAfter(&lst, 23, 1);

    ListPrint(&lst);

    ListErase(&lst, 2);
    ListErase(&lst, 4);

    ListPrint(&lst);
    ListDumpGraph(&lst);

    ListPushFront(&lst, 93);
    ListPushFront(&lst, 32);

    ListPrint(&lst);

    return 0;
}
