#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  for (int i = 1; i < argc; i++)
  {
    FILE *f;
    f = fopen(argv[i], "r");

    if (!f)
    {
      // システムコールでエラーが発生すると、グローバル変数errornoにエラーに対応する番号がセットされる
      // perrorはerrornoに対応するメッセージを標準出力に出力する
      // 引数にはエラーの原因となった文字列を渡しとくと、一緒に標準出力に出してくれるっぽい
      perror(argv[i]);
      exit(-1);
    }

    int c;
    while ((c = fgetc(f)) != EOF)
    {
      if (putchar(c) < 0)
        exit(1);
    }

    fclose(f);
  }

  // exitにstdlibが必要
  exit(0);
}