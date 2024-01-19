# OpenMP Parallelization

This code should familiarize you with using OpenMP to parallelize code.

## Summary:
In this code I have parallelized two sorting algorithm using OpenMP, ie. MergeSort and QuickSort.  

## Code Requirements / Restrictions

My code leverages the OpenMP library to parallelize code.  My code does not use pthreads or C/C++ atomics to manage or synchronize threads.  There is no prewritten sorted data structure.  

### Lab write-up:
Your write-up, called `WRITEUP.md` includes:
* A comparison between the ease of parallelization between pthreads and OpenMP.
* A discussion of performance between OpenMP and pthread implementations
* A description of your parallelization strategy
* A brief description of your code organization
* A description of every file submitted
* Any extant bugs

### Compilation and Execution:
This folder contains a Makefile and the project can be built using a single `make` command.  The generated executable is called `mysort`.  The `mysort` command has the following syntax:

`mysort [--name] [source.txt] [-o out.txt] [-t NUMTHREADS]  [--alg=<merge,quick>]`

Using the `--name` option prints your name.  Otherwise, the program sorts the source file.  The source file is a text file with a single integer on each line.  The `mysort` command then sorts all integers in the source file and print them sorted one integer per line to an output file (specified by the `-o` option). The time taken to sort the file (excluding the time for file I/O and to initially launch / finally join threads) is printed to standard out in nanoseconds.  The `-t` option specifies the number of threads that should be used, including the master thread, to accomplish the sorting.  

See below for `mysort` syntax examples.

## Examples

Examples of `mysort` program's syntax
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
./mysort -t 4 -i case1.txt  -o out.txt --alg=quick
# prints time taken in nanoseconds for 4 threads on quicksort:
294759

cat out.txt
# prints:
1
2
3
```


Testing this code:
```
### Generate a test file
### (of unspecified range and size)
shuf -i0-2147483643 -n382 > case1.txt

### Sort it using sort
sort -n case1.txt > soln1.txt

### Run your mysort program to also sort the test file
./mysort -t 4 -i case1.txt  -o out.txt --alg=quick

### Compare test results
cmp --silent your1.txt soln1.txt && echo "Pass (5pts)" || echo "Fail (0pts)"
```