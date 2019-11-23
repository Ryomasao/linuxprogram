#include <stdio.h>
#include <string.h>

int main(void) {
  char str[] = "12345";
  int len = strspn(str, "4512");
  printf("%d\n", len);
  return 0;
}