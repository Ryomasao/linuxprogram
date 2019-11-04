
#include <stdio.h>
#include <stdlib.h>
#include "web.h"

static void read_request_line(FILE *in);

HTTPRequest* read_request(FILE *in) {
  HTTPRequest *req;
  read_request_line(in);
  // req = xmalloc(sizeof(HTTPRequest));

  return req;
}

static void read_request_line(FILE *in) {
  char buf[LINE_BUF_SIZE];
  // fgetsは、改行コードを意識して1行とってくる
  if(!fgets(buf, LINE_BUF_SIZE, in)) {
    log_exit(ERROR_NO_REQUEST_LINE, "no request line");
  } 
  printf("%s", buf);
}
