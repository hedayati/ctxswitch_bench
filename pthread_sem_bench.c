#define _GNU_SOURCE
#include <errno.h>
#include <libgen.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <ucontext.h>
#include <unistd.h>

sem_t sem0;
sem_t sem1;

#define iterations 1000000

void *first_ctx_func(void *arg) {
  struct timespec start, end;
  unsigned long diff;
  int i = 0;
  cpu_set_t cpuset;
  pthread_t thread;

  thread = pthread_self();
  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);
  pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);

  clock_gettime(CLOCK_MONOTONIC, &start);

  for (; i < iterations; ++i) {
    sem_wait(&sem1);
    sem_post(&sem0);
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  diff = 1E9 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
  printf("elapsed time = %llu nanoseconds for pthread-semaphore.\n",
         (long long unsigned int)diff / iterations);

  exit(0);
  return NULL;
}

void second_ctx_func() {
  cpu_set_t cpuset;
  pthread_t thread;

  thread = pthread_self();
  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);
  pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);

  for (;;) {
    sem_wait(&sem0);
    sem_post(&sem1);
  }
}

int main(int argc, char const *argv[]) {
  pthread_t first_ctx_thr;

  sem_init(&sem0, 0, 0);
  sem_init(&sem1, 0, 1);

  pthread_create(&first_ctx_thr, NULL, first_ctx_func, NULL);

  while (1) {
    second_ctx_func();
  }

  return 0;
}