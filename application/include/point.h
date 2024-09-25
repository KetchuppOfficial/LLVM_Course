#ifndef INCLUDE_POINT_H
#define INCLUDE_POINT_H

typedef struct point_s
{
    int x, y;
} Point;

// trying to imitate stateful lambda (God, I miss C++...)
extern Point center; // an internal point of a polygon which vertices we wish to sort

int point_compare(const void *ptr_1, const void *ptr_2); // to be passed to qsort

#endif // INCLUDE_POINT_H
