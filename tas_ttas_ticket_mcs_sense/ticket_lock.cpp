#include <bits/stdc++.h>

using namespace std;
#define SEQ memory_order_seq_cst
#define RELAX memory_order_relaxed

// Main

vector<thread*> threads;
size_t NUM_THREADS = 4;
int NUM_ITERATIONS = 1;
int counter = 0;

// Tickets
atomic<int> next_number = 0;
atomic<int> now_serving = 0;


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


void ticket_lock(){
    int num = next_number.fetch_add(1, SEQ);
    while(num != now_serving.load(SEQ)){}
}

void ticket_unlock(){
    now_serving.fetch_add(1, SEQ);
}

void * thread_counter(size_t tid){
    // printf("Inside function: Tid: %zu, Counter: %d", tid, counter);
    for(int i = 0; i < NUM_ITERATIONS; i++){
        // Lock            
        ticket_lock();
                    
        counter++;
        printf("Tid: %zu, Counter: %d\n", tid, counter);
        
        // Unlock
        ticket_unlock();
    }
    
    return 0;
}

int main(int arcg, const char* argv[]) {
        
    global_init();
    
    // launch threads
    try{
        NUM_THREADS = stoi(argv[1]);
    }    
    catch(...){}
    int ret; size_t i;
    threads.resize(NUM_THREADS);
    for(i=0; i<NUM_THREADS; i++){
        // cout << "Spawn " << i << endl;
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