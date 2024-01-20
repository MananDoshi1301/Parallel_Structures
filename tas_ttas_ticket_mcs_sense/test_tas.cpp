#include <bits/stdc++.h>

using namespace std;
#define SEQ memory_order_seq_cst
#define RELAX memory_order_relaxed

// Main

vector<thread*> threads;
size_t NUM_THREADS = 4;
int NUM_ITERATIONS = 10;
int counter = 0;

// Test_Tas
atomic<bool> test_tas_flag = 0;


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

bool test_tas_impl(atomic<bool> &x, memory_order MEM){
    bool exp = false;
    bool desired = true;
    return x.compare_exchange_strong(exp, desired, MEM);
}

void test_tas_lock(){
    while(test_tas_flag.load(SEQ) == true || test_tas_impl(test_tas_flag, SEQ) == false){}
}

void test_tas_unlock(){
    test_tas_flag.store(false, SEQ);
}


void * thread_counter(size_t tid){
    // printf("Inside function: Tid: %zu, Counter: %d", tid, counter);
    for(int i = 0; i < NUM_ITERATIONS; i++){
        // Lock    
        test_tas_lock();
        
        counter++;
        printf("Tid: %zu, Counter: %d\n", tid, counter);
        // Unlock
        test_tas_unlock();
    }
    
    return 0;
}

int main(int arcg, const char* argv[]) {
    
    cout << "Inside main" << endl;
    global_init();
    
    // launch threads
    NUM_THREADS = stoi(argv[1]);
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