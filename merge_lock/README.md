# Lab 1

This code familiarizes with fork/join parallelism, barriers, and locks.  

## Summary:
These are two sorting algorithms in C++. The first algorithm uses fork/join parallelism to parallelize Mergesort. The second algorithm uses locks to implement BucketSort across multiple sorted data structures. 

## Implementation of Fork/Join Merge Sort and Bucket Sort using locks

### Merge Sort Logic

In the given algorithm the array is first divided into several partitions based on the equal partitioning right in the independent sorting function. This function enables the partitioning of the input array for running threads on each partition. Each partition is sorted and then the threads are joined. The joining is a complicated step where we take the low as the low of the first partition, mid as the low of the second partition and high which is the high of second partition. This keeps on going until theres only 1 thread left.


### Bucket Sort Logic

In this algorithm, we again create independent partitions where we consider the range of numbers and number of threads to partition data. Here for n inputs we can use (n-1) threads to avoid segmentation faults. Each thread then inputs the element into a global multiset(not 'set' to maintain the number of copies of an element). The multiset sorts the element on its own and then we merge it into our vector again sorted.

The implementation of Locking BucketSort avoids idling threads if at all possible. For instance, a problem decomposition that assigns the bucket ranges evenly across threads will result in idle threads when the source distribution is highly skewed e.g. (1,2,3,4,5,6,7,1001,1000,1002).  Instead, it works to ensure that all threads are busy at all times, even at the cost of synchronizing across buckets.



### Compilation and Execution:
The repository contains a Makefile and the project is built using a single `make` command.  The generated executable is called `mysort`.  The `mysort` command has the following syntax:

`mysort [--name] [-i source.txt] [-o out.txt] [-t NUMTHREADS] [--alg=<forkjoin,lkbucket>] `

See below for `mysort` syntax examples.  As before, you can assume that all input values are non-negative, less than or equal to `INT_MAX`, and that there are no duplicates.


## Examples
```
Examples of your `mysort` program's syntax
$ make
$ shuf -i0-2147483643 -n382 > case1.txt
$ ./mysort -i case1.txt -o output1.txt --alg=forkjoin -t 4
$ ./mysort -i case1.txt -o output1.txt --alg=lkbucket -t 4

```

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
./mysort -i 321.txt -o out.txt -t5 --alg=forkjoin
# prints time taken in nanoseconds for 5 threads on fork/join sort:
294759

cat out.txt
# prints:
1
2
3
```


### Files:

merge_par1.cpp: Parallel Merge Sort file

main.cpp: Main file containing of both algorithms 

Please ignore other files