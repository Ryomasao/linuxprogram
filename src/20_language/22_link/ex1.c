#include <stdio.h>

#include "ex1.h"

int f(int);
int f2(void);

int x = NUMBER;
int y = 2;
int a;

int main(void) {
  printf("x=%d\n", x);
  printf("f(x)=%d\n", f(x));
  printf("f2(void)=%d\n", f2());
  printf("main f2 a=%d\n", a);
  return 0;
}
