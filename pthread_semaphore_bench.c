#include <errno.h>
#include <libgen.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/ipc.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <ucontext.h>
#include <unistd.h>

int sem_id;

#define iterations 1000000

void *first_ctx_func(void *arg) {
  struct timespec start, end;
  unsigned long diff;
  int i = 0;
  struct sembuf sem;
  sem.sem_flg = 0;

  clock_gettime(CLOCK_MONOTONIC, &start);

  for (; i < iterations; ++i) {
    sem.sem_num = 1;
    sem.sem_op = -1;
    semop(sem_id, &sem, 1);
    sem.sem_num = 0;
    sem.sem_op = 1;
    semop(sem_id, &sem, 1);
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  diff = 1E9 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
  printf("elapsed time = %llu nanoseconds for pthread-semaphore.\n",
         (long long unsigned int)diff / iterations);

  exit(0);
  return NULL;
}

void second_ctx_func() {
  int i = 0;
  struct sembuf sem;
  sem.sem_flg = 0;

  for (; i < iterations; ++i) {
    sem.sem_num = 0;
    sem.sem_op = -1;
    semop(sem_id, &sem, 1);
    sem.sem_num = 1;
    sem.sem_op = 1;
    semop(sem_id, &sem, 1);
  }
}

int main(int argc, char const *argv[]) {
  pthread_t first_ctx_thr;
  int val;
  key_t key;

  key = ftok("/etc/passwd", 'A');
  sem_id = semget(key, 2, 0666 | IPC_CREAT);
  if (sem_id < 0) {
    perror("semget failed");
    exit(1);
  }

  val = 1;
  semctl(sem_id, 1, SETVAL, val);

  pthread_create(&first_ctx_thr, NULL, first_ctx_func, NULL);

  while (1) {
    second_ctx_func();
  }

  return 0;
}