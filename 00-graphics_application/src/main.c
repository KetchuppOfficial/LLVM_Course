#include "graphics.h"

void lloyd_relaxation();

int main()
{
    int res = init_screen();
    if (res)
        return 1;

    lloyd_relaxation();
    return 0;
}
