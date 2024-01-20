#include <bits/stdc++.h>
using namespace std;

vector<thread*> threads;
size_t NUM_THREADS;

// Query to run this: ./sgl_queue 20
queue<int> q;
 
mutex* lk;
barrier<> *bar; 

struct timespec startTime, endTime;

void global_init(){	
	lk = new mutex();
    bar = new barrier(NUM_THREADS);
}
void global_cleanup(){	
	delete lk;
    delete bar;
}

void local_init(){}
void local_cleanup(){}

void enqueue(int value, size_t tid){
    lk->lock();
    q.push(value);
    // printf("%zu Enqueuing: %d\n", tid, value);
    lk->unlock();
}

int dequeue(size_t tid){
    int t = -1;
    lk->lock();
    if(!q.empty()){        
        t = q.front();
        q.pop();
        // printf("%zu Dequeuing: %d\n", tid, t);
    }    
    lk->unlock();
    return t;
}

int front(size_t tid){
    int t = -1;
    lk->lock();
    if(!q.empty()){        
        t = q.front();    
        // printf("%zu front: %d\n", tid, t);
    }
    lk->unlock();
    return t;
}

int back(size_t tid){
    int t = -1;
    lk->lock();
    if(!q.empty()){        
        t = q.back();    
        // printf("%zu back: %d\n", tid, t);
    }
    lk->unlock();
    return t;
}

bool isEmpty(size_t tid){
    lk->lock();
    bool p = q.empty();
    // printf("%zu isEmpty: %d\n", tid, p);
    lk->unlock();
    
    return p;
}

void* controller(size_t tid, int op_num, int push_val){
    
    bar->arrive_and_wait();
    
    if(tid == 0) clock_gettime(CLOCK_MONOTONIC,&startTime);    
    int val;      
    
    switch(op_num){
        case 1:             
            enqueue(push_val, tid);    
            printf("%zu Pushing: %d\n", tid, push_val);
            break;
            
        case 2:                  
            val = dequeue(tid);     
            printf("%zu Popping: %d\n", tid, val);
            break;
            
        case 3:
            val = front(tid);
            printf("%zu Front: %d\n", tid, val);
            break;
            
        case 4:             
            val = back(tid); 
            printf("%zu Back: %d\n", tid, val);
            break;
            
        case 5:             
            val = isEmpty(tid);  
            printf("%zu Empty: %d\n", tid, val);
            break;
    }
        
    bar->arrive_and_wait();  
    
    if(tid == 0) clock_gettime(CLOCK_MONOTONIC,&endTime);
    return 0;
}

int main(int argc, const char* argv[]){

	

	int ret; size_t i;
    NUM_THREADS = stoi(argv[1]);
    global_init();
	threads.resize(NUM_THREADS);
    
    
    // Generate Random Number    
    // 1 = enqueue, 2 = dequeue, 3 = front, 4 = back, 5 = empty;    
    int  t;    
    int push_num = -1;
    
    // Seed to get different value in rands     
    srand (time(NULL));
	for(i=0; i<NUM_THREADS; i++){
        
        // Identify which operation is being performed         
        t = (rand() % 5) + 1;
        
        // If pushing, get rand value otherwise -1         
        if(t == 1) push_num = rand() % 500;
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
