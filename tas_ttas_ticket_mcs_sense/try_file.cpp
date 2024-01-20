#include <bits/stdc++.h>
#include "./inclusives/tas.h"
#include "./inclusives/test_tas.h"
#include "./inclusives/ticket_lock.h"
#include "./inclusives/MCS.h"
#include "./inclusives/sense.h"

using namespace std;
#define SEQ memory_order_seq_cst
#define RELAX memory_order_relaxed

// Main

vector<thread*> threads;
size_t NUM_THREADS = 2;
int NUM_ITERATIONS = 1;
int counter = 0;

// Tas
atomic<bool> tas_flag = 0;

// Test_Tas
atomic<bool> test_tas_flag = 0;

// Ticket Lock
atomic<int> next_number = 0;
atomic<int> now_serving = 0;

// MCS Lock
atomic <Node*> tail = nullptr;

barrier<> *bar; 
mutex *lk;

struct timespec startTime, endTime;

void global_init(){
    bar = new barrier(NUM_THREADS);
    lk = new mutex();    
}
void global_cleanup(){
    delete bar;
    delete lk;    
}

void local_init(){}
void local_cleanup(){}

Sense_Barrier* sense = new Sense_Barrier(NUM_THREADS);

void * thread_barrier(size_t t_id){    
    int val;
    for(int i = 0; i < NUM_ITERATIONS * NUM_THREADS; i++){
                        
        val = i % NUM_THREADS;        
        if(val == t_id){
            counter++;
            printf("After: Tid: %zu, Counter: %d, i: %d\n", t_id, counter, i);
        }
        sense->wait(t_id);        
    }
        
    
    return 0;
}

void * thread_counter(size_t tid){
    // printf("Inside function: Tid: %zu, Counter: %d", tid, counter);
    
    // Only for MCS lock     
    thread_local Node* k;
    // bar->arrive_and_wait();
    // if(tid==1){
    //     clock_gettime(CLOCK_MONOTONIC,&startTime);
    // }
    
    
    for(int i = 0; i < NUM_ITERATIONS; i++){
        // Lock    
        // tas_lock(tas_flag);
        // ticket_lock(next_number, now_serving);
        acquire(k, tail);
        
        counter++;
        // printf("Tid: %zu, Counter: %d\n", tid, counter);
        // cout << tid << "\t" << counter << endl;
        
        // Unlock
        // tas_unlock(tas_flag);
        // ticket_unlock(now_serving);
        release(k, tail);
    }
    
    return 0;
}

int main(int argc, const char* argv[]) {
    
    // cout << "Inside main" << endl;
    global_init();
    
    // launch threads
    int ret; size_t i;
    try{
        NUM_THREADS = stoi(argv[1]);
        NUM_ITERATIONS = stoi(argv[2]);
    } 
    catch(...){}
    threads.resize(NUM_THREADS);
    for(i=0; i<NUM_THREADS; i++){
        if(i == 0) clock_gettime(CLOCK_MONOTONIC,&startTime);
        // threads[i] = new thread(thread_counter,i+1);
        threads[i] = new thread(thread_barrier, i);
    }
    
    unsigned long long elapsed_ns;
	elapsed_ns = (endTime.tv_sec-startTime.tv_sec)*1000000000 + (endTime.tv_nsec-startTime.tv_nsec);
	printf("Elapsed (ns): %llu\n",elapsed_ns);
	double elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);
    
    // join threads
    for(size_t i=0; i<NUM_THREADS; i++){
        threads[i]->join();
        // printf("joined thread %zu\n",i+1);
        delete threads[i];
    }
    
    global_cleanup();
}