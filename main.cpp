#include "list.h"

int main()
{
    List lst = {};
    ListCtor(&lst, 4);

    ListPushBack(&lst, 1);
    ListPushBack(&lst, 2);
    ListPushFront(&lst, 3);
    ListPushFront(&lst, 4);

    ListRealloc(&lst, 8, false);
    ListDumpGraph(&lst, 1);

    return 0;
}
