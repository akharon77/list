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
    ListPushBack(&lst, 4);
    ListPushBack(&lst, 5);
    ListPushBack(&lst, 6);
    ListPushBack(&lst, 7);
    ListPushBack(&lst, 8);
    ListPushBack(&lst, 9);

    ListLinearize(&lst);

    ListLogClose();
    return 0;
}
