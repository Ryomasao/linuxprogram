
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "web.h"

#define LINE_BUF_SIZE 4096
static void read_request_line(FILE *in);

// HTTPヘッダのRFC
// https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
HTTPRequest* read_request(FILE *in) {
  HTTPRequest *req;
  read_request_line(in);
  // req = xmalloc(sizeof(HTTPRequest));

  return req;
}

// HTTPヘッダの一行目はRequest-Lineと呼ばれる
// METHOD Request-URI HTTP_Version 
// Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
static void read_request_line(FILE *in) {
  char buf[LINE_BUF_SIZE];
  char *p;


  // fgetsは、改行コードを意識して1行とってくる
  if(!fgets(buf, LINE_BUF_SIZE, in)) 
    log_exit(ERROR_NO_REQUEST_LINE, "no request line");

  printf("%s", buf);
}
