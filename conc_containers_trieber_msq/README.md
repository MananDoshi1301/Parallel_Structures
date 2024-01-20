# Concurrent Containers (Trieber Stack, M&S Queue with elimination and flat-combining)

## Summary

### Concurrent Containers
Implementation of several concurrent stack and queue algorithms, including a Single Global stack and queue, the Treiber stack, and the M&S queue. Write-up  includes experimental explorations of data structure throughput across varying thread counts.

## Write-up:
The write-up, called `WRITEUP.md` includes:
* Experimental results as required by the prompt
* Analysis of results using `perf` as necessary to support explanations
* A description of your code organization
* A description of every file submitted
* Compilation instructions
* Execution instructions, particulary for any results presented in the write-up
* Any extant bugs

## Compilation and Execution:
The code contains a Makefile and the project is built using a single `make` command.  Executables generated provide execution instructions when given a `-h` flag.

#### There are 6 different lab files: (Each file can be run by this command: ./[filename] [num_threads])
```
For eg: 
1. make
2. ./sgl_stack 20
```