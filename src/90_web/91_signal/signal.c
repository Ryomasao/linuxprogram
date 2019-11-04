#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void got_signal(int sig);
typedef void (*sighandler_t)(int);
sighandler_t trap_signal(int sig, sighandler_t handler);

// SIGINT(ctrl + c)で特定の処理を実行するようにする処理:
int main(int argc, char *argv[])
{
  trap_signal(SIGINT, got_signal);
  for (;;)
  {
    sleep(1);
    printf("Hello World \n");
  }

  // loopではなくシグナルを待つ、以下のAPIを使うと便利
  // pause()

  exit(0);
}

void got_signal(int sig)
{
  printf("Got the signal:%d\n", sig);
  exit(1);
}

// この記事がとてもわかりやすかった。
// https://harasou.jp/2017/01/23/linux-signal/
// シグナルを補足する関数sigactionのWrapper
sighandler_t trap_signal(int sig, sighandler_t handler)
{
  // 新しい設定
  struct sigaction act;
  // 元々の設定
  struct sigaction old;
  // sigに指定されたシグナルが発生した場合に実行する関数
  act.sa_handler = handler;
  // いろんなシグナルが同時に発生したときに、ブロックするシグナルを指定する
  // ブロックされたシグナルは、保留状態になる。アンブロックされるまで、ずっと保留。
  // sigactionでは、sigactionで登録したハンドルが実行中の間は、
  // そもそも他のシグナルをブロックするとのことなので、ほとんどの場合、個別に指定しなくてもよいとのこと。
  // マスクの設定には、専用の関数が用意されているので、こちらを使う。
  // 以下は、マスクを空っぽにする設定。
  sigemptyset(&act.sa_mask);
  // この設定がちょっとよくわからない。システムコールが再起動されるってどういうことだろう。
  act.sa_flags = SA_RESTART;

  if (sigaction(sig, &act, &old) < 0)
  {
    return NULL;
  }

  return old.sa_handler;
}
