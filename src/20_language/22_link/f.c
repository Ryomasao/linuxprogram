#include <stdio.h>

#include "ex1.h"

extern int y;
int y;

int f(int x) {
  extern int y;
  x = x * NUMBER;
  return x * y;
}