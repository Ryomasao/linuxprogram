#include "web.h"

static void service(FILE *in, FILE *out, char *docroot);
static int listen_socket(char *port);

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
  // TODO ポート番号をオプションで受け取るようにする
  listen_socket("8888");
  service(tmpStdin, stdout, "./docroot");
  exit(0);
}

static void service(FILE *in, FILE *out, char *docroot) {
  HTTPRequest *req;
  req = read_request(in);
  response_to(req, out, docroot);
  // TODO free_request
}

static int listen_socket(char *port) {
  struct addrinfo hints, *res, *ai;
  int err;

  memset(&hints, 0, sizeof(struct addrinfo));

  // サーバサイドでsocketの設定をするときは以下の設定をするみたい
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // getaddrinfoは、ホスト名、ポート番号から使えるIPが含まれる構造体のリストを返してくれる。
  // サーバサイドときは、ホスト名はNULLでいいっぽい。
  // 使えるIPっていっても、大抵はひとつらしい。どういうときに複数になるのかしら。NICが複数ある場合？
  // イメージとして、ホスト名とポートがベースにあって、hintsを与えることで絞り込むような感じっぽい。
  if((err = getaddrinfo(NULL, port, &hints, &res)) != 0)
    log_exit(ERROR_CANNOT_ASSIGN_IP, (char *)gai_strerror(err));

  // getaddrinfoで取得した構造体をもとに、socketとして使えるアドレスを探す
  for(ai = res; ai; ai = ai->ai_next) {
    int sock;
    // sokcet→bind→listen→acceptの詳細な話は一旦置いておく
    sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

    if(sock < 0)
      continue;

    // portがすでにつかわれているとbindで失敗するっぽい
    if(bind(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
      close(sock);
      continue;
    }

    if(listen(sock, MAX_BACKLOG) < 0) {
      close(sock);
      continue;
    }

    // getaadrinfoで取得した構造体は、リンクリストになっているので？解放するときに専用の関数が存在している。
    freeaddrinfo(res);
    return sock;
  }

  log_exit(ERROR_CANNOT_LISTEN_SOCKET, "failed to listen socket");
  // log_exitで終了するため、returnされることはないが、型をあわせる
  return -1;
}
