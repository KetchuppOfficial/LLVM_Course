#include "graphics.h"

void lloydRelaxation(void);

int main(void) {
  int res = initScreen();
  if (res)
    return 1;

  lloydRelaxation();
  return 0;
}
