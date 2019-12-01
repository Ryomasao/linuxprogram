#include "web.h"

static void do_file_response(HTTPRequest *req, FILE *out, char *docroot);
static FileInfo *get_fileinfo(char *urlpath, char *docroot);
static char *build_fspath(char *urlpath, char *docroot);
static void free_fileinfo(FileInfo *info);
static void not_found(HTTPRequest *req, FILE *out);
static void output_common_header_fields(HTTPRequest *req, FILE *out, char *status);

void response_to(HTTPRequest *req, FILE *out, char *docroot) {
  if(strcmp(req->method, "GET") == 0) {
    do_file_response(req, out, docroot);
  } else {
    // TODO
    log_exit(999, "NOT IMPLEMENTED");
  }
}

static void do_file_response(HTTPRequest *req, FILE *out, char *docroot) {
  FileInfo *info;
  info = get_fileinfo(req->path, docroot);

  // 対象のファイルが存在しない場合
  if(info->ok == 0) {
    free_fileinfo(info);
    not_found(req, out);
    return;
  }

  // TODO ファイルの内容を出力
  printf("FILE_INFO\n path:%s \n ok: %d\n", info->path, info->ok);
  free_fileinfo(info);
}

// urlpathで指定しているファイルの付帯情報を取得し、Fileinfoにセットして返す
// ファイルが存在しない場合、Fileinfo->okに0をセットする
static FileInfo *get_fileinfo(char *urlpath, char *docroot) {
  FileInfo *info;
  struct stat st;

  info = xmalloc(sizeof(FileInfo));
  info->path = build_fspath(urlpath, docroot);
  info->ok = 0;
  // statとlstatはファイルの付帯情報を得るシステムコール
  // 渡したパスが、シンボリックリンクとき、statはリンクの参照先の情報を返す。
  // lsstatはシンボリックリンクそのものの情報を返す。
  if(lstat(info->path, &st) < 0)
    return info;

  // S_ISREGはstatで取得した付帯情報からファイルなのかを判定してくれるマクロ。
  // 権限までは見てない
  if(!S_ISREG(st.st_mode))
    return info;

  info->size = st.st_size;
  info->ok = 1;

  return info;
}

// docrootとurlpathから、取得するファイルのパスを組み立てる
static char *build_fspath(char *urlpath, char *docroot) {
  char *path;
  // docrootの末尾の / + urlpath末尾の終端文字列分
  path = xmalloc(strlen(urlpath) + 1 + strlen(docroot) + 1);
  // docroot//urlpathになるような気がするが問題はない？
  sprintf(path, "%s/%s", docroot, urlpath);
  return path;
}

static void free_fileinfo(FileInfo *info) {
  free(info->path);
  free(info);
}

static void not_found(HTTPRequest *req, FILE *out) {
  output_common_header_fields(req, out, "404 Not Found");
  fprintf(out, "Content-Type: text/html\r\n");
  fprintf(out, "\r\n");

  fprintf(out, "<html>\r\n");

  fprintf(out, "<header>\r\n");
  fprintf(out, "<title>Not Found</title>\r\n");
  fprintf(out, "</header>\r\n");

  fprintf(out, "<body>\r\n");
  fprintf(out, "<p>File Not found</p>\r\n");
  fprintf(out, "</body>\r\n");

  fprintf(out, "</html>\r\n");

  fflush(out);
}

#define TIME_BUF_SIZE 64
static void output_common_header_fields(HTTPRequest *req, FILE *out, char *status) {
  time_t t;
  struct tm *tm;
  char buf[TIME_BUF_SIZE];

  // UNIXエポック(1970/1/1 00:00:00)からの現在時刻の経過秒数を返す。
  // OSXだとman 3 time でいるけど、本にはtime(2)になってる。システムコールかも。
  // 時刻は返り値の他、引数にセットしたtime_t型の構造体に渡すことでも取得できるみたい
  t = time(NULL);

  // time関数で取得した秒数をもとに、年月日の情報をもつ構造体の形式に変換してくれる
  // gmtimeは常にUTC時間。ロケールを気にするならlocaltimeを使う。
  // 設定で変更できるようにするなら、自分で実装する必要がある。
  tm = gmtime(&t);
  if(!tm)
    log_exit(ERROR_SYSTEM_ERRRO, "gmtime() failed: %s", strerror(errno));

  strftime(buf, TIME_BUF_SIZE, "%a, %d %b %Y %H:%M:%S GMT", tm);
  fprintf(out, "HTTP/1.%d %s\r\n", req->protocol_minor_version, status);
  fprintf(out, "Date: %s\r\n", buf);
  fprintf(out, "Server: %s/%s\r\n", SERVER_NAME, SERVER_VERSION);
  fprintf(out, "Connection: close\r\n");
}
