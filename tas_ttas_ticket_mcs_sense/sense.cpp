#include <bits/stdc++.h>
#include <barrier>
using namespace std;
#define SEQ memory_order_seq_cst
#define RELAX memory_order_relaxed

// Main

vector<thread*> threads;
size_t NUM_THREADS = 2;
int NUM_ITERATIONS = 1;
int counter = 0;

// Sense


struct timespec startTime, endTime;

barrier<> *bar; 
mutex* lk;
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

class Sense_Barrier{
    public:
        atomic<bool> sense;
        int cnt = 0;
        mutex* bar_lk;        
        int N = NUM_THREADS;
    
        Sense_Barrier(){
            bar_lk = new mutex();
        }
    
        void wait(size_t t_id){
            // cout << "Calling sense: " << t_id << endl;
            thread_local bool my_sense = 0;
            
            if(my_sense == false) my_sense = true;
            else my_sense = false;
            
            bar_lk->lock();            
            // cout << "From Bar: " << cnt << endl;
            cnt++;
            if(cnt == N) {
                cnt = 0;
                bar_lk->unlock();
                sense.store(my_sense, SEQ);
            }
            
            else{
                bar_lk->unlock();
                while(my_sense != sense.load(SEQ)){}
            }
        }
};
Sense_Barrier* sense = new Sense_Barrier;

void * thread_counter(size_t t_id){
    // printf("Inside function: Tid: %zu, Counter: %d\n", t_id, counter);
    int val;
    for(int i = 0; i < NUM_ITERATIONS * NUM_THREADS; i++){
        
        // printf("Tid: %zu, Counter: %d, i: %d\n", t_id, counter, i);
        
        // if(i != NUM_THREADS) 
            // val = i % NUM_THREADS;
            printf("Val: %d\n", i);
        // else val = i;
        // cout  << val << endl;
        if(i == t_id){
            counter++;
            printf("After: Tid: %zu, Counter: %d, i: %d\n", t_id, counter, i);
        }
        
        // bar->arrive_and_wait();
        sense->wait(t_id);
    }
    
    
    return 0;
}

int main(int argc, const char* argv[]) {
    
    // cout << "Inside main" << endl;
    
    global_init();
    try{
        NUM_THREADS = stoi(argv[1]);
        NUM_ITERATIONS = stoi(argv[2]);
    } 
    catch(...){}
    // launch threads
    int ret; size_t i;
    threads.resize(NUM_THREADS);
    for(i=0; i<NUM_THREADS; i++){
        threads[i] = new thread(thread_counter,i);
    }
    
    // join threads
    for(size_t i=0; i<NUM_THREADS; i++){
        threads[i]->join();
        // printf("joined thread %zu\n",i);
        delete threads[i];
    }
    
    printf("Counter in main: %d\n", counter);
    
    global_cleanup();
}