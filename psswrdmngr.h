#include "mngr.h"
int mngr()
{
    printHeader();
    initialize();
    if (login())
    {
        menu();
    };
    return 0;
}