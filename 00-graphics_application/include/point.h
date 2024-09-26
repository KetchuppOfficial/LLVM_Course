#ifndef INCLUDE_POINT_H
#define INCLUDE_POINT_H

typedef struct point_s
{
    int x, y;
} Point;

// trying to imitate stateful lambda (God, I miss C++...)
extern Point center; // an internal point of a polygon which vertices we wish to sort

// comparator to sort points in counterclockwise order
int point_compare(const Point *lhs, const Point *rhs);

#endif // INCLUDE_POINT_H
