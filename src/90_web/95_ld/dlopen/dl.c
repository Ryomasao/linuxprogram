#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include "sub.h"

int main() {
  // dynamic link
  sub(10);

  // dynamic load
  void *handle;
  // http://itref.fc2web.com/unix/dlopen.html
  // 絶対パス名と相対パス名のどちらで指定しても構いません。
  // パス名ではなく、ファイル名だけを指定することもできます。ファイル名だけを指定した場合、環境変数LD_LIBRARY_PATHに設定されたディレクトリ群から指定された共有ライブラリを探します。
  handle = dlopen("libdyn.so", RTLD_LAZY);

  if(!handle) {
    printf("failed to dlopen\n");
    exit(-1);
  }

  void (*func)(int);
  func = dlsym(handle, "dynamic");

  if(!func) {
    printf("failed to dlsym\n");
    exit(-1);
  }

  func(1);

  return 0;
}
