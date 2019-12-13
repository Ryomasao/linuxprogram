#define _GNU_SOURCE

#include <stdio.h>

void main() {
// gccのオプションにstd=c11ををつけた場合、__USE_POSIXは定義されていなかった
// しかし_GNU_SOURCEを定義することで、__USE_POSIXも同時に定義されるようになるっぽい
// stdのオプションはデフォルトgnu90なるものっぽい。
#ifdef __USE_POSIX
  printf("defined!\n");
#endif
  return;
}