#include "fcgi_stdio.h"
#include <stdlib.h>
#include <unistd.h>

void main(void) {
  int count = 0;
  pid_t pid = getpid();
  while(FCGI_Accept() >= 0)
    printf("Content-type: text/html\r\n"
           "\r\n"

           "<title>FastCGI Hello!</title>"
           "<h1>FastCGI Hello!</h1>"
           "<h2>pid:%d</h2>"
           "Request number %d running on host <i>%s</i>\n",
           pid, ++count, getenv("SERVER_NAME"));
}