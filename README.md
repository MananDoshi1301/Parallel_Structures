# Parallel Data Structures

This repository contains code, compilation intructions and in some cases observations to parallel data structures and algorithms developed by me.

### Following are the algorithms to refer:
1. Merge Sort and Bucket Sort using locks, barriers in pthreads (merge_lock)
2. Merge Sort and Quick Sort (Two variations) using OpenMP
3. Custom Concurreny Primitives (Without using Pthread mutexes)
    * Test-and-set lock
    * Test-and-test-and-set lock
    * Ticket lock
    * MCS Lock 
    * Sense reversal barrier

4. Concurrent Containers (Optimized non-blocking stacks and queues)
    * sgl_stack - SGL Stack (single global stack)
    * sgl_queue -SGL Queue (single global queue)
    * tstack - Trieber Stack
    * ms_queue - M&S queue
    * sgl_elim - SGL Stack with Elimination Array
    * num_elim - Trieber Stack with Elimination Array