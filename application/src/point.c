#include <math.h>

#include "point.h"

Point center;

static int atan2_int(int x, int y)
{
    static const double pi = 3.1415926535;
    double res = atan2((double)x, (double)y) / pi;
    if (res < 0)
        res += 2.0;
    return res * 180.0;
}

int point_compare(const Point *lhs, const Point *rhs)
{
    return atan2_int(lhs->y - center.y, lhs->x - center.x) -
           atan2_int(rhs->y - center.y, rhs->x - center.x);
}
