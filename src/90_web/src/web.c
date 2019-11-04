#include <stdio.h>
#include <stdlib.h>
#include "web.h"

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
  read_request(in);
  // TODO レスポンス処理
}
