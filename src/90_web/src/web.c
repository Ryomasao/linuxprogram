#include "web.h"

static void service(FILE *in, FILE *out, char *docroot);
static void listen_request(int server_fd, char *docroot);
static int listen_socket(char *port);

#define USAGE "Usage: %s [--port=n] [--chroot --user=u --group=g] [--debug] <docroot>\n"

// vscodeでのdebugを想定
static int debug_mode = 0;
// テストコードでの実行を想定
static int test_mode = 0;
static struct option longopts[] = {
    {"debug", no_argument, &debug_mode, 1},  {"test", no_argument, &test_mode, 1},
    {"chroot", no_argument, NULL, 'c'},      {"user", required_argument, NULL, 'u'},
    {"group", required_argument, NULL, 'g'}, {"port", required_argument, NULL, 'p'},
    {"help", no_argument, NULL, 'h'},        {0, 0, 0, 0}};

int main(int argc, char *argv[]) {

  int opt;

  while((opt = getopt_long(argc, argv, "", longopts, NULL)) != -1) {
    // TODO ポート番号等をオプションで受け取るようにする
    // TODO getoptの使い方をちゃんとみる
    switch(opt) {
    case 0:
      break;
    case 'u':
      break;
    case 'h':
      fprintf(stdout, USAGE, argv[0]);
      exit(0);
    case '?':
      fprintf(stderr, USAGE, argv[0]);
      exit(1);
    }
  }

  if(debug_mode && test_mode)
    log_exit(ERROR_INVALID_PARAM, "you can not use debug and test mode together");

  if(debug_mode) {
    // vscodeのデバックで標準入力を扱える方法がわからないので、ひとまず
    FILE *debugStdin = fopen("./test_data/HEADER.txt", "r");
    if(!debugStdin)
      log_exit(99, " file open error");

    service(debugStdin, stdout, "./docroot");
  }

  if(test_mode) {
    service(stdin, stdout, "./docroot");
  }

  if(!(debug_mode || test_mode)) {
    // TODO signal補足処理の初期設定
    int server_fd;
    server_fd = listen_socket("8888");

    printf("webserer listen on %d\n", 8888);
    fflush(stdout);

    listen_request(server_fd, "./docroot");
  }

  exit(0);
}

static void listen_request(int server_fd, char *docroot) {
  for(;;) {
    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof addr;
    int sock;
    int pid;

    // ファイルディスクリプターのserver_fd(socket)から、
    // accept状態のファイルディスクリプター(socket)を取得する
    sock = accept(server_fd, (struct sockaddr *)&addr, &addrlen);
    if(sock < 0)
      log_exit(ERROR_CANNOT_ACEEPT_SOCKET, "accept(2) failed: %s", strerror(errno));

    pid = fork();
    // TODO forkに失敗したときなぜexitを直接呼ぶのか調べる
    if(pid < 0)
      exit(3);

    // child process
    if(pid == 0) {
      FILE *in = fdopen(sock, "r");
      FILE *out = fdopen(sock, "w");
      service(in, out, docroot);
      exit(0);
    }
    close(sock);
  }
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
