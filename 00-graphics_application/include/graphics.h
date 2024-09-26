#ifndef INCLUDE_GRAPHICS_H
#define INCLUDE_GRAPHICS_H

enum
{
    kWidth = 720,
    kHeight = 720,
    kMargin = 10
};

int init_screen();

void set_color(int r, int g, int b);
void draw_point(int x, int y);
void draw_line(int x_1, int y_1, int x_2, int y_2);
void update_screen();

int Rand();

#endif // INCLUDE_GRAPHICS_H
