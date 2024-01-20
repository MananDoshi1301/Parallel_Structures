#include <bits/stdc++.h>

using namespace std;
#define SEQ memory_order_seq_cst
#define RELAX memory_order_relaxed

// Main

vector<thread*> threads;
size_t NUM_THREADS = 2;
int NUM_ITERATIONS = 10;
int counter = 0;

// MCS Vars

// 
barrier <> *bar;

void global_init(){
    bar = new barrier(NUM_THREADS);
}

void global_cleanup(){
	delete bar;
}

struct timespec startTime, endTime;

// void global_init(){}
// void global_cleanup(){}
void local_init(){}
void local_cleanup(){}


// MCS Class
class Node{
    public:
        atomic <bool> wait;
        atomic <Node*> next;
};

atomic <Node*> tail = nullptr;

void pt(size_t tid, string txt = ""){
    cout << tid << " " << txt << endl;
}

void acquire(Node* &myNode, size_t tid){
    
    Node* oldTail = tail.load(SEQ);
    
    // (myNode->next).store(NULL, SEQ);    
    
    // pt(tid, "acq");
    while(!tail.compare_exchange_strong(oldTail, myNode, SEQ)){
        oldTail = tail.load(SEQ);
    }
    
    if(oldTail != nullptr){
        (myNode->wait).store(true, RELAX);
        (oldTail->next).store(myNode, SEQ);
        while((myNode->wait).load(SEQ)){}
    }
}

void release(Node* &myNode, size_t tid){
    // Check if theres no one after us using cas
    // pt(tid, "rel");
    Node* m = myNode;
    if(tail.compare_exchange_strong(m, nullptr, SEQ)){}
    else{
        while((myNode->next).load(SEQ) == NULL){}
        Node * p = myNode->next;
        (p->wait).store(false, SEQ);
    }
}

void * thread_counter(size_t tid){
    // printf("Inside function: Tid: %zu, Counter: %d\n", tid, counter);
    Node* k;
    // bar->arrive_and_wait();
    for(int i = 0; i < NUM_ITERATIONS; i++){
        
        // Lock 
        acquire(k, tid);
        
        counter++;
        printf("Tid: %zu, Counter: %d\n", tid, counter);
        
        // Unlock
        release(k, tid);
        
    }
    
    // bar->arrive_and_wait();
    return 0;
}

int main(int arcg, const char* argv[]) {
    
    // cout << "Inside main" << endl;
    global_init();
    
    // launch threads
    NUM_THREADS = stoi(argv[1]);
    int ret; size_t i;
    threads.resize(NUM_THREADS);    
    for(i=0; i<NUM_THREADS; i++){
        // cout << "Spawning: " << i << endl;
        threads[i] = new thread(thread_counter,i+1);
    }    
    
    // join threads
    for(size_t i=0; i<NUM_THREADS; i++){
        threads[i]->join();
        // printf("joined thread %zu\n",i+1);
        delete threads[i];
    }
    
    global_cleanup();
}