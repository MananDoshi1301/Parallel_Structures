#include <bits/stdc++.h>
using namespace std;
#define REL memory_order_release
#define ACQ memory_order_acquire
#define ACQ_REL memory_order_acq_rel

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

atomic <int> arr[3];

void* controller(size_t tid, int push_val, int pos){         
    
    arr[pos].store(push_val, ACQ_REL);
    return 0;
}

int main(){
    
    int ret; size_t i;    
    NUM_THREADS = 4;
    global_init();    
	threads.resize(NUM_THREADS);
    
    
    // Generate Random Number    
    // 1 = push, 2 = pop
    int  t;    
    int push_num = -1, arr_pos = 0;
    
    // Seed to get different value in rands     
    srand (time(NULL));
	for(i=0; i<NUM_THREADS; i++){        
        
        // If pushing, get rand value otherwise -1    
        arr_pos = (rand() % 3);
        push_num = rand() % 100;
        printf("%zu will push %d at %d\n", i, push_num, arr_pos);                
        
        // printf("%zu\n", i);
		threads[i] = new thread(controller,i, push_num, arr_pos);
	}

	// join threads
	for(size_t i=0; i<NUM_THREADS; i++){
		threads[i]->join();
		// printf("joined thread %zu\n",i);
		delete threads[i];
	}
    
    for(int i=0; i<3; i++){
        cout << arr[i].load(ACQ) << endl;
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