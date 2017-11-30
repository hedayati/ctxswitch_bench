#include <errno.h>
#include <libgen.h>
#include <pthread.h>
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

pthread_cond_t cond;
pthread_mutex_t mutex;

#define iterations 1000000

void *first_ctx_func(void *arg) {
  struct timespec start, end;
  unsigned long diff;
  int i = 0;

  clock_gettime(CLOCK_MONOTONIC, &start);

  for (; i < iterations; ++i) {
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  diff = 1E9 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
  printf("elapsed time = %llu nanoseconds for pthread-mutex.\n",
         (long long unsigned int)diff / iterations);

  exit(0);
  return NULL;
}

void second_ctx_func() {
  for (;;) {
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
  }
}

int main(int argc, char const *argv[]) {
  pthread_t first_ctx_thr;

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);

  pthread_create(&first_ctx_thr, NULL, first_ctx_func, NULL);

  while (1) {
    second_ctx_func();
  }

  return 0;
}