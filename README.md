# Context-Switch Benchmarking
`make` to compile the benchmarks.

`make run` to run the benchmarks.

## Results
on Dual-Socket Xeon E5-2620 v3 @ 2.40GHz (2x6x2), Linux 4.11, 8GB RAM:

```
elapsed time = 282 nanoseconds for ucontext swap.
elapsed time = 2747 nanoseconds for fork-socket.
elapsed time = 1747 nanoseconds for fork-sysvsemaphore.
elapsed time = 2008 nanoseconds for fork-semaphore.
elapsed time = 1954 nanoseconds for pthread-mutex.
elapsed time = 1518 nanoseconds for pthread-sysvsemaphore.
elapsed time = 1595 nanoseconds for pthread-semaphore.
```
