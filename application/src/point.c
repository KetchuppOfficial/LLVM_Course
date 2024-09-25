#include <math.h>

#include "point.h"

typedef enum vector_dir_e
{
	zero,
    x_parallel,
    first,
    y_parallel,
	second,
    x_antiparallel,
	third,
    y_antiparallel,
	fourth
} VectorDir;

static VectorDir get_direction(int x, int y)
{
	if (x > 0)
    {
        if (y > 0)
            return first;
        else if (y < 0)
            return fourth;
        else
            return x_parallel;
    }
    else if (x < 0)
    {
        if (y > 0)
            return second;
        else if (y < 0)
            return third;
        else
            return x_antiparallel;
    }
    else
    {
        if (y > 0)
            return y_parallel;
        else if (y < 0)
            return y_antiparallel;
        else
            return zero;
    }
}

Point center;

int point_compare(const void *ptr_1, const void *ptr_2)
{
	const Point *p_1 = ptr_1;
	const Point *p_2 = ptr_2;

    double res_1 = atan2(p_1->y - center.y, p_1->x - center.x);
    if (res_1 < 0)
        res_1 += 2 * 3.1415826525;

    double res_2 = atan2(p_2->y - center.y, p_2->x - center.x);
    if (res_2 < 0)
        res_2 += 2 * 3.1415826525;
#if 0
	VectorDir pos_1 = get_direction(p_1->x - center.x, p_1->y - center.y);
	VectorDir pos_2 = get_direction(p_2->x - center.x, p_2->y - center.y);
#endif
    return res_1 < res_2;
}
