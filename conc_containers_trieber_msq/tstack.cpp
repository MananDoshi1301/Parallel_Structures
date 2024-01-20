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
        Node *down;
        Node(int v){
            val = v;
        }
};

class tstack{
    
    public:
        atomic<Node *> top;        

        void push(int val_, size_t tid){
            // printf("%zu in push\n", tid);
            Node *t, *n = new Node(val_);                        
            do{
                t = top.load(ACQ);
                n->down = t;
            }while(!top.compare_exchange_strong(t, n, ACQ_REL));
        }

        int pop(size_t tid){
            Node *n, *t;
            int v;       
            // printf("%zu in pop\n", tid);
            do{
                t = top.load(ACQ);
                if(t == nullptr) return -1;

                n = t->down;
                v = t->val;
            }while(!top.compare_exchange_strong(t, n, ACQ_REL));
            return v;
        }
};

tstack st;

void* controller(size_t tid, int op_num, int push_val){         
    
    // bar->arrive_and_wait();
    
    if(tid == 0) clock_gettime(CLOCK_MONOTONIC,&startTime);    
    int val;      
    
    switch(op_num){
        case 1:             
            st.push(push_val, tid);            
            printf("Pushed: %d\n", push_val);
            break;
            
        case 2:             
            val = st.pop(tid); 
            printf("%zu Popped: %d\n", tid, val);
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