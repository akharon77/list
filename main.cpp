#include <unistd.h>
#include "list.h"

int main()
{
    ListLogInit();

    List lst = {};
    ListCtor(&lst, 1);
    
    ListPushBack(&lst, 1);
    ListPushBack(&lst, 2);

    for (int i = 0; i < 5; ++i)
        ListPushBack(&lst, i*2);

    ListErase(&lst, 4);

    ListPushFront(&lst, 42);

    ListLogClose();
    return 0;
}
