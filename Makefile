all: uctx_bench.c fork_sock_bench.c
	gcc -o uctx_bench uctx_bench.c
	gcc -o fork_sock_bench fork_sock_bench.c

clean:
	rm -rf uctx_bench fork_sock_bench
	rm -rf *.o
