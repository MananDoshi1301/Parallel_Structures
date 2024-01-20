# Experimental results 
(All results are in seconds)
## sgl stack
Threads
(80): 0.0018
(8000): 0.348
(20000): 0.8931

## sgl queue
Threads
(80): 0.003215
(8000): 0.3935
(20000): 1.0046

## trieber stack
Threads
(80): 0.00019
(8000): 0.00013
(20000): 0.000223

## m&s queue
Threads
(80): 0.000217
(8000): 0.000081
(20000): 0.000158

## sgl stack with elimination (sgl_elim.cpp)
Threads
(80): 1.0005
(8000): 1.118476 
(20000): 2.888

## trieber stack with elimination (num_elim.cpp)
Threads
(80): 0.000148
(8000): 0.000079
(20000): 0.000158


# Analysis of results using perf as necessary to support explanations

Based on the assignment, we were supposed to design locking and lock-free algorithms along with solutions to contention problems. Thus, based on the above results it is very clear that single global locks are very expensive in terms yielding, taking more time than normal algorithms. However, lock-free algorithms Trieber Stack and M&S Queue were extremely fast in rendering results. Elimination array is a solution to stack especially trieber stack to widen up the top. Thus, it performs extremely well, however, with single global lock elimination also cannot help too much. However, the time taken is consistent over several threads and data. The following perf results too show the same:

1. sgl_stack 
Cache_Misses: 1731
Cache References: 15989

2. sgl_queue 
Cache_Misses: 852
Cache References: 16184

3. tstack 
Cache_Misses: 874
Cache References: 15606

4. ms_queue 
Cache_Misses: 1676
Cache References: 14292

5. sgl_elim 
Cache_Misses: 1298
Cache References: 14950

6. num_elim 
Cache_Misses: 1657
Cache References: 15193

At someplace it is not that great of a significant difference due to the limit in number of threads to produce but still at low-level its parts the result.

# A description of code organization
For each 6 files, threads are generated in the main file based on the num of threads provided by the user in cmd line. The thread is fed with controller function, which operation to perform on data structure, value if required generated randomly using seed. Controller uses switch case to make the threads jumps tp their respective function:

1. sgl_stack
push, pop, top, empty all mentioned globally

2. sgl_queue
enqueue, dequeue, front, back, isEmpty

3. tstack
Class tstack defined using class Node containing push and pop

4. ms_queue
Class ms_queue defined using class Node containing push and pop

5. sgl_elim
Push and pop function with Elimination array function

6. num_elim
Class tstack defined using class Node containing push and pop. elimArr function implementing Elimination Array features

# A description of every file submitted
1. sgl_stack - SGL Stack
2. sgl_queue -SGL Queue
3. tstack - Trieber Stack
4. ms_queue - M&S queue
5. sgl_elim - SGL Stack with Elimination Array
6. num_elim - Trieber Stack with Elimination Array

# Compilation instructions
There are 6 different lab files: (Each file can be run by this command: ./[filename] [num_threads])
For eg: 
1. make
2. ./sgl_stack 20