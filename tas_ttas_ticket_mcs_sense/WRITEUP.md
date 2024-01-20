## Counter Program

| Primitive    | Runtime (s) | L1 cache hit (%) | Branch Pred Hit Rate (%) | Page Fault Count (#)|
|--------------|-------------|------------------|--------------------------|---------------------|
| TAS Lock             | 0.0006  | 89.63 | 93.88 | 37 |
| TTAS Lock           | 0.0006  | 90.3 | 93.82 | 38 |
| Ticket lock              | 0.0008  | 90.47 | 93.89 | 38 |
| MCS lock           | 0.00082  | 90.34 | 94.23 | 38 |
| Pthread Lock              | 0.00085  | 94.2 | 50 | 39 |
| Sense Barrier           | 0.009  | 89.69 | 94.1 | 38 |
| Pthread Barrier              | 0.0009  | 90.46 | 93.95 | 37 |


## Bucket Sort Program

| Primitive    | Runtime (s) | L1 cache hit (%) | Branch Pred Hit Rate (%) | Page Fault Count (#)|
|--------------|-------------|------------------|--------------------------|---------------------|
| TAS Lock              | 1.72  | 96.61 | 98.01 | 8033 |
| TTAS Lock           | 1.67  | 97.85 | 99.07 | 8030 |
| Ticket lock              | 1.93  | 98.71 | 99.49 | 8031 |
| MCS lock            | 1.47  | 96.45 | 97.98 | 8031 |
| Pthread Lock              | 1.973  | 96.17 | 97.91 | 8031 |
| Sense Barrier           | 5.21  | 99.89 | 99.94 | 8036 |
| Pthread Barrier              | 27.161  | 93.46 | 97.56 | 8035 |


## A description of your algorithms and challenges overcome
For this assignment we were suppose to implement:
Test and Set Lock (TAS)
Using Sequential Consistency, compare and swap atomic operation(library operation: compare_exchange_strong), and global atomic variable, we set the flag to true which is done by the first thread acquiring the lock, which when releases by setting the flag as false(both actions under sequential consistency) which gets identified by other threads waiting to acquire lock. Initial challenge was the order of function placement which can enable threads to acquire lock and maintain total global order.

Test and Test and Set Lock
This is an extention to TAS Lock where we just to improve performance making sure that cache line is not always bouncing, we add a sequential load function in the while loop for checking flag as true in which the threads are waiting while also checking if the thread can acquire the lock in an "OR" condition. This algorithm was straight-forward tom implement after implementing TAS Lock.

Ticket Lock
This is a typical ticket system lock where we use fetch and increment method on atomic variables. Threads incoming get themselves a number and then wait in the while loop for their turn governed by niw serving number under sequential consistency. There were thoughts required when assigning sequential or relaxed memory consistency when issuing a number. With Relaxed there were complete outputs of counter but more chance misses like 2nd getting chance before 1st issued ticket.

MCS Lock
One of the tricky implementation of locks where we hand chance to the next node attached to the queue as a node to process critical section. This is done by assigning each thread a node which tries to acquire lock by attaching themself to the global queue. Once the current lock holding thread completes its execution, turns the waiting state of the next node in the linked list to false which executes critical section and stops spinning on its own node.

Sense Reversal Barrier
One of the straight-forward implementations where we create a Barrier method wait based on the lass sense and the individual 'local_threads' my_sense which flips to opposite of sense and turns to sense once all threads arrive to the barrier where the last arriving thread flips sense by sequential conistency. 

## A brief discussion of performance - what algorithm is faster? on what cases? why?
On the basis of practical algorithm implementation on bucketsort, MCS lock has performed very well in terms of runtime since it hands over the lock which avoid contention and does not let the cache line jump. After which TTAS and Tas lock have performed better ith their simple implementation however, having great cache bouncing due to contention introducing misses. Ticket lock is a FIFO lock making it slower with assignment and turn implementation updating cache line. Sense barrier performs better in scenarios on requiring less iterations on functions but here due to numerous iterations threads take time to implement. Lastly the pthread locks and barriers perform poorly compared to everyone due to their property of yielding for other threads as modified TAS version.

## A brief description of your code organization
In counter.cpp each lock has its thread forking and joining function governed by cmd arguments processing in int main. This thread generating fucntions call respective locks with counters recording time.

In buck_sort.cpp sense lock has different thread generation while all other thread generation pass function pointers as per arguments in int main calling respective lock functions.

## A description of every file submitted
MCS.h => MCS Directive
tas.h => tas Directive
test_tas.h => test_tas Directive
ticket_lock.h => ticket_lock Directive
sense.h => sense Directive