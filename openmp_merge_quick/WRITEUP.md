## A comparison between the ease of parallelization between pthreads and OpenMP.
Both threading approaches are very important based on the use case. With pthreads, there is explicit thread  management which enables fine grained control and more resource management over the program. However, it makes it more complex and error prone due to manual synchronization. On the other hand, Openmp with its compiler directives simplifies parallelization, abstracting complexities and automating thread creation and synchronization. It excels in ease of use, requiring minimal modifications to introduce parallelism, especially for loops or sections of code. In terms of ease of parallelization, Pthreads demand meticulous attention to synchronization, making it more challenging, yet offering precise control. In contrast, OpenMP shines for simpler to moderately complex tasks, abstracting thread management and simplifying the introduction of parallelism through directives, making it an efficient choice for many parallelization needs.

## A discussion of performance between OpenMP and pthread implementations
There is a wide time difference between two executions (five different runtimes). The algorithm implementation is on the same line with just one using OpenMP thread generation and other using the forkjoin pthread implementation.

#### Open MP Time:
##### Elapsed (ns): 71339539
##### Elapsed (ns): 67349219
##### Elapsed (ns): 70451598
##### Elapsed (ns): 66728338
##### Elapsed (ns): 70332171

#### Pthread Fork Join Time:
##### Elapsed (ns): 4462526311
##### Elapsed (ns): 4359345764
##### Elapsed (ns): 4252601591
##### Elapsed (ns): 4020585246
##### Elapsed (ns): 4101844927

There can be several reasons for this to happen since Pthread implementation is more open ended and gives freedom to work around threads and resources, while OpenMP uses default constructs which makes sense in the terms of High Performance Computing. OpenMP also utilizes process isolation to make it more efficient which may be in the programmers control in terms of Pthread programming. But there is wide time difference which can be seen from the above five runtime implementations of Merge Sort. 

## A description of your parallelization strategy

### Merge Sort:
As like the pthread implementation I have used the method of initial vector separation based on tids and let all the threads sort their independent partions. While noting each partitions high and low linked to concurrently in a map, I merge sequentially with the help of map.

### Quick Sort:
This is a straight forward algorithm where I recursively call the quicksort function by getting the pivot. This way I sort the partitions bottom up starting with small partition initially. For this implementation I use openmp sections to independently use two threads on two sections which internally again call next two threads working simultaneously. 

## A brief description of your code organization

### Merge Sort:
The merge sort algorithm beguns when the function merge_ctrl function is called. It primarily identifies partitions which threads use to merge internally by calling the merge_proc function which checks if further division of partition is possible or not and then eventually sorts using the merge function.

### Quick Sort:
On the other hand, the main function calls quick_ctrl function which calls quick function responsible for calling itself recursively where it finds partition. Inside this function 2 threads process recursively. Once it is processed it, finds pivot on each partition enabling the sort bottom up in D&C manner.

## A description of every file submitted
mysort.cpp: It contains the code for quick sort and merge sort respectively.