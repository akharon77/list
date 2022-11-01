#include <unistd.h>
#include "list.h"

int main()
{
    ListLogInit();

    List lst = {};
    ListCtor(&lst, 1);
    
    ListPushBack(&lst, 1);
    ListPushBack(&lst, 2);
    ListPushBack(&lst, 3);

    int32_t anch4 = ListPushFront(&lst, 4);
    ListPushFront(&lst, 5);
    ListPushFront(&lst, 6);

    ListPushBack(&lst, 7);
    ListPushBack(&lst, 8);
    ListPushBack(&lst, 9);

    ListErase(&lst, anch4);

    ListLinearize(&lst);

    ListLogClose();
    return 0;
}
