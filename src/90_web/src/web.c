#include "web.h"

static void service(FILE *in, FILE *out, char *docroot);

int main(int argc, char *argv[]) {

  FILE *tmpStdin = stdin;

  // vscodeのデバックで標準入力を扱える方法がわからないので、ひとまず
  if(argv[1]) {
    FILE *debugStdin = fopen("./test_data/HEADER.txt", "r");
    if(!debugStdin)
      log_exit(99, " file open error");

    tmpStdin = debugStdin;
  }

  // TODO 引数のチェック
  // TODO signal補足処理の初期設定
  service(tmpStdin, stdout, "docroot");
  exit(0);
}

static void service(FILE *in, FILE *out, char *docroot) {
  // TODO リクエストパース
  read_request(in);
  // TODO レスポンス処理
}
