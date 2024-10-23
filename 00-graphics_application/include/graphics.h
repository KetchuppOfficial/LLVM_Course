#ifndef INCLUDE_GRAPHICS_H
#define INCLUDE_GRAPHICS_H

enum { kWidth = 720, kHeight = 720, kMargin = 10 };

#ifdef __cplusplus
extern "C" {
#endif

int init_screen(void);

void set_color(int r, int g, int b);
void draw_point(int x, int y);
void draw_line(int x_1, int y_1, int x_2, int y_2);
void update_screen(void);

int Rand(void);
int atan2_int(int x, int y);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // INCLUDE_GRAPHICS_H
