#include <stdio.h>

#include "ex1.h"

extern int y;
int y;

// global scopeにzがあるが、staticをつけることで、重複ではなくなる
static int z = 888;
//extern int z;

int f(int x) {
  extern int y;
  x = x * NUMBER;
  return x * y;
}

// この関数は、グローバル変数のzではなく、このファイルスコープの変数zを返す
int static_value(int arg_z) {
  //arg_z + 1;
  return z; // return 888
}