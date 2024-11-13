#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "graphics.h"

static SDL_Window *Window;
static SDL_Renderer *Renderer;

static void appExit(void) {
  SDL_DestroyRenderer(Renderer);
  SDL_DestroyWindow(Window);
  SDL_Quit();
}

int initScreen(void) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
    return 1;
  }

  Window = SDL_CreateWindow("Lloyd relaxation", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, kWidth, kHeight,
                            SDL_WINDOW_SHOWN);

  if (!Window) {
    fprintf(stderr, "Error creating Window: %s\n", SDL_GetError());
    return 1;
  }

  Renderer = SDL_CreateRenderer(Window, -1, 0);
  if (!Renderer) {
    fprintf(stderr, "Error creating Renderer: %s\n", SDL_GetError());
    return 1;
  }

  srand(time(NULL));
  atexit(appExit);

  setColor(255, 255, 255); // white
  SDL_RenderClear(Renderer);

  return 0;
}

void setColor(int R, int G, int B) {
  SDL_SetRenderDrawColor(Renderer, R, G, B, 255);
}

void putPixel(int X, int Y) {
  SDL_RenderDrawPoint(Renderer, X, Y);
}

void updateScreen(void) {
  SDL_Event Event;
  if (SDL_PollEvent(&Event) && Event.type == SDL_QUIT)
    exit(EXIT_SUCCESS);

  SDL_RenderPresent(Renderer);

  setColor(255, 255, 255); // white
  SDL_RenderClear(Renderer);
}

int Rand(void) { return rand(); }
