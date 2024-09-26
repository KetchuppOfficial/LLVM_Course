#include <assert.h>

#include "line.h"

static int midpoint(int x, int y) { return (x + y) / 2; }

void compute_normal(Line *perp, const Point *p_1, const Point *p_2)
{
    assert(perp);
    assert(p_1);
    assert(p_2);

    // (x_0, y_0) -- point through which the perpendicular passes
    const int x_0 = midpoint(p_1->x, p_2->x);
    const int y_0 = midpoint(p_1->y, p_2->y);

    // (a_1, a_2) -- direction vector
    const int a_1 = p_1->y - p_2->y;
    const int a_2 = p_2->x - p_1->x;

    perp->a = a_2;
    perp->b = -a_1;
    perp->c = a_1 * y_0 - a_2 * x_0;
}

bool lines_intersection(const Line *l_1, const Line *l_2, Point *p)
{
    assert(l_1);
    assert(l_2);

    const int det = l_1->a * l_2->b - l_2->a * l_1->b;
    if (det == 0)
        return false;

    assert(p);

    p->x = (l_1->b * l_2->c - l_2->b * l_1->c) / det;
    p->y = (l_2->a * l_1->c - l_1->a * l_2->c) / det;

    return true;
}

bool on_the_same_side(const Line *l, const Point *p_1, const Point *p_2)
{
    const int denom = l->a * l->a + l->b * l->b;
    assert(denom != 0);

    // (x_0, y_0) -- point through which the line passes
    const int x_0 = -(l->a * l->c) / denom;
    const int y_0 = -(l->b * l->c) / denom;

    // z-components of cross-products of line's direction vector
    // with 2 vectors from a point on the line to given points
    const int res_1 = (p_1->x - x_0) * l->a + (p_1->y - y_0) * l->b;
    const int res_2 = (p_2->x - x_0) * l->a + (p_2->y - y_0) * l->b;

    return (res_1 > 0 && res_2 > 0 || res_1 < 0 && res_2 < 0);
}
