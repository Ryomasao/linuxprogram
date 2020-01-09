#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
  pid_t pid;
  char *argv[3] = {"node", "sample.js", NULL};
  int fds[2];
  char buf[100];

  // pipeは、自分のプロセスへの読み取りと書き込み用のファイルディスクリプタを返してくれる。
  // fds[0]はread fds[1]はwrite
  pipe(fds);
  pid = fork();

  if(pid < 0) {
    fprintf(stderr, "fork(2) failed\n");
    exit(1);
  }

  if(pid == 0) {
    // 子プロセスの場合

    // pipeで作成したファイルディスクリプタはforkした場合、そのまま子プロセスに複製される。
    // これにより、子プロセスで書き込み用ファイルディスクリプタに書いて、親プロセスで読み取り用のファイルディスクリプタを読むことでやりとりが可能。

    // 子プロセスではread用のファイルディスクリプタは使わないのでクローズする。
    close(fds[0]);
    // もともとの標準出力は使わないのでクローズ。
    close(STDOUT_FILENO);
    // 標準出力にpipeで作った書き込み用ファイルディスクリプタを割り当てる
    dup2(fds[1], STDOUT_FILENO);

    execve("/usr/bin/node", argv, NULL);

    printf("execveが成功しているのであれば、このコードは実行されない\n");
    exit(1);
  } else {
    // 親プロセス
    int status;
    // 親プロセスは、write用のファイルディスクリプタは使わないのでクローズする。
    close(fds[1]);
    // 子プロセスの終了を待つ
    wait(&status);
    // 子プロセスが書き出した結果を読み込む
    read(fds[0], buf, 100);

    printf("parent fds[1]: %s\n", buf);
  }

  return 0;
}