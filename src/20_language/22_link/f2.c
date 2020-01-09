#include <stdio.h>

static int x;
int a = 2;
int f2(void) {

  // グローバル変数にaがあるけど、スコープが違うから大丈夫。
  int a = 1;
  printf("f2 a=%d\n", a);
  // extern int x = 300;
  return x;
}
