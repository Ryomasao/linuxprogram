#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static void service(FILE *in, FILE *out, char *docroot);

int main(int argc, char *argv[])
{
  // TODO 引数のチェック
  // TODO signal補足処理の初期設定
  service(stdin, stdout, "docroot");
  exit(0);
}

static void service(FILE *in, FILE *out, char *docroot)
{
  // TODO リクエストパース
  // TODO レスポンス処理
}

// TODO エラーログ処理
//static void log_exit(char *fmt, ...)
//{
//  va_list ap;
//
//  va_start(ap, fmt);
//  vfprintf(stderr, fmt, ap);
//  fputc('\n', stderr);
//  va_end(ap);
//  exit(1);
//}
//

// TODO メモリアロケート処理
//static void *xmalloc(size_t sz)
//{
//  void *p;
//  p = malloc(sz);
//  if (!p)
//    log_exit("failed to allocate memory");
//  return p;
//}