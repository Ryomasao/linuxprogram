#include "web.h"

static void service(FILE *in, FILE *out, char *docroot);
static void listen_socket(char *port);

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
  service(tmpStdin, stdout, "./docroot");
  exit(0);
}

static void service(FILE *in, FILE *out, char *docroot) {
  HTTPRequest *req;
  req = read_request(in);
  response_to(req, out, docroot);
  // TODO free_request
}

static void listen_socket(char *port) {
  struct addrinfo hints, *res, *ai;
  int err;

  memset(&hints, 0, sizeof(struct addrinfo));

  // サーバサイドでsocketの設定をするときは以下の設定をするみたい
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // getaddrinfoは、
  if((err = getaddrinfo(NULL, port, &hints, &res)) != 0) {
    // TODO
  }
}
