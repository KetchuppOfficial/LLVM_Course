#include <assert.h>

#include "graphics.h"

enum {
  kPoints = 100,
};

// Helpful function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static inline int minElementIndex(const int *Array, int N) {
  assert(Array);
  assert(N > 0);

  int minElementI = 0;
  int minElement = Array[0];

  for (int I = 1; I != N; ++I) {
    int Element = Array[I];
    if (Element < minElement) {
      minElementI = I;
      minElement = Element;
    }
  }

  return minElementI;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Point ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef struct point_s {
  int x, y;
} Point;

static inline int square(int X) { return X * X; }

static inline int squareDist(const Point A, const Point B) {
  return square(A.x - B.x) + square(A.y - B.y);
}

static inline void generatePoints(Point *Points) {
  assert(Points);

  for (int I = 0; I != kPoints; ++I) {
    Points[I].x = kWidth / 4 + Rand() % (kWidth / 2);
    Points[I].y = kHeight / 4 + Rand() % (kHeight / 2);
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Color ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef struct color_s {
  int r, g, b;
} Color;

static inline void generateColors(Color *Colors) {
  assert(Colors);

  for (int I = 0; I != kPoints; ++I) {
    Colors[I].r = Rand() % 256;
    Colors[I].g = Rand() % 256;
    Colors[I].b = Rand() % 256;
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Lloyd relaxation ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static int getColorIndex(const Point *Points, Point P) {
  assert(Points);

  int Distances[kPoints];

  for (int I = 0; I != kPoints; ++I) {
    Distances[I] = squareDist(Points[I], P);
  }

  return minElementIndex(Distances, kPoints);
}

void lloydRelaxation(void) {
  Point Points[kPoints];
  generatePoints(Points);

  Color Colors[kPoints];
  generateColors(Colors);

  for (;;) {
    int SumX[kPoints] = {0};
    int SumY[kPoints] = {0};
    int NPointsInCell[kPoints] = {0};

    for (int X = 0; X != kWidth; ++X) {
      for (int Y = 0; Y != kHeight; ++Y) {
        Point P = {X, Y};
        int I = getColorIndex(Points, P);

        SumX[I] += X;
        SumY[I] += Y;
        NPointsInCell[I]++;

        setColor(Colors[I].r, Colors[I].g, Colors[I].b);
        putPixel(X, Y);
      }
    }

    updateScreen();

    for (int I = 0; I != kPoints; ++I) {
      Points[I].x = SumX[I] / NPointsInCell[I];
      Points[I].y = SumY[I] / NPointsInCell[I];
    }
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
