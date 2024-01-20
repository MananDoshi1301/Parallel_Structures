#include <bits/stdc++.h>

using namespace std;
#define REL memory_order_release
#define ACQ memory_order_acquire
#define ACQ_REL memory_order_acq_rel
// compare_exchange_strong

vector<thread*> threads;
size_t NUM_THREADS;
 
mutex* lk;
// barrier<> *bar; 

struct timespec startTime, endTime;
const size_t elimArr_size = 5;
// atomic <pair<size_t, int>> *arr[elimArr_size];
// atomic <int> arr[elimArr_size];

class ps {
    public:
        atomic<size_t> tid;
        atomic<int> op_num;
        atomic<int> val;
};

atomic<ps*> arr[elimArr_size];


void global_init(){	
	lk = new mutex();
    // bar = new barrier(NUM_THREADS);
    // arr = new int();
    // arr = new atomic<vector<pair<size_t, int>>>(vector<pair<size_t, int>>()); 
    // for(int i=0; i<elimArr_size; i++){
    //     arr.store(0, ACK_REL);
    // }    
    // int temp = -1;
    // size_t t = -1;
    // for(size_t i=0; i<elimArr_size; i++){        
    //     arr[i].load(ACQ)->op_num.store(temp, ACQ_REL);
    //     arr[i].load(ACQ)->tid.store(t, ACQ_REL);
    //     arr[i].load(ACQ)->val.store(temp, ACQ_REL);
    // }
}
void global_cleanup(){	
	delete lk;
	// delete bar;
    // delete arr;
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

int elimArr(size_t tid, int waitTime, int arr_pos, int op_num, int val){     
    
    // -1, -1: No one present
    
    size_t temp = -1;
    int v = 1;
    // Want to push      
    if(op_num == 1){        
        // No one present                 
        if(arr[arr_pos].load(ACQ)->tid.compare_exchange_strong(temp, tid, ACQ_REL)){
            arr[arr_pos].load(ACQ)->op_num.store(1, ACQ_REL);
            arr[arr_pos].load(ACQ)->val.store(val, ACQ_REL);
            sleep(waitTime); 
            // Check if someone popped value  
            
            // If not restore value to -1 and get back to cas;
            if(arr[arr_pos].load(ACQ)->tid.load(ACQ) != -1) {
                arr[arr_pos].load(ACQ)->tid.store(temp, ACQ_REL);
                arr[arr_pos].load(ACQ)->val.store(-1, ACQ_REL);
                arr[arr_pos].load(ACQ)->op_num.store(-1, ACQ_REL);
                return -1;
            }
            else return -2;
            // break;
        }                       
        
        // Pop present: while pushing
        else if(arr[arr_pos].load(ACQ)->op_num.load(ACQ) == 2){
            int check_int = -1;
            if(arr[arr_pos].load(ACQ)->val.compare_exchange_strong(check_int, val, ACQ_REL)) return -2;
        }
        
        return -1;
    }
    
    // pop has arrived
    else{
        
        if(arr[arr_pos].load(ACQ)->tid.load(ACQ) == -1){            
            if(arr[arr_pos].load(ACQ)->tid.compare_exchange_strong(temp, tid, ACQ_REL)){
                arr[arr_pos].load(ACQ)->op_num.store(op_num, ACQ_REL);                
                sleep(waitTime); 
                
                // Check if someone pushed value while we were waiting;
                if(arr[arr_pos].load(ACQ)->val.load(ACQ) != -1){
                    int my_v;
                    my_v = arr[arr_pos].load(ACQ)->val.load(ACQ);
                    if(arr[arr_pos].load(ACQ)->val.compare_exchange_strong(my_v, -1, ACQ_REL)){
                        return my_v;
                    }
                    
                    else return -1;
                }                
            }               
        }       
        
        // push present            
        else if(arr[arr_pos].load(ACQ)->op_num.load(ACQ) == 1){            
            // store op to -1
            if(arr[arr_pos].load(ACQ)->op_num.compare_exchange_strong(v, -1, ACQ_REL)){
                v = arr[arr_pos].load(ACQ)->val.load(ACQ);
                arr[arr_pos].load(ACQ)->val.store(-1, ACQ);
                arr[arr_pos].load(ACQ)->tid.store(-1, ACQ);
                
                return v;
            }
            
            else return -1;
        }
        
        return -1;
        
    }
    
    // printf("%zu Sleep Starts\n", tid); 
    // sleep(waitTime); 
    // printf("%zu Sleep ends\n", tid); 
    return -1;
}


class tstack{
    
    public:
        atomic<Node *> top;        

        void push(int val_, size_t tid){
            bool top_cas;
            int arr_pos;
            // printf("%zu in push\n", tid);
            Node *t, *n = new Node(val_);                        
            do{
                t = top.load(ACQ);
                n->down = t;
                top_cas = top.compare_exchange_strong(t, n, ACQ_REL);                                
                if(top_cas == false){                    
                    srand (time(NULL));
                    arr_pos = rand() % elimArr_size;
                    // -2: I am done return;
                    // -1: Retry cas
                    int ret;
                    ret = elimArr(tid, 5, arr_pos, 1, val_);
                    if(ret == -2) break;
                }
            }while(!top_cas);
        }

        int pop(size_t tid){
            Node *n, *t;
            int v, arr_pos;       
            bool top_cas;
            // printf("%zu in pop\n", tid);
            do{
                t = top.load(ACQ);
                if(t == nullptr) return -1;

                n = t->down;
                v = t->val;
                top_cas = top.compare_exchange_strong(t, n, ACQ_REL);                
                // if(top_cas == false){
                //     // printf("%zu False\n", tid);
                //     elimArr(tid, 5);                    
                // }
                if(top_cas == false){                    
                    srand (time(NULL));
                    arr_pos = rand() % elimArr_size;
                    // -2: I am done return;
                    // -1: Retry cas
                    int ret;
                    ret = elimArr(tid, 5, arr_pos, 2, -1);
                    if(ret != -1) return ret;                    
                }
            }while(!top_cas);
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
            push_num = (rand() % 100) + 1;
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