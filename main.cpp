#include "list.h"

int main()
{
    List lst = {};
    ListCtor(&lst, 1);
    
    ListPushBack(&lst, 1);
    ListPushBack(&lst, 2);
    ListPushBack(&lst, 3);

    ListDumpGraph(&lst, 1);

    return 0;
}
