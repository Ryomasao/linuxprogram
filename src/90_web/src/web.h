// common
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

// define
#define SERVER_NAME "FIRST_HTTPD"
#define SERVER_VERSION "1.0"

// errorCode
enum ERROR_CODE {
  ERROR_ALLOCATE_MEMORY = 10,
  ERROR_NO_REQUEST_LINE = 20,
  ERROR_METHODS_DOES_NOT_EXISTS,
  ERROR_PATH_DOES_NOT_EXISTS,
  ERROR_PROTOCOL_VERSION_DOSE_NOT_EXIST,
  ERROR_CONTENT_LENGTH_IS_NEGATIVE,
  ERROR_CANNOT_READ_BODY,
  ERROR_SYSTEM_ERRRO = 999
};

// util.c
void log_exit(int exitCode, char *fmt, ...);
void *xmalloc(size_t sz);
void upcase(char *str);

// request.c
typedef struct HTTPHeaderField {
  char *name;
  char *value;
  struct HTTPHeaderField *next;
} HTTPHeaderField;

typedef struct {
  int protocol_minor_version;
  char *method;
  char *path;
  HTTPHeaderField *header;
  char *body;
  long length;
} HTTPRequest;

HTTPRequest *read_request(FILE *in);

// response.c
void response_to(HTTPRequest *req, FILE *out, char *docroot);

typedef struct {
  char *path;
  long size;
  int ok; // pathが存在しない、許可されたファイルではない場合、0が設定。
} FileInfo;
