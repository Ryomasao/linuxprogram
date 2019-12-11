// common
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <netdb.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

// define
#define SERVER_NAME "FIRST_HTTPD"
#define SERVER_VERSION "1.0"

#define MAX_BACKLOG 5

// global variables
// vscodeでのdebugを想定
extern int global_debug_mode;
// テストコードでの実行を想定
extern int global_test_mode;

// errorCode
// exit(3)の引数int型だけど、0~255を超える値を設定するとオーバーフローする
// TODO エラーのカテゴリでわけたほうがよさそう
enum PARAM_ERROR_CODE {
  ERROR_INVALID_PARAM = 10,
};

enum ERROR_CODE {
  // common error
  ERROR_ALLOCATE_MEMORY = 10,
  ERROR_SIGACTION_FAILED = 11,
  ERROR_CHDIR_FAILED = 12,
  ERROR_FORK_FAILED = 13,
  ERROR_SETSID_FAILED = 14,
  // About Request Header
  ERROR_NO_REQUEST_LINE = 20,
  ERROR_METHODS_DOES_NOT_EXISTS,
  ERROR_PATH_DOES_NOT_EXISTS,
  ERROR_PROTOCOL_VERSION_DOSE_NOT_EXIST,
  ERROR_CONTENT_LENGTH_IS_NEGATIVE,
  ERROR_BODY_LENGTH_TOO_LONG,
  ERROR_CANNOT_READ_BODY,
  ERROR_CANNOT_OPEN_FILE,
  ERROR_CANNOT_READ_FILE,
  ERROR_CANNOT_WRITE_FILE,
  // About Network Error
  ERROR_CANNOT_ASSIGN_IP = 100,
  ERROR_CANNOT_LISTEN_SOCKET = 101,
  ERROR_CANNOT_ACEEPT_SOCKET = 102,
  ERROR_SYSTEM_ERRRO = 255
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
void free_request(HTTPRequest *req);

// response.c
void response_to(HTTPRequest *req, FILE *out, char *docroot);

typedef struct {
  char *path;
  long size;
  int ok; // pathが存在しない、許可されたファイルではない場合、0が設定。
} FileInfo;
