#ifndef INCLUDE_LINE_H
#define INCLUDE_LINE_H

#include <stdbool.h>

#include "point.h"

// ax + by + c = 0
typedef struct line_s
{
    int a, b, c;
} Line;

void compute_normal(Line *perp, const Point *p_1, const Point *p_2);
bool lines_intersection(const Line *l_1, const Line *l_2, Point *p);
bool on_the_same_side(const Line *l, const Point *p_1, const Point *p_2);

#endif // INCLUDE_LINE_H
