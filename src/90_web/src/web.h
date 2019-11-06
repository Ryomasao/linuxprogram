//common

// errorCode
enum ERROR_CODE {
  ERROR_ALLOCATE_MEMORY = 10,
  ERROR_NO_REQUEST_LINE = 20,
};


//util.c
void log_exit(int exitCode, char *fmt, ...);
void* xmalloc(size_t sz);

//request.c
typedef struct {
  int protocol_minor_version;
  char *method;
  char *path;
  struct HTTPHeaderField *header;
  char *body;
  long length;
} HTTPRequest;

HTTPRequest* read_request(FILE *in);