#include <bits/stdc++.h>
using namespace std;
#define REL memory_order_release
#define ACQ memory_order_acquire
#define ACQ_REL memory_order_acq_rel
// Query to run this: ./sgl_stack 20

vector<thread*> threads;
size_t NUM_THREADS;

stack <int> st;
 
mutex* lk;
barrier<> *bar; 

struct timespec startTime, endTime;
int elimArr_size = 5;
atomic <int> arr[5];

void global_init(){	
	lk = new mutex();
    bar = new barrier(NUM_THREADS);
    for(int i=0; i<5; i++){
        arr[i].store(0, ACQ_REL);
    }
}
void global_cleanup(){	
	delete lk;
    delete bar;
}

void local_init(){}
void local_cleanup(){}

int elimArr(size_t tid, int waitTime, int arr_pos, int op_num, int val){ 
    // ofstream op_file; 
    // op_file.open("arr_pos.txt"); 
    // op_file << "ArrArrArrArrArrArrArrArrArrArrArrArrArrArr: " << arr[arr_pos]; 
    // op_file.close(); 
    int temp = -1, zero = 0;
    // 0: means no operation is present  
    // -1: pop already present
    
    // Want to push      
    if(op_num == 1){
        
        // Only push present
        if(arr[arr_pos].load(ACQ) == 0){
            int t = 0;
            arr[arr_pos].compare_exchange_strong(t, val, ACQ_REL);
            sleep(waitTime);
            // wait 
            if(arr[arr_pos].compare_exchange_strong(temp, val, ACQ_REL)) return -3;            
            // break;
            else return -1;
        }
        
        // pop is waiting there
        else if(arr[arr_pos].load(ACQ) == -1){
            if(arr[arr_pos].compare_exchange_strong(temp, val, ACQ_REL))
                return -3;
        }
        
        // push present at location        
        return -1;          
    }
    else{
        // No one is present
        if(arr[arr_pos].load(ACQ) == 0){
            if(arr[arr_pos].compare_exchange_strong(zero, temp, ACQ_REL)){
                sleep(waitTime);
                // Someone entered val > 0
                if(arr[arr_pos].load(ACQ) > 0){
                    int v;
                    v = arr[arr_pos].load(ACQ);
                    if(arr[arr_pos].compare_exchange_strong(v, 0)) return v;
                    else return -1;
                }
            }
        }
        
        else if(arr[arr_pos].load(ACQ) > 0){
            int v;
            v = arr[arr_pos].load(ACQ);
            if(arr[arr_pos].compare_exchange_strong(v, 0)) return v;
            else return -1;
        }
        
        return -1;
    }
        
}

void push(int value, size_t tid){
    bool acq = false;
    int ret, arr_pos;
    do{
        acq = lk->try_lock();
        if(acq){            
            st.push(value);
            lk->unlock();            
            break;
        }
        else{
            srand(time(NULL));
            arr_pos = rand() % 5;
            ret = elimArr(tid, 1, arr_pos, 1, value);
            if(ret == -3) break;
        }        
    } while(true);
}

int pop(size_t tid){
    int t = -1, arr_pos, ret;
    bool acq = false;
    
    do{
        acq = lk->try_lock();
        if(acq){
            if(!st.empty()){        
                t = st.top();
                st.pop();
            }            
            lk->unlock();
            return t;
        }
        else{            
            srand(time(NULL));
            arr_pos = rand() % 5;
            ret = elimArr(tid, 1, arr_pos, 2, -1);
            if(ret != -1) return ret;            
        }
    }
    while(true);        
}



void* controller(size_t tid, int op_num, int push_val){         
    
    bar->arrive_and_wait();
    
    if(tid == 0) clock_gettime(CLOCK_MONOTONIC,&startTime);    
    int val;      
    
    switch(op_num){
        case 1:             
            push(push_val, tid);   
            printf("%zu Pushing: %d\n", tid, push_val);
            break;
            
        case 2:             
            val = pop(tid);      
            printf("%zu Popping: %d\n", tid, val);
            break;                    
    }
        
    bar->arrive_and_wait();  
    
    if(tid == 0) clock_gettime(CLOCK_MONOTONIC,&endTime);
    return 0;
}

int main(int argc, const char* argv[]){
	

	// launch threads
	int ret; size_t i;
    NUM_THREADS = stoi(argv[1]);
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
        if(t == 1) push_num = rand() % 100;
        else push_num = -1;
        
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
