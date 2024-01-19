#include <bits/stdc++.h>

using namespace std;

// Global operations
// Create Thread arr and num_threads
vector<thread*> threads;
size_t NUM_THREADS, ACTIVE_THREADS;

// Create barriers and mutex locks
barrier<> *bar; 
mutex* lk;

// Create time structs
struct timespec startTime, endTime;

void global_init(){
    bar = new barrier(NUM_THREADS);
    lk = new mutex();
}

void global_cleanup(){
    delete bar;
    delete lk;
}

// Global vectors and ints
vector <int> v;
map <size_t, pair<int, int>> position_mapper;
int temp;
int v_size;


// Local initializers

void local_init(){}
void local_cleanup(){}

// Custom Fucntions
// void join_threads(){        
//     size_t i;
//     for(i=0; i<NUM_THREADS; i++){
//         threads[i]->join();        
//         delete threads[i];
//     }
// }

// Merge functions start

// Print all elements in vector v;
void show_elems(){    
    for(auto i: v) cout << i << " ";        
    cout << endl;    
}

// Actual merge function 
void merge(int low, int middle, int high, size_t t_id){
    int left_ptr = low, right_ptr = middle + 1;
    vector <int> t;        
    
    // lk->lock();
    // // show_elems();
    // lk->unlock();
    
    while(left_ptr <= middle && right_ptr <= high){
        if(v[left_ptr] <= v[right_ptr]) {
            t.emplace_back(v[left_ptr]);
            left_ptr++;
        }
        else{
            t.emplace_back(v[right_ptr]);
            right_ptr++;
        }
    }

    for(;left_ptr <= middle; left_ptr++){
        t.emplace_back(v[left_ptr]);
    }
    
    for(;right_ptr <= high; right_ptr++){
        t.emplace_back(v[right_ptr]);
    }

    for(int i=0; i <= high - low; i++){
        v[low + i] = t[i];
    }        
    
    // lk->lock();
    // printf("merge: T_id-> %zu, low: %d, mid: %d, high: %d\n", t_id, low, middle, high);
    // // show_elems();
    // cout << endl << endl;
    // lk->unlock();
}

void mergesort(int low, int high, size_t t_id){
    
    // First round merge sort pseudo     
    if(low < high){
        int middle = (low + high) / 2;
        mergesort(low, middle, t_id);
        mergesort(middle + 1, high, t_id);      
        merge(low, middle, high, t_id);
    }
}


void* independent_separating(size_t t_id, int parse){
    
    // printf("Thread %zu reporting for duty\n",t_id);
    bar->arrive_and_wait();
    if(t_id==0){
        clock_gettime(CLOCK_MONOTONIC,&startTime);
    }
    
    if(t_id < ACTIVE_THREADS){        
        
        int low = t_id * ( v_size / ACTIVE_THREADS);
        int high, mid;
        if(t_id == size_t(ACTIVE_THREADS - 1)){
            // printf("\n\nlast_thread: t_id -> %zu\n", t_id);
            high = v_size - 1;    
        }
        else{
            high = (t_id + 1) * (v_size / ACTIVE_THREADS) - 1;    
        }        
        mid = (high + low) / 2;    


        switch(parse){
            case 0:
                position_mapper[t_id] = {low, high};
                // printf("checking mapper: t_id->%zu, low->%d, high->%d\n", t_id, position_mapper[t_id].first, position_mapper[t_id].second);
                mergesort(low, high, t_id);    
                break;

            //Not used             
            case 1:
                // Never use mid here
                if(t_id == ACTIVE_THREADS - 1 && ACTIVE_THREADS == 1) 
                    mid = position_mapper[1].first - 1;
                else{
                    position_mapper[t_id] = {low, high};
                }                                
                
                merge(low, mid, high, t_id);                         
                break;
        }  
        
        bar->arrive_and_wait();
    }
    
    return 0;
}

// Merge functions end

void print_pmapper(){
    cout << endl << endl;
    for(auto t : position_mapper){
        printf("l: %d, h: %d\n", t.second.first, t.second.second);
    }
    cout << endl << endl;
}

int main(int argc, const char* argv[]) {
        
    // For added elements insert using v.emplace_back()
    for (int i = 0; i < stoi(argv[1]); i++)
        v.emplace_back(rand() % 100);
    
    // for (int i = 0; i < 52; i++)
    //     v.emplace_back(rand() % 100);
    
    v_size = v.size();    
    // cout << "V Size: " << v_size << endl;
    
    // Create threads
    
    NUM_THREADS = stoi(argv[2]);
    // NUM_THREADS = 7;    
    
    // Limiting the number of threads to be used    
    if(NUM_THREADS > v_size / 2) NUM_THREADS = v_size / 2;
    
    ACTIVE_THREADS = NUM_THREADS;
    
    global_init();
    
    threads.resize(NUM_THREADS);
    size_t i;
    for(i = 0; i < NUM_THREADS; ++i){
        // For merge_parallel: arg -> 0         
        threads[i] = new thread(independent_separating, i, 0);
    }
    
    // join_threads();      
    for(i=0; i<NUM_THREADS; i++){
        threads[i]->join();        
        delete threads[i];
    }
    // print_pmapper();
    
    // Starting merge only  
    int low, mid, high;
    size_t s = ACTIVE_THREADS * 2;    
        
    while(ACTIVE_THREADS > 0){
        
        if(ACTIVE_THREADS % 2 == 1){
            // cout << endl << "Inside if\n"; 
            low = position_mapper[ACTIVE_THREADS - 2].first;
            mid = position_mapper[ACTIVE_THREADS - 1].first;
            high = position_mapper[ACTIVE_THREADS - 1].second;
            position_mapper[ACTIVE_THREADS - 2] = {low, high};
            merge(low, mid - 1, high, s);
            ACTIVE_THREADS--;
            // print_pmapper();
        }
        
        for(int k = 0; k < ACTIVE_THREADS / 2; k++){
            low = position_mapper[k * 2].first;
            mid = position_mapper[(k+1) * 2 - 1].first;
            high = position_mapper[(k+1) * 2 - 1].second;
            position_mapper[k] = {low, high};
            merge(low, mid - 1, high, s);
        }
        
        ACTIVE_THREADS /= 2;
    }    
       
    
    clock_gettime(CLOCK_MONOTONIC,&endTime);
    
    cout << endl;
    for(auto i: v) cout << i << " ";
    cout << endl;
    
    unsigned long long elapsed_ns;
    elapsed_ns = (endTime.tv_sec-startTime.tv_sec)*1000000000 + (endTime.tv_nsec-startTime.tv_nsec);
    printf("Elapsed (ns): %llu\n",elapsed_ns);
    double elapsed_s = ((double)elapsed_ns)/1000000000.0;
    printf("Elapsed (s): %lf\n",elapsed_s);
    
    global_cleanup();
}