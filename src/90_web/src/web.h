// common
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// errorCode
enum ERROR_CODE {
  ERROR_ALLOCATE_MEMORY = 10,
  ERROR_NO_REQUEST_LINE = 20,
  ERROR_METHODS_DOES_NOT_EXISTS,
  ERROR_PATH_DOES_NOT_EXISTS,
  ERROR_PROTOCOL_VERSION_DOSE_NOT_EXIST,
  ERROR_CONTENT_LENGTH_IS_NEGATIVE,
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