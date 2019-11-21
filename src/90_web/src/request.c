
#include "web.h"

// def
#define LINE_BUF_SIZE 4096
static void read_request_line(FILE *in, HTTPRequest *req);
static HTTPHeaderField *read_header_field(FILE *in);

// implement

// HTTPヘッダのRFC
// https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
HTTPRequest *read_request(FILE *in) {
  HTTPRequest *req;
  HTTPHeaderField *h;

  req = xmalloc(sizeof(HTTPRequest));
  read_request_line(in, req);

  req->header = NULL;

  while(h = read_header_field(in)) {
    // 一方向リストが理解するのに時間がかかる
    // 0800  req->header  →0900
    // 0900  h->next -> 1000
    // 1000  h->next -> NULL
    h->next = req->header;
    req->header = h;
  }

  return req;
}

// HTTPヘッダの一行目はRequest-Lineと呼ばれる
// METHOD Request-URI HTTP_Version
// Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
static void read_request_line(FILE *in, HTTPRequest *req) {
  char buf[LINE_BUF_SIZE];
  char *p;
  char *path;

  // fgetsは、改行コードを意識して1行とってくる
  if(!fgets(buf, LINE_BUF_SIZE, in))
    log_exit(ERROR_NO_REQUEST_LINE, "no request line");

  //    ↓このスペースまで移動
  // GET /hoge/fuga HTTP/1.1
  p = strchr(buf, ' ');
  if(!p) {
    log_exit(ERROR_METHODS_DOES_NOT_EXISTS,
             "expected space on request line, but not exist");
  }
  // *p++ = '/0'でシンプルに書ける
  // *p++ だからポインタに対する演算じゃないようにみえるけれど不思議
  // スペース部分に終端文字列をセット
  *p = '\0';
  p++;
  req->method = xmalloc(p - buf);
  strcpy(req->method, buf);
  upcase(req->method);

  //               ↓このスペースまで移動
  // GET /hoge/fuga HTTP/1.1
  path = p;
  p = strchr(path, ' ');
  if(!p) {
    log_exit(ERROR_PATH_DOES_NOT_EXISTS,
             "expected space on request line, but not exist");
  }
  *p = '\0';
  p++;
  req->path = xmalloc(p - path);
  strcpy(req->path, path);

  //                       ↓最後にこのマイナーバージョンを取得する
  // GET /hoge/fuga HTTP/1.1
  if(strncasecmp(p, "HTTP/1.", strlen("HTTP/1.")) != 0)
    log_exit(ERROR_PROTOCOL_VERSION_DOSE_NOT_EXIST,
             "expected protocol version on request line, but not exist");

  p += strlen("HTTP/1.");
  req->protocol_minor_version = atoi(p);

  // printf("%s %s %d", req->method, req->path, req->protocol_minor_version);
}

static HTTPHeaderField *read_header_field(FILE *in) {
  char buf[LINE_BUF_SIZE];
  char *p;
  HTTPHeaderField *h = xmalloc(sizeof(HTTPHeaderField));

  if(!fgets(buf, LINE_BUF_SIZE, in))
    log_exit(-1, "failed to read request header %s", strerror(errno));

  if((buf[0] == '\n') || (strcmp(buf, "\r\n") == 0))
    return NULL;

  p = strchr(buf, ':');
  if(!p)
    log_exit(-1, "expected colon, but not existed. filed is %s", p);

  *p = '\0';
  p++;
  h->name = xmalloc(p - buf);
  strcpy(h->name, buf);

  // strspnがすごいややこしい
  // 文字列を先頭から順にkeywordのいずれかの文字に該当するかを調べる
  // 該当するまでの文字数を返す
  // ex) 文字列   keyword   結果
  //     12345    34512    5 先頭から5文字全部keywordに含まれる
  //     12345    4512     2 先頭から1,2がkeywordに含まれ、3がないので終了
  //     12345    2345     0 先頭の文字1がkeywordにないので終了
  // ここでやってるのは、 key:
  // valueのvalueの前のスペース、タブ文字を読み飛ばすこと
  // RFCのkey:valueにスペース、タブ文字があってもいいみたいな記載は見つけられなかった。
  p += strspn(p, " \t");
  h->value = xmalloc(strlen(p) + 1);
  strcpy(h->value, p);

  return h;
}
