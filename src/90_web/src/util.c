#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "web.h"

void log_exit(int exitCode, char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fputc('\n', stderr);
  va_end(ap);
  exit(exitCode);
}

void *xmalloc(size_t sz)
{
  void *p;
  p = malloc(sz);
  if (!p)
    log_exit(ERROR_ALLOCATE_MEMORY, "failed to allocate memory");
  return p;
}