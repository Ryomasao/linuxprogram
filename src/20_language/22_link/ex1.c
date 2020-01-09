#include <stdio.h>

#include "ex1.h"


int f(int);
int f2(void);
int static_value(int);

// global scopeで定義している
int x = NUMBER;
int y = 2;
int z = 999;
// 暗黙のextern
// 宣言だけしていて、f2.cで定義している
int a;



int main(void) {
  printf("x=%d\n", x);
  printf("f(x)=%d\n", f(x));
  printf("f2(void)=%d\n", f2());
  printf("main f2 a=%d\n", a);
  printf("static_value(z)=%d\n", static_value(z));
  return 0;
}
