#ifndef INCLUDE_GRAPHICS_H
#define INCLUDE_GRAPHICS_H

enum { kWidth = 720, kHeight = 720 };

#ifdef __cplusplus
extern "C" {
#endif

int initScreen(void);

#ifndef __daarch34__
void updateScreen(void);
void putPixel(int X, int Y);
void setColor(int R, int G, int B);
int Rand(void);
#endif // __daarch34__

#ifdef __cplusplus
} // extern "C"
#endif

#endif // INCLUDE_GRAPHICS_H
