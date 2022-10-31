#include "list.h"

int main()
{
    List lst = {};
    ListCtor(&lst, 1);
    
    ListPushBack(&lst, 1);
    ListDumpGraph(&lst, 1);

    ListPushBack(&lst, 2);
    ListDumpGraph(&lst, 1);

    ListPushBack(&lst, 3);
    ListDumpGraph(&lst, 1);

    ListPushBack(&lst, 4);
    ListDumpGraph(&lst, 1);

    ListPushBack(&lst, 5);
    ListDumpGraph(&lst, 1);

    ListPushBack(&lst, 6);
    ListDumpGraph(&lst, 1);

    ListPushBack(&lst, 7);
    ListDumpGraph(&lst, 1);

    ListPushBack(&lst, 8);
    ListDumpGraph(&lst, 1);

    ListPushBack(&lst, 9);
    ListDumpGraph(&lst, 1);

    for (int i = 0; i < 9; ++i)
    {
        ListPopBack(&lst);
        ListDumpGraph(&lst, 1);
    }

    for (int i = 0; i < 9; ++i)
    {
        ListPushFront(&lst, i + 1);
        ListDumpGraph(&lst, 1);
    }

    ListLinearize(&lst);
    ListDumpGraph(&lst, 1);
    return 0;
}
