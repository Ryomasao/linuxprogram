#include "web.h"

#define USAGE "Usage: %s [--port=n] [--chroot --user=u --group=g] [--debug] <docroot>\n"

// sighandler_t型を定義する
// sighandlet_t型は、intを引数にとりvoidを返す関数へのポインタ
typedef void (*sighandler_t)(int);
static void trap_signal(int sig, sighandler_t handler);
static void install_signal_handlers();
static void signal_exit(int sig);
static void service(FILE *in, FILE *out, char *docroot);
static void listen_request(int server_fd, char *docroot);
static int listen_socket(char *port);
static void become_daemon();

// vscodeでのdebugを想定
static int debug_mode = 0;
// テストコードでの実行を想定
static int test_mode = 0;
static struct option longopts[] = {
    {"debug", no_argument, &debug_mode, 1},  {"test", no_argument, &test_mode, 1},
    {"chroot", no_argument, NULL, 'c'},      {"user", required_argument, NULL, 'u'},
    {"group", required_argument, NULL, 'g'}, {"port", required_argument, NULL, 'p'},
    {"help", no_argument, NULL, 'h'},        {0, 0, 0, 0}};

// daemonになるための関数
// dameon(3)を使うと、以下の処理はいらなくなる
static void become_daemon() {
  int n;

  // プロセスがカレントディレクトリにしているディレクトリは、プロセスが実行中、アンマウントできない。
  // なのでルートディレクトリに移動しておくとよいらしい。
  // ルートディレクトリはアンマウントするとないからっていう前提？
  if(chdir("/") < 0)
    log_exit(ERROR_CHDIR_FAILED, "chdir(2) failed:%s", strerror(errno));

  // 標準入出力につながらないようにしておく
  freopen("/dev/null", "r", stdin);
  freopen("/dev/null", "w", stdout);
  freopen("/dev/null", "w", stderr);

  // 後述のsetsidをする際に、自分がプロセスグループリーダーにならないようにforkしとく
  n = fork();
  if(n < 0)
    log_exit(ERROR_FORK_FAILED, "fork(2) failed: %s", strerror(errno));

  // forkした際、親プロセスは不要なので終了
  // exit(3)だと、FILEに対してfflushが走るからとのこと。子プロセスで参照しているFILEに関しても走っちゃうよってことかな？
  // それがどういう悪影響があるのか謎。
  // socketに書き込んだデータがいつクライアントに送信されるのかが、あんまりわかってない。
  // fflushしたからといって、すぐに送るかというとそんなことはなさそう。
  // socketにはsocketのバッファを持っている。
  // https://stackoverflow.com/questions/914286/what-does-it-mean-to-flush-a-socket
  if(n != 0)
    _exit(0);

  // プロセスの関係を復習する
  // - 全てのプロセスは、なんからのプロセスから生成されていて、親子関係にある
  // - 最初のプロセスはsystemd
  // - プロセスグループとセッションっていう概念もある
  // - プロセスグループはシェルのために存在していて、コマンドをまとめて実行したときのグループ
  // - セッションは、ユーザーが端末からログイン〜ログアウトするまでの管理するグループ
  // - ps j コマンドで、PPID(親のPID) PID PGID SID　ttyを参照できる
  // - ttyは制御端末
  // - PIDとPGIDが等しい場合、プロセスグループリーダー、SIDとPIDが等しければ、セッションリーダー
  // - リーダーは、新しいグループや、セッションをつくれない

  // 長くなったけど、forkして、リーダーではないプロセスになった上でsetsidで新しいセッションをつくる
  // 新しいセッションなので、ユーザーがログインしたセッションが終了しても(ログアウトとか)プロセスが生き続けることができる
  if(setsid() < 0)
    log_exit(ERROR_SETSID_FAILED, "setsid(2) failed: %s", strerror(errno));
}

static void install_signal_handlers() {
  // コネクションが切断された後のsocketにwriteを行うと、SIGPIPEが発生するとのこと
  trap_signal(SIGPIPE, signal_exit);
}

static void signal_exit(int sig) {
  //
  log_exit(12, "exit by signal %d", sig);
}

static void trap_signal(int sig, sighandler_t handler) {
  struct sigaction act;
  act.sa_handler = handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_RESTART;
  if(sigaction(sig, &act, NULL) < 0) {
    log_exit(ERROR_SIGACTION_FAILED, "sigaction() failed: %s", strerror(errno));
  }
}

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
    install_signal_handlers();

    int server_fd;
    server_fd = listen_socket("8888");
    printf("webserer listen on %d\n", 8888);
    fflush(stdout);

    become_daemon();

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

    // accept()はsocketにつながっているファイルディスクリプタをもとに、そのsocketに接続がくるまで待つ。
    sock = accept(server_fd, (struct sockaddr *)&addr, &addrlen);
    if(sock < 0)
      log_exit(ERROR_CANNOT_ACEEPT_SOCKET, "accept(2) failed: %s", strerror(errno));

    pid = fork();
    // forkに失敗したときはlog_exitじゃないのはなぜかしら。
    if(pid < 0)
      exit(3);

    // child process
    if(pid == 0) {
      // fdopen()はファイルディスクリプタから、FILE型に変換するためのもの
      // これにより、後続のserviceはつながっている先が標準入力なのか、socketなのかを意識しなくてよくなる。
      FILE *in = fdopen(sock, "r");
      FILE *out = fdopen(sock, "w");
      service(in, out, docroot);
      exit(0);
    }
    // accetptで接続済のsocketは親プロセスでもcloseをする必要がある。
    // forkして出来た子のプロセスはexit()で済むけど、親側は起動しつづけるので、別途closeする。
    // ※ forkしてもsocketが複製されるわけではなくって、親と子でそれぞれ参照しているイメージ？
    //   参照しているプロセス全部でもう使わないよ状態にしとかないとsocketが残りっぱなしになる。
    close(sock);
  }
}

static void service(FILE *in, FILE *out, char *docroot) {
  HTTPRequest *req;
  req = read_request(in);
  response_to(req, out, docroot);
  free_request(req);
}

static int listen_socket(char *port) {
  struct addrinfo hints, *res, *ai;
  int err;

  memset(&hints, 0, sizeof(struct addrinfo));

  // サーバサイドでsocketの設定をするときは以下の設定をするみたい
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // getaddrinfoは、ホスト名、ポート番号から使えるアドレスが含まれる構造体のリストを返してくれる。
  // サーバサイドときは、ホスト名はNULLでいいっぽい。
  // 使えるアドレスっていっても、大抵はひとつらしい。どういうときに複数になるのかしら。NICが複数ある場合？
  // イメージとして、ホスト名とポートがベースにあって、hintsを与えることで絞り込むような感じっぽい。
  if((err = getaddrinfo(NULL, port, &hints, &res)) != 0)
    log_exit(ERROR_CANNOT_ASSIGN_IP, (char *)gai_strerror(err));

  // getaddrinfoで取得した構造体をもとに、socketとして使えるアドレスを探す
  for(ai = res; ai; ai = ai->ai_next) {
    int sock;
    // socket()はsocketを作成し、それにつながるファイルディスクリプタを返す。
    // ファイルディスクリプタは、カーネルがストリームに接続するためにアプリに見せてくれているもの。
    // socketはストリームとはちょっと違う扱いなのかもしれないけど、ファイルディスクリプタ経由で参照できる。
    sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

    if(sock < 0)
      continue;

    // bind()は接続を待つアドレスをsock()で作成したsocketに割り当てる。
    // portがすでにつかわれているとbindで失敗するっぽい
    if(bind(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
      close(sock);
      continue;
    }

    // listen()は、このsocketは接続を待ってるんだよ！ってカーネルに伝えるためのもの。
    /// 第二引数は、このsocketが同時に受け付けるコネクション数とのこと。どういうことだろう。
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
