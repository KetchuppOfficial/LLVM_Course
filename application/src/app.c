#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <stddef.h>

#include <SDL2/SDL.h>

#include "line.h"

enum
{
	kPoints = 100,
	kWidth = 720,
	kHeight = 720,
	kMargin = 10
};

static Line *compute_normals(const Point *points)
{
	static Line perps[kPoints][kPoints - 1];

	for (int i = 0; i != kPoints; ++i)
	{
		for (int j = i + 1; j != kPoints; ++j)
			compute_normal(&perps[i][j - 1], &points[i], &points[j]);

		for (int j = 0; j != i; ++j)
			perps[i][j] = perps[j][i - 1];
	}

	return &perps[0][0];
}

typedef enum border_name_e
{
	x_axis = 0,
	y_axis = 1,
	right_border = 2,
	bottom_border = 3
} BorderName;

static const Line borders[] = {
	{0, 1, 0}, // X axis
	{1, 0, 0}, // Y axis
	{1, 0, -kWidth}, // right border
	{0, 1, -kHeight} // top border
};

typedef struct polygon_s
{
	Point points[kPoints + 4];
	int n_vertices;
} Polygon;

typedef struct tagged_point_s
{
	Point p;
	int first_perp;
	int second_perp;
} TaggedPoint;

#if 0
Point *get_intersections(const Point *points, const Line *perps)
{
	/*
	 * dimension 0: index of a points perpendiculars associated to which are intersected
	 * dimension 1: index of the first perpendicular
	 * dimension 2: index of the second perpendicular
	 */
	static Point intersections[kPoints][kPoints - 1][kPoints - 1];


	Point p;
	for (int i = 0; i != kPoints; ++i)
	{
		for (int j = 0; j != kPoints - 2; ++j)
		{
			const Line *perp = &perps[i * (kPoints - 1) + j];

			for (int k = j + 1; k != kPoints - 1; ++k)
			{
				if (lines_intersection(perp, &perps[i * (kPoints - 1) + k], &p) &&
				    (0 < p.x && p.x < kWidth) && (0 < p.y && p.y < kHeight))
				{
					intersections[i][j][k] = p;
				}
			}

			if (lines_intersection(perp, &borders[x_axis], &p) && (0 < p.x && p.x < kWidth))
			{
				assert(p.y == 0);
				intersections[i][] = p;
			}

			if (lines_intersection(perp, &borders[y_axis], &p) && (0 < p.y && p.y < kHeight))
			{
				assert(p.x == 0);
				intersections[i][n_points] = p;
			}

			if (lines_intersection(perp, &borders[right_border], &p) && (0 < p.y && p.y < kHeight))
			{
				assert(p.x == kWidth);
				intersections[i][n_points] = p;
			}

			if (lines_intersection(perp, &borders[bottom_border], &p) && (0 < p.x && p.x < kWidth))
			{
				assert(p.y == kHeight);
				intersections[i][n_points] = p;
			}
		}
	}
}
#endif

Polygon *get_cells(const Point *points)
{
	static Polygon cells[kPoints];

	/*
	 * Let N == (kPoints - 1) -- the number of perpendiculars associated with each point.
	 * Then there are N * (N - 1) / 2 intersections between perpendiculars.
	 * Each perpendicular may also intersect the borders of the rectangle, that gives us 2 * N more
	 * intersections.
	 * All in all, there are (N * (N - 1) / 2) + 2 * N = N * (N + 3) / 2 intersections.
	 */
	static TaggedPoint intersections[4 + (kPoints - 1) * (kPoints + 3) / 2] = {
		{{0,      0},       x_axis - 4,        y_axis - 4       },
		{{kWidth, 0},       x_axis - 4,        right_border - 4 },
		{{kWidth, kHeight}, right_border - 4,  bottom_border - 4},
		{{0,      kHeight}, bottom_border - 4, y_axis - 4       }
	};

	const Line *perps = compute_normals(points);

	Point p;
	for (int i = 0; i != kPoints; ++i)
	{
		int n_points = 4;

		for (int j = 0; j != kPoints - 2; ++j)
		{
			const Line *perp = &perps[i * (kPoints - 1) + j];

			for (int k = j + 1; k != kPoints - 1; ++k)
			{
				if (lines_intersection(perp, &perps[i * (kPoints - 1) + k], &p) &&
				    (0 < p.x && p.x < kWidth) && (0 < p.y && p.y < kHeight))
				{
					intersections[n_points].p = p;
					intersections[n_points].first_perp = j;
					intersections[n_points++].second_perp = k;
				}
			}

			if (lines_intersection(perp, &borders[x_axis], &p) && (0 < p.x && p.x < kWidth))
			{
				assert(p.y == 0);
				intersections[n_points].p = p;
				intersections[n_points].first_perp = j;
				intersections[n_points++].second_perp = x_axis - 4;
			}

			if (lines_intersection(perp, &borders[y_axis], &p) && (0 < p.y && p.y < kHeight))
			{
				assert(p.x == 0);
				intersections[n_points].p = p;
				intersections[n_points].first_perp = j;
				intersections[n_points++].second_perp = y_axis - 4;
			}

			if (lines_intersection(perp, &borders[right_border], &p) && (0 < p.y && p.y < kHeight))
			{
				assert(p.x == kWidth);
				intersections[n_points].p = p;
				intersections[n_points].first_perp = j;
				intersections[n_points++].second_perp = right_border - 4;
			}

			if (lines_intersection(perp, &borders[bottom_border], &p) && (0 < p.x && p.x < kWidth))
			{
				assert(p.y == kHeight);
				intersections[n_points].p = p;
				intersections[n_points].first_perp = j;
				intersections[n_points++].second_perp = bottom_border - 4;
			}
		}

		int n_vertices = 0;
		for (int j = 0; j != n_points; ++j)
		{
			bool add_to_polygon = true;

			for (int perp_i = -4; perp_i != 0; ++perp_i)
			{
				if (perp_i == intersections[j].first_perp || perp_i == intersections[j].second_perp)
					continue;

				if (!on_the_same_side(&borders[perp_i + 4], &points[i], &intersections[j].p))
				{
					add_to_polygon = false;
					break;
				}
			}

			if (!add_to_polygon)
				continue;

			for (int perp_i = 0; perp_i != kPoints - 1; ++perp_i)
			{
				if (perp_i == intersections[j].first_perp || perp_i == intersections[j].second_perp)
					continue;

				if (!on_the_same_side(&perps[i * (kPoints - 1) + perp_i], &points[i], &intersections[j].p))
				{
					add_to_polygon = false;
					break;
				}
			}

			if (add_to_polygon)
				cells[i].points[n_vertices++] = intersections[j].p;
		}
		cells[i].n_vertices = n_vertices;
		assert(n_vertices <= kPoints + 4);

		center = points[i];
		qsort(cells[i].points, n_vertices, sizeof(Point), point_compare);
	}

	return cells;
}

static int clamp(int value, int min, int max)
{
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else
		return value;
}

static Point *generate_points()
{
	static Point points[kPoints];

	srand(time(NULL));

	for (int i = 0; i != kPoints; ++i)
	{
		points[i].x = clamp(rand() % kWidth, kMargin, kWidth - kMargin);
		points[i].y = clamp(rand() % kHeight, kMargin, kHeight - kMargin);
	}

	return points;
}

static void line_to_segment(const Line *l, Point *p_1, Point *p_2);

Point mass_center(const Polygon *polygon)
{
	const int n = polygon->n_vertices;

	int x = 0;
	int y = 0;
	for (int i = 0; i != n; ++i)
	{
		x += polygon->points[i].x;
		y += polygon->points[i].y;
	}

	Point p = {x / n, y / n};
	return p;
}

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow("Window",
	                                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
										  kWidth, kHeight,
										  SDL_WINDOW_SHOWN);
	if (!window)
	{
		fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer)
	{
		fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Event event;
	Point *points = generate_points();

	while (true)
	{
		Polygon *cells = get_cells(points);

		if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
		{
			SDL_DestroyWindow(window);
			SDL_Quit();
			break;
		}

		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

		for (int i = 0; i != kPoints; ++i)
		{
			SDL_Rect r = {points[i].x - 1, points[i].y - 1, 3, 3};
			SDL_RenderDrawRect(renderer, &r);
		}

		for (int i = 0; i != kPoints; ++i)
		{
			const Point *vertices = cells[i].points;
			const int n_vertices = cells[i].n_vertices;

			for (int k = 0; k != n_vertices - 1; ++k)
			{
				SDL_RenderDrawLine(renderer, vertices[k].x, vertices[k].y,
				                             vertices[k + 1].x, vertices[k + 1].y);
			}

			#if 0
			SDL_RenderDrawLine(renderer, vertices[n_vertices - 1].x, vertices[n_vertices - 1].y,
				                         vertices[0].x, vertices[0].y);
			#endif
		}

#if 0
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

		Point p_1, p_2;
		Line *perps = compute_normals(points);

		for (int i = 0; i != kPoints; ++i)
		{
			for (int j = i; j != kPoints - 1; ++j)
			{
				line_to_segment(&perps[i * (kPoints - 1) + j], &p_1, &p_2);
				SDL_RenderDrawLine(renderer, p_1.x, p_1.y, p_2.x, p_2.y);
			}
		}
#endif
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderPresent(renderer);

		for (int i = 0; i != kPoints; ++i)
			points[i] = mass_center(&cells[i]);
	}

	return 0;
}

static void line_to_segment(const Line *l, Point *p_1, Point *p_2)
{
	assert(l);
	assert(p_1);
	assert(p_2);

	Point p_x;
	const bool int_x = lines_intersection(l, &borders[x_axis], &p_x) && (0 <= p_x.x && p_x.x <= kWidth);

	Point p_y;
	const bool int_y = lines_intersection(l, &borders[y_axis], &p_y) && (0 <= p_y.y && p_y.y <= kHeight);

	Point p_r;
	const bool int_r = lines_intersection(l, &borders[right_border], &p_r) && (0 <= p_r.y && p_r.y <= kHeight);

	Point p_t;
	const bool int_t = lines_intersection(l, &borders[bottom_border], &p_t) && (0 <= p_t.x && p_t.x <= kWidth);

	if (int_x)
	{
		*p_1 = p_x;
		if (int_y)
			*p_2 = p_y;
		else if (int_r)
			*p_2 = p_r;
		else if (int_t)
			*p_2 = p_t;
		else
		{
			assert(false);
		}
	}
	else if (int_y)
	{
		*p_1 = p_y;
		if (int_r)
			*p_2 = p_r;
		else if (int_t)
			*p_2 = p_t;
		else
		{
			assert(false);
		}
	}
	else
	{
		assert(int_t || int_r);
		*p_1 = p_t;
		*p_2 = p_r;
	}
}
