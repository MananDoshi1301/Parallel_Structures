#include <bits/stdc++.h>

using namespace std;
#define REL memory_order_release
#define ACQ memory_order_acquire
#define ACQ_REL memory_order_acq_rel
// compare_exchange_strong

vector<thread*> threads;
size_t NUM_THREADS;
 
// mutex* lk;
// barrier<> *bar; 

struct timespec startTime, endTime;

void global_init(){	
	// lk = new mutex();
    // bar = new barrier(NUM_THREADS);
}
void global_cleanup(){	
	// delete lk;
	// delete bar;
}

void local_init(){}
void local_cleanup(){}

class Node{
    public: 
        int val;
        atomic <Node *> next;        
        Node(){
            next.store(NULL, REL);
        }
        Node(int v){
            val = v;
        }
};

class ms_queue{
    public:
        atomic <Node *> head, tail;

        ms_queue(){
            Node *dummy = new Node(-1);
            head.store(dummy, REL);
            tail.store(dummy, REL);
        }

        void enqueue(int val){
            Node *t, *e, *n = new Node(val);            
            Node *null_expected = new Node();
            while(true){
                t = tail.load(ACQ_REL);
                e = (t->next).load(ACQ_REL);
                if(t == tail.load(ACQ_REL)){
                    if(e == nullptr && (t->next).compare_exchange_strong(null_expected, n, ACQ_REL)){ 
                        break;
                    }
                    
                    else if(e != nullptr) tail.compare_exchange_strong(t, e, ACQ_REL);
                }
            }
            tail.compare_exchange_strong(t, n, ACQ_REL);
        }
    
        int dequeue(){
            Node *t, *n, *h;
            int ret;
            while(true){
                h = head.load(ACQ);
                t = tail.load(ACQ);
                n = (h->next).load(ACQ);
                if(h == head.load(ACQ)){
                    if(h == t){
                        if(n == nullptr) return -2;
                        else tail.compare_exchange_strong(t, n, ACQ_REL);
                    }
                    else{
                        ret = n->val;
                        if(head.compare_exchange_strong(h, n, ACQ_REL)) return ret;
                    }
                }
            }
        }
};

ms_queue q;

void* controller(size_t tid, int op_num, int push_val){         
    
    // bar->arrive_and_wait();
    
    if(tid == 0) clock_gettime(CLOCK_MONOTONIC,&startTime);    
    int val;      
    
    switch(op_num){
        case 1:             
            q.enqueue(push_val);         
            printf("Enqueued: %d\n", push_val);
            break;
            
        case 2:             
            val = q.dequeue(); 
            printf("%zu Dequeued: %d\n", tid, val);
            break;                                       
    }
        
    // bar->arrive_and_wait();  
    
    if(tid == 0) clock_gettime(CLOCK_MONOTONIC,&endTime);
    return 0;
}

int main(int argc, const char* argv[]){
    
    int ret; size_t i;
    NUM_THREADS = stoi(argv[1]);
    // printf("Reached\n");
    // NUM_THREADS = 4;
    global_init();
	threads.resize(NUM_THREADS);
    
    
    // Generate Random Number    
    // 1 = push, 2 = pop
    int  t;    
    int push_num = -1;
    
    // Seed to get different value in rands     
    srand (time(NULL));
	for(i=0; i<NUM_THREADS; i++){
        
        // Identify which operation is being performed         
        t = (rand() % 2) + 1;
        
        // If pushing, get rand value otherwise -1         
        if(t == 1) {
            push_num = rand() % 100;
            // printf("%zu will push %d\n", i, push_num);
        }
        else {
            push_num = -1;
            // printf("%zu will pop\n", i);
        }
        
        // printf("%zu\n", i);
		threads[i] = new thread(controller,i, t, push_num);
	}

	// join threads
	for(size_t i=0; i<NUM_THREADS; i++){
		threads[i]->join();
		// printf("joined thread %zu\n",i);
		delete threads[i];
	}

	global_cleanup();

    cout << endl <<endl;
	unsigned long long elapsed_ns;
	elapsed_ns = (endTime.tv_sec-startTime.tv_sec)*1000000000 + (endTime.tv_nsec-startTime.tv_nsec);
	printf("Elapsed (ns): %llu\n",elapsed_ns);
	double elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);
    
    return 0;
}