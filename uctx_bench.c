#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>
#include <ucontext.h>
#include <unistd.h>

static ucontext_t ctx[3];

#define iterations 1000000

void first_ctx_func() {
  int c = 0;

  while (c < iterations) {
    ++c;
    if (swapcontext(&ctx[1], &ctx[2]) == -1) {
      perror("failed to swapcontext in first_ctx_func");
      exit(0);
    }
  }
}

void second_ctx_func() {
  int c = 0;

  while (c < iterations) {
    ++c;
    if (swapcontext(&ctx[2], &ctx[1]) == -1) {
      perror("failed to swapcontext in second_ctx_func");
      exit(0);
    }
  }
}

int main(int argc, char const *argv[]) {
  struct timespec start, end;
  unsigned long diff;

  void *stack_first = mmap(NULL, 1 << 23, PROT_READ | PROT_WRITE,
                           MAP_ANON | MAP_PRIVATE, -1, 0);
  void *stack_second = mmap(NULL, 1 << 23, PROT_READ | PROT_WRITE,
                            MAP_ANON | MAP_PRIVATE, -1, 0);

  if (!stack_first || !stack_second) return -ENOMEM;

  getcontext(&ctx[1]);
  ctx[1].uc_stack.ss_sp = stack_first;
  ctx[1].uc_stack.ss_size = 1 << 23;
  ctx[1].uc_link = &ctx[0];
  makecontext(&ctx[1], first_ctx_func, 0);

  getcontext(&ctx[2]);
  ctx[2].uc_stack.ss_sp = stack_second;
  ctx[2].uc_stack.ss_size = 1 << 23;
  ctx[2].uc_link = &ctx[1];
  makecontext(&ctx[2], second_ctx_func, 0);

  clock_gettime(CLOCK_MONOTONIC, &start);

  swapcontext(&ctx[0], &ctx[2]);

  clock_gettime(CLOCK_MONOTONIC, &end);
  diff = 1E9 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
  printf("elapsed time = %llu nanoseconds for ucontext swap.\n",
         (long long unsigned int)diff / iterations);

  return 0;

  return 0;
}