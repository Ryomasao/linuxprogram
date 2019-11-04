#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
  pid_t pid;
  pid = fork();

  if (pid < 0)
  {
    fprintf(stderr, "fork(2) failed\n");
    exit(1);
  }

  if (pid == 0)
  {
    printf("im child, my-pid=%d, child-pid=%d\n", getpid(), pid);
  }
  else
  {
    printf("im parent, my-pid=%d, child-pid=%d\n", getpid(), pid);
  }

  return 0;
}