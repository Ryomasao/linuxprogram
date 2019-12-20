#include <stdio.h>

#define APR_HOOK_REALLY_FIRST (-10)
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define MODULE(foo) int foo##_module

int hoge_module;
MODULE(hoge) = 1;

int main() {
  // オブジェクト形式のマクロ。ただの置換。
  int a = APR_HOOK_REALLY_FIRST;

  // 関数形式のマクロ
  int b = MAX(1, 2);

  printf("hoge=%d", hoge_module);

  return 0;
}