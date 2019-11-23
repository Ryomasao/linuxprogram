#include "web.h"

void log_exit(int exitCode, char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fputc('\n', stderr);
  va_end(ap);
  exit(exitCode);
}

// メモリアロケート失敗時の処理を共通化できるから、個別の関数にしたほうがいいのかしら
void *xmalloc(size_t sz) {
  void *p;
  p = malloc(sz);
  if(!p)
    log_exit(ERROR_ALLOCATE_MEMORY, "failed to allocate memory");
  return p;
}

void upcase(char *str) {
  // strのアドレスは動かしたくない場合の常套手段
  // このへんサクッとかけるようになりたいね
  char *p;
  for(p = str; *p; p++) {
    *p = (char)toupper((int)*p);
  }
}