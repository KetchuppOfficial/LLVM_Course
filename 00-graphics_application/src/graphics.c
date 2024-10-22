#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "graphics.h"

static SDL_Window *window;
static SDL_Renderer *renderer;

static void app_exit(void) {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int init_screen(void) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
    return 1;
  }

  window = SDL_CreateWindow("Lloyd relaxation", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, kWidth, kHeight,
                            SDL_WINDOW_SHOWN);

  if (!window) {
    fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
    return 1;
  }

  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
    return 1;
  }

  srand(time(NULL));
  atexit(app_exit);

  set_color(255, 255, 255); // white
  SDL_RenderClear(renderer);

  return 0;
}

void set_color(int r, int g, int b) {
  SDL_SetRenderDrawColor(renderer, r, g, b, 255);
}

void draw_point(int x, int y) {
  SDL_Rect r = {x - 1, y - 1, 3, 3};
  SDL_RenderDrawRect(renderer, &r);
}

void draw_line(int x_1, int y_1, int x_2, int y_2) {
  SDL_RenderDrawLine(renderer, x_1, y_1, x_2, y_2);
}

void update_screen(void) {
  SDL_Event event;
  if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
    exit(EXIT_SUCCESS);

  SDL_RenderPresent(renderer);

  set_color(255, 255, 255); // white
  SDL_RenderClear(renderer);
}

int Rand(void) { return rand(); }

int atan2_int(int x, int y) {
  static const double pi = 3.1415926535;
  double res = atan2((double)x, (double)y) / pi;
  if (res < 0)
    res += 2.0;
  return res * 180.0;
}
