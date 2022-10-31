#include <unistd.h>
#include "list.h"

int main()
{
    int32_t fd_dump = ListInitLog();

    List lst = {};
    ListCtor(&lst, 1);
    
    ListPushBack(&lst, 1);
    ListDumpGraph(&lst, fd_dump);

    ListPushBack(&lst, 2);
    ListDumpGraph(&lst, fd_dump);

    ListPushBack(&lst, 3);
    ListDumpGraph(&lst, fd_dump);

    ListPushBack(&lst, 4);
    ListDumpGraph(&lst, fd_dump);

    ListPushBack(&lst, 5);
    ListDumpGraph(&lst, fd_dump);

    ListPushBack(&lst, 6);
    ListDumpGraph(&lst, fd_dump);

    ListPushBack(&lst, 7);
    ListDumpGraph(&lst, fd_dump);

    ListPushBack(&lst, 8);
    ListDumpGraph(&lst, fd_dump);

    ListPushBack(&lst, 9);
    ListDumpGraph(&lst, fd_dump);

    ListLinearize(&lst);
    ListDumpGraph(&lst, fd_dump);

    close(fd_dump);
    return 0;
}
