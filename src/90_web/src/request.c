
#include "web.h"

#define LINE_BUF_SIZE 4096
static void read_request_line(FILE *in, HTTPRequest *req);

// HTTPヘッダのRFC
// https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
HTTPRequest *read_request(FILE *in) {
  HTTPRequest *req;
  req = xmalloc(sizeof(HTTPRequest));
  read_request_line(in, req);

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

  p = strchr(buf, ' ');
  if(!p) {
    log_exit(ERROR_METHODS_DOES_NOT_EXISTS,
             "expected space on request line, but not exist");
  }
  // *p++ = '/0'でシンプルに書ける
  // *p++ だからポインタに対する演算じゃないようにみえるけれど
  *p = '\0';
  p++;
  req->method = xmalloc(p - buf);
  strcpy(req->method, buf);
  upcase(req->method);

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

  if(strncasecmp(p, "HTTP/1.", strlen("HTTP/1.")) != 0)
    log_exit(ERROR_PROTOCOL_VERSION_DOSE_NOT_EXIST,
             "expected protocol version on request line, but not exist");

  p += strlen("HTTP/1.");
  req->protocol_minor_version = atoi(p);
  printf("%s %s %d", req->method, req->path, req->protocol_minor_version);
}
