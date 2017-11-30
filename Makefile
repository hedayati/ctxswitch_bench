all: uctx_bench.c fork_sock_bench.c pthread_mutex_bench.c pthread_semaphore_bench.c fork_semaphore_bench.c
	gcc -o uctx_bench uctx_bench.c
	gcc -o fork_sock_bench fork_sock_bench.c
	gcc -o fork_semaphore_bench fork_semaphore_bench.c
	gcc -o pthread_mutex_bench pthread_mutex_bench.c -lpthread
	gcc -o pthread_semaphore_bench pthread_semaphore_bench.c -lpthread

run:
	./uctx_bench
	./fork_sock_bench
	./fork_semaphore_bench
	./pthread_mutex_bench
	./pthread_semaphore_bench

clean:
	rm -rf uctx_bench fork_sock_bench pthread_mutex_bench pthread_semaphore_bench fork_semaphore_bench
	rm -rf *.o
