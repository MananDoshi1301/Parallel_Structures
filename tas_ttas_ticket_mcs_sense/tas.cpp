#include <bits/stdc++.h>
#include <barrier>
using namespace std;
#define SEQ memory_order_seq_cst
#define RELAX memory_order_relaxed

// Main

vector<thread*> threads;
size_t NUM_THREADS = 2;
int NUM_ITERATIONS = 10;
int counter = 0;

// Tas
atomic<bool> tas_flag = 0;


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

bool tas_impl(atomic<bool> &x, memory_order MEM){
    bool exp = false;
    bool desired = true;
    return x.compare_exchange_strong(exp, desired, MEM);
}

void tas_lock(){
    while(tas_impl(tas_flag, SEQ) == false){}
}

void tas_unlock(){
    tas_flag.store(false, SEQ);
}


void * thread_counter(size_t tid){
    // printf("Inside function: Tid: %zu, Counter: %d", tid, counter);
    for(int i = 0; i < NUM_ITERATIONS; i++){
        // Lock    
        tas_lock();
        
        counter++;
        printf("Tid: %zu, Counter: %d\n", tid, counter);
        // Unlock
        tas_unlock();
    }
    
    return 0;
}

int main() {
    
    cout << "Inside main" << endl;
    global_init();
    
    // launch threads
    int ret; size_t i;
    threads.resize(NUM_THREADS);
    for(i=0; i<NUM_THREADS; i++){
        threads[i] = new thread(thread_counter,i+1);
    }
    
    
    // join threads
    for(size_t i=0; i<NUM_THREADS; i++){
        threads[i]->join();
        printf("joined thread %zu\n",i+1);
        delete threads[i];
    }
    
    global_cleanup();
}