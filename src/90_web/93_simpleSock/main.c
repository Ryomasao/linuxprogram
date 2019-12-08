#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// シンプルなsocket
int main() {
  struct addrinfo hints, *res;
  int err;
  int sock;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if((err = getaddrinfo(NULL, "8000", &hints, &res)) != 0)
    exit(-1);

  printf("create socket\n");
  sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  if(sock < 0)
    exit(-2);

  printf("bind socket\n");
  if(bind(sock, res->ai_addr, res->ai_addrlen) < 0) {
    close(sock);
    exit(-3);
  }

  printf("listen socket\n");
  if(listen(sock, 5) < 0) {
    close(sock);
    exit(-4);
  }

  struct sockaddr_storage addr;
  socklen_t addrlen = sizeof addr;

  freeaddrinfo(res);

  printf("accept socket\n");
  int listendSock = accept(sock, (struct sockaddr *)&addr, &addrlen);
  if(listendSock < 0)
    exit(-5);

  return 0;
}