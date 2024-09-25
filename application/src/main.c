#include "graphics.h"
#include "lloyd_relaxation.h"

int main()
{
    int res = init_screen();
    if (res)
        return 1;

    lloyd_relaxation();
    return 0;
}
