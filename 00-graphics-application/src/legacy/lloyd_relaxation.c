#include <assert.h>

#include "graphics.h"

enum {
  kPoints = 100,
};

// Helpful functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static inline int midpoint(int x, int y) { return (x + y) / 2; }

static inline int within_range(int x, int from, int to) {
  assert(from < to);
  return from < x && x < to;
}

static inline int clamp(int value, int min, int max) {
  assert(min < max);
  if (value < min)
    return min;
  else if (value > max)
    return max;
  else
    return value;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Point ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef struct point_s {
  int x, y;
} Point;

// trying to imitate stateful lambda (God, I miss C++...)
static Point
    center; // an internal point of a polygon which vertices we wish to sort

static int point_compare(const Point *lhs, const Point *rhs) {
  return atan2_int(lhs->y - center.y, lhs->x - center.x) -
         atan2_int(rhs->y - center.y, rhs->x - center.x);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Line ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ax + by + c = 0
typedef struct line_s {
  int a, b, c;
} Line;

static void compute_normal(Line *perp, const Point *p_1, const Point *p_2) {
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

static int lines_intersection(const Line *l_1, const Line *l_2, Point *p) {
  assert(l_1);
  assert(l_2);

  const int det = l_1->a * l_2->b - l_2->a * l_1->b;
  if (det == 0)
    return 0;

  assert(p);

  p->x = (l_1->b * l_2->c - l_2->b * l_1->c) / det;
  p->y = (l_2->a * l_1->c - l_1->a * l_2->c) / det;

  return 1;
}

static int on_the_same_side(const Line *l, const Point *p_1, const Point *p_2) {
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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Lloyd relaxation ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// We statically store the border of the screen
typedef enum border_name_e {
  x_axis = 0,
  y_axis = 1,
  right_border = 2,
  bottom_border = 3
} BorderName;

static const Line borders[] = {
    {0, 1, 0},       // X axis
    {1, 0, 0},       // Y axis
    {1, 0, -kWidth}, // right border
    {0, 1, -kHeight} // top border
};

// TaggedPoint -- point that knows which perpendiculars intersect on it
typedef struct tagged_point_s {
  Point p;
  int first_perp;
  int second_perp;
} TaggedPoint;

static inline int has_tag(const TaggedPoint *p, int tag) {
  assert(p);
  return p->first_perp == tag || p->second_perp == tag;
}

// Polygon -- structure for representation of a Voronoi cell
typedef struct polygon_s {
  Point vertices[kPoints + 4];
  int n_vertices;
} Polygon;

static Point mass_center(const Polygon *polygon) {
  const int n = polygon->n_vertices;

  int x = 0;
  int y = 0;
  for (int i = 0; i != n; ++i) {
    x += polygon->vertices[i].x;
    y += polygon->vertices[i].y;
  }

  Point p = {x / n, y / n};
  return p;
}

static Line *compute_normals(const Point *points) {
  static Line perps[kPoints][kPoints - 1];

  for (int i = 0; i != kPoints; ++i) {
    for (int j = i + 1; j != kPoints; ++j)
      compute_normal(&perps[i][j - 1], &points[i], &points[j]);

    for (int j = 0; j != i; ++j)
      perps[i][j] = perps[j][i - 1];
  }

  return &perps[0][0];
}

static inline void swap(int *lhs, int *rhs) {
  int tmp = *lhs;
  *lhs = *rhs;
  *rhs = tmp;
}

static void sort(Point *points, int n_points) {
  for (int i = 1; i != n_points; ++i) {
    for (int j = 0; j != n_points - i; ++j) {
      if (point_compare(&points[j], &points[j + 1]) > 0) {
        swap(&points[j].x, &points[j + 1].x);
        swap(&points[j].y, &points[j + 1].y);
      }
    }
  }
}

static Polygon *get_cells(const Point *points) {
  static Polygon cells[kPoints];

  /*
   * Let N == (kPoints - 1) -- the number of perpendiculars associated with each
   * point. Then there are N * (N - 1) / 2 intersections between perpendiculars.
   * Each perpendicular may also intersect the borders of the rectangle, that
   * gives us 2 * N more intersections. All in all, there are (N * (N - 1) / 2)
   * + 2 * N = N * (N + 3) / 2 intersections.
   */
  static TaggedPoint intersections[4 + (kPoints - 1) * (kPoints + 3) / 2] = {
      {{0, 0}, x_axis - 4, y_axis - 4},
      {{kWidth, 0}, x_axis - 4, right_border - 4},
      {{kWidth, kHeight}, right_border - 4, bottom_border - 4},
      {{0, kHeight}, bottom_border - 4, y_axis - 4}};

  const Line *perps = compute_normals(points);

  Point p;
  for (int p_i = 0; p_i != kPoints; ++p_i) {
    int n_points = 4;

    for (int perp_1i = 0; perp_1i != kPoints - 2; ++perp_1i) {
      const Line *perp = &perps[p_i * (kPoints - 1) + perp_1i];

      for (int perp_2i = perp_1i + 1; perp_2i != kPoints - 1; ++perp_2i) {
        if (lines_intersection(perp, &perps[p_i * (kPoints - 1) + perp_2i],
                               &p) &&
            within_range(p.x, 0, kWidth) && within_range(p.y, 0, kHeight)) {
          intersections[n_points].p = p;
          intersections[n_points].first_perp = perp_1i;
          intersections[n_points++].second_perp = perp_2i;
        }
      }

      if (lines_intersection(perp, &borders[x_axis], &p) &&
          within_range(p.x, 0, kWidth)) {
        assert(p.y == 0);
        intersections[n_points].p = p;
        intersections[n_points].first_perp = perp_1i;
        intersections[n_points++].second_perp = x_axis - 4;
      }

      if (lines_intersection(perp, &borders[y_axis], &p) &&
          within_range(p.y, 0, kHeight)) {
        assert(p.x == 0);
        intersections[n_points].p = p;
        intersections[n_points].first_perp = perp_1i;
        intersections[n_points++].second_perp = y_axis - 4;
      }

      if (lines_intersection(perp, &borders[right_border], &p) &&
          within_range(p.y, 0, kHeight)) {
        assert(p.x == kWidth);
        intersections[n_points].p = p;
        intersections[n_points].first_perp = perp_1i;
        intersections[n_points++].second_perp = right_border - 4;
      }

      if (lines_intersection(perp, &borders[bottom_border], &p) &&
          within_range(p.x, 0, kWidth)) {
        assert(p.y == kHeight);
        intersections[n_points].p = p;
        intersections[n_points].first_perp = perp_1i;
        intersections[n_points++].second_perp = bottom_border - 4;
      }
    }

    int n_vertices = 0;
    for (const TaggedPoint *inter = intersections;
         inter != intersections + n_points; ++inter) {
      int add_to_polygon = 1;

      for (int perp_i = 0; perp_i != 4; ++perp_i) {
        if (!has_tag(inter, perp_i - 4) &&
            !on_the_same_side(&borders[perp_i], &points[p_i], &inter->p)) {
          add_to_polygon = 0;
          break;
        }
      }

      if (!add_to_polygon)
        continue;

      for (int perp_i = 0; perp_i != kPoints - 1; ++perp_i) {
        if (!has_tag(inter, perp_i) &&
            !on_the_same_side(&perps[p_i * (kPoints - 1) + perp_i],
                              &points[p_i], &inter->p)) {
          add_to_polygon = 0;
          break;
        }
      }

      if (add_to_polygon)
        cells[p_i].vertices[n_vertices++] = inter->p;
    }
    cells[p_i].n_vertices = n_vertices;
    assert(n_vertices <= kPoints + 4);

    center = points[p_i];
    sort(cells[p_i].vertices, n_vertices);
  }

  return cells;
}

static Point *generate_points(void) {
  static Point points[kPoints];

  for (int i = 0; i != kPoints; ++i) {
    points[i].x = clamp(Rand() % kWidth, kMargin, kWidth - kMargin);
    points[i].y = clamp(Rand() % kHeight, kMargin, kHeight - kMargin);
  }

  return points;
}

void lloyd_relaxation(void) {
  Point *points = generate_points();

  for (;;) {
    const Polygon *cells = get_cells(points);

    set_color(0, 0, 0); // black

    for (int i = 0; i != kPoints; ++i)
      draw_point(points[i].x, points[i].y);

    for (int i = 0; i != kPoints; ++i) {
      const Point *vertices = cells[i].vertices;
      const int n_vertices = cells[i].n_vertices;

      for (int k = 0; k != n_vertices - 1; ++k)
        draw_line(vertices[k].x, vertices[k].y, vertices[k + 1].x,
                  vertices[k + 1].y);

      draw_line(vertices[n_vertices - 1].x, vertices[n_vertices - 1].y,
                vertices[0].x, vertices[0].y);
    }

    update_screen();

    for (int i = 0; i != kPoints; ++i)
      points[i] = mass_center(&cells[i]);
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
