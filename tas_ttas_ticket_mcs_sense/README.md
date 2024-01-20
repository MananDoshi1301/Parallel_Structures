# Custom Locks and Barriers

This code should familiarize you with building concurrency primitives
and the C/C++ memory model.

## Summary:
In this repository you there are four locking algorithms and one barrier algorithm. I have used both these primitives in BucketSort algorithm and also in a stand-alone "counter" micro-benchmark. `perf` is used to investigate the performance of code.

The algorithms written here are (under sequential consistency):
* Test-and-set lock
* Test-and-test-and-set lock
* Ticket lock
* MCS Lock
* Sense reversal barrier

## Code Requirements / Restrictions
The primitives are written using C/C++ atomics and do not rely on external concurrency primitives (e.g. `pthread_mutex_t`). Code performance is optimized so as to avoid contention over algorithms.

### Microbenchmark:
It is a small stand-alone program, called `counter`. The program increments a counter, using your new primitives.  Either the counter is protected by a lock, or the threads using a barrier to rotate turns incrementing the counter. At program end it will print the time the test took. The counter program will have options to use all locks/barriers I wrote, plus the pthread ones taking note of the performance differences.


## Lab write-up:
The write-up, , called `WRITEUP.md`, describes how changing the lock and barrier types in the two programs changes performance. In particular, the write-up includes:
* A table of all different locks (for both programs), which includes run time, L1 cache hit rate, branch-prediction hit rate, and page-fault count with four threads.
* The same table for all barriers.
* A discussion explaining why the best and worst primitives have these results.

The write-up also includes the normal requirements:
* A description of your algorithms and challenges overcome
* A brief discussion of performance - what algorithm is faster? on what cases? why? 
* A brief description of your code organization
* A description of every file submitted
* Any extant bugs


## Compilation and Execution:
Your submission should contain a Makefile and the project should build using a single `make` command.  Your makefile
will generate two executables.

### Counter

`counter [--name] [-t NUM_THREADS] [-n NUM_ITERATIONS] [--bar=<sense,pthread,senserel>] [--lock=<tas,ttas,ticket,mcs,pthread,peterson,tasrel,ttasrel,mcsrel,petersonrel>] [-o out.txt]`

The program launches `NUM_THREADS` and each increments the counter `NUM_ITERATIONS` times.  The counter is synchronized using either the `bar` or `lock` argument.  The time taken to increment the counter to its total (excluding the time for file I/O and to initially launch / finally join threads) is printed to standard out in nanoseconds, and the final counter value is written to the output file designated by the `-o` flag. See below for `counter` syntax examples.

### BucketSort

`mysort [--name] [-i source.txt] [-o out.txt] [-t NUM_THREADS] [--bar=<sense,pthread>] [--lock=<tas,ttas,ticket,mcs,pthread,petersonseq,petersonrel>]`

The new `bar` and `lock` arguments chooses which barrier and lock to use respectively when executing the algorithm. If this argument is excluded, it uses the default pthread primitives.  Using the `--name` option should print my name.  Otherwise, the program should sort the source file.  The `mysort` command sorts all integers in the source file and print them sorted one integer per line to an output file (specified by the `-o` option). The time taken to sort the file (excluding the time for file I/O and to initially launch / finally join threads) is printed to standard out in nanoseconds.  The `-t` option specifies the number of threads that should be used, including the master thread, to accomplish the sorting.   The `getopt` and `getopt_long` method calls are helpful for parsing the command line.

## Examples

Examples of your `mysort` program's syntax
```
### print your name
./mysort --name
# prints:
Your Full Name

### Consider an unsorted file
printf "3\n2\n1\n" > 321.txt
cat 321.txt
# prints
3
2
1

### Sort the text file and print to file
./mysort -i 321.txt -o out.txt -t5 --lock=tas
# prints time taken in nanoseconds for 5 threads:
294759

cat out.txt
# prints:
1
2
3
```

Examples of your `counter` program's syntax
```
### print your name
./counter --name
# prints:
Your Full Name

### Increment the counter
./counter -o out.txt -t5 -n10000 --lock=tas 
# prints time taken in seconds for 5 threads on tas lock,
# to increment 10000 times each and outputs final counter value to out.txt
294759

cat out.txt
# prints:
50000
```
