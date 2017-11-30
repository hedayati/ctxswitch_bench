#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <ucontext.h>
#include <unistd.h>

#define iterations 1000000

void first_ctx_func(int sock) {
  int c = 0;
  struct timespec start, end;
  unsigned long diff;
  char buf[1];

  clock_gettime(CLOCK_MONOTONIC, &start);

  while (c < iterations) {
    if (read(sock, buf, 1) != 1) {
      perror("first_ctx_func could not read");
      exit(1);
    }
    c++;
    buf[0] = 'c';

    if (write(sock, buf, 1) != 1) {
      perror("first_ctx_func could not write");
    }
  }

  buf[0] = 'e';
  write(sock, buf, 1);

  clock_gettime(CLOCK_MONOTONIC, &end);
  diff = 1E9 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
  printf("elapsed time = %llu nanoseconds for fork-socket.\n",
         (long long unsigned int)diff / iterations);
}

void second_ctx_func(int sock) {
  char buf[1] = {0};
  for (; buf[0] != 'e';) {
    buf[0] = 'p';
    if (write(sock, buf, 1) != 1) {
      perror("second_ctx_func could not write");
    }
    if (read(sock, buf, 1) != 1) {
      perror("second_ctx_func could not read");
      exit(1);
    }
  }
}

int main(int argc, char const *argv[]) {
  int sfd[2];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sfd))
    perror("failed to create sockets");

  if (fork()) {
    close(sfd[0]);
    first_ctx_func(sfd[1]);
  } else {
    close(sfd[1]);
    second_ctx_func(sfd[0]);
  }

  return 0;
}