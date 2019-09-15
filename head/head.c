#include <stdio.h>
#include <stdlib.h>

static void do_head(FILE *f, long nlines);

int main(int argc, char *argv[])
{
  // ./head 2
  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s n\n", argv[0]);
  }

  do_head(stdin, atol(argv[1]));

  exit(0);
}

static void do_head(FILE *f, long nlines)
{
  int c;
  while ((c = fgetc(f)) != EOF)
  {
    if (putchar(c) < 0)
      exit(-1);

    if (c == '\n')
      nlines--;

    if (nlines == 0)
      return;
  }
}