#include <bits/stdc++.h>
// #include <barrier>
#include "./inclusives/tas.h"
#include "./inclusives/test_tas.h"
#include "./inclusives/ticket_lock.h"
#include "./inclusives/MCS.h"
// #include "./inclusives/sense.h"
// mado6037@ecen4313-fl23-0
using namespace std;
#define SEQ memory_order_seq_cst
#define RELAX memory_order_relaxed

vector<thread*> threads;
size_t NUM_THREADS;
int NUM_ITERATIONS = 10;

///////////////////////////////////////////
// Tas
atomic<bool> tas_flag = 0;

// Test_Tas
atomic<bool> test_tas_flag = 0;

// Ticket Lock
atomic<int> next_number = 0;
atomic<int> now_serving = 0;

// MCS Lock
atomic <Node*> tail = nullptr;

///////////////////////////////////////////

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


int cntr=0;
unsigned long long elapsed_ns;

void* thread_tas(size_t tid){
    
    if(tid == 1){        
        clock_gettime(CLOCK_MONOTONIC,&startTime);
    }        
    
    for(int i = 0; i < NUM_ITERATIONS; i++){        
        tas_lock(tas_flag);        
        cntr++;                        
        tas_unlock(tas_flag);        
    }
    
    if(tid == 1){        
        clock_gettime(CLOCK_MONOTONIC,&endTime);
        elapsed_ns = (endTime.tv_sec-startTime.tv_sec)*1000000000 + (endTime.tv_nsec-startTime.tv_nsec);        
    }
    
    return 0;
}


void tas_func(){
    
    global_init();
    
    threads.resize(NUM_THREADS);
    size_t i;
    
    for(i = 0; i < NUM_THREADS; ++i){     
        threads[i] = new thread(thread_tas, i+1);
    }    
    
    // join_threads();      
    for(i=0; i<NUM_THREADS; i++){
        threads[i]->join();        
        delete threads[i];
    }
    
    global_cleanup();
}

//////////////////////////////////////////////////////////////////////////////////////


void* thread_ttas(size_t tid){
    if(tid == 1){        
        clock_gettime(CLOCK_MONOTONIC,&startTime);
    }
    for(int i = 0; i < NUM_ITERATIONS; i++){        
        test_tas_lock(test_tas_flag);        
        cntr++;
        // printf("Ttas: %zu, Counter: %d\n", tid, cntr);
        // cout << tid << "\t" << counter << endl;                
        test_tas_unlock(test_tas_flag);        
    }
    if(tid == 1){        
        clock_gettime(CLOCK_MONOTONIC,&endTime);
        elapsed_ns = (endTime.tv_sec-startTime.tv_sec)*1000000000 + (endTime.tv_nsec-startTime.tv_nsec);        
    }
    return 0;
}


void ttas_func(){
    
    global_init();
    
    threads.resize(NUM_THREADS);
    size_t i;
    
    for(i = 0; i < NUM_THREADS; ++i){            
        threads[i] = new thread(thread_ttas, i+1);
    }
    
    // join_threads();      
    for(i=0; i<NUM_THREADS; i++){
        threads[i]->join();        
        delete threads[i];
    }
    
    global_cleanup();
}

//////////////////////////////////////////////////////////////////////////////////////

void* thread_ticket(size_t tid){
    if(tid == 1){        
        clock_gettime(CLOCK_MONOTONIC,&startTime);
    }
    
    for(int i = 0; i < NUM_ITERATIONS; i++){  
                
        ticket_lock(next_number, now_serving);
        cntr++;
        // printf("Ticket:%zu, Counter: %d\n", tid, cntr);
        // cout << tid << "\t" << counter << endl;                
        ticket_unlock(now_serving);
    }
    
    if(tid == 1){        
        clock_gettime(CLOCK_MONOTONIC,&endTime);
        elapsed_ns = (endTime.tv_sec-startTime.tv_sec)*1000000000 + (endTime.tv_nsec-startTime.tv_nsec);        
    }
    
    return 0;
}

void ticket_func(){
    
    global_init();
    
    threads.resize(NUM_THREADS);
    size_t i;
    for(i = 0; i < NUM_THREADS; ++i){            
        threads[i] = new thread(thread_ticket, i+1);
    }
    
    // join_threads();      
    for(i=0; i<NUM_THREADS; i++){
        threads[i]->join();        
        delete threads[i];
    }
    
    global_cleanup();
}

//////////////////////////////////////////////////////////////////////////////////////

void* thread_MCS(size_t tid){
    thread_local Node* k;
    if(tid == 1){        
        clock_gettime(CLOCK_MONOTONIC,&startTime);
    }
    
    for(int i = 0; i < NUM_ITERATIONS; i++){        
        acquire(k, tail);
        cntr++;
        // printf("MCS:%zu, Counter: %d\n", tid, cntr);
        // cout << tid << "\t" << counter << endl;                
        release(k, tail);
    }
    
    if(tid == 1){        
        clock_gettime(CLOCK_MONOTONIC,&endTime);
        elapsed_ns = (endTime.tv_sec-startTime.tv_sec)*1000000000 + (endTime.tv_nsec-startTime.tv_nsec);        
    }
    return 0;
}

void MCS_func(){
    
    global_init();
    
    threads.resize(NUM_THREADS);
    size_t i;
    
    for(i = 0; i < NUM_THREADS; ++i){            
        threads[i] = new thread(thread_MCS, i+1);
    }
    
    // join_threads();      
    for(i=0; i<NUM_THREADS; i++){
        threads[i]->join();        
        delete threads[i];
    }
    
    global_cleanup();
}

//////////////////////////////////////////////////////////////////////////////////////

void* thread_pthread(size_t tid){
    if(tid == 1){        
        clock_gettime(CLOCK_MONOTONIC,&startTime);
    }
    for(int i = 0; i < NUM_ITERATIONS; i++){        
        lk->lock();
        cntr++;
        // printf("pthread:%zu, Counter: %d\n", tid, cntr);
        // cout << tid << "\t" << counter << endl;                
        lk->unlock();
    }
    if(tid == 1){        
        clock_gettime(CLOCK_MONOTONIC,&endTime);
        elapsed_ns = (endTime.tv_sec-startTime.tv_sec)*1000000000 + (endTime.tv_nsec-startTime.tv_nsec);        
    }
    return 0;
}

void Pthread_func(){
    
    global_init();
    
    threads.resize(NUM_THREADS);
    size_t i;
    for(i = 0; i < NUM_THREADS; ++i){            
        threads[i] = new thread(thread_pthread, i+1);
    }
    
    // join_threads();      
    for(i=0; i<NUM_THREADS; i++){
        threads[i]->join();        
        delete threads[i];
    }
    
    global_cleanup();
}

//////////////////////////////////////////////////////////////////////////////////////

class Sense_Barrier{
    public:
        atomic<bool> sense;
        int cnt = 0;
        mutex* bar_lk;        
        int N;
    
        Sense_Barrier(int NUM_THREADS){
            bar_lk = new mutex();
             N = NUM_THREADS;
        }
    
        void wait(size_t t_id){            
            thread_local bool my_sense = 0;
            
            if(my_sense == false) my_sense = true;
            else my_sense = false;
            
            bar_lk->lock();                        
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

void * thread_sense(size_t t_id, Sense_Barrier* sense){    
    int val;
    
    if(t_id == 0){        
        clock_gettime(CLOCK_MONOTONIC,&startTime);
    }
    
    for(int i = 0; i < NUM_ITERATIONS * NUM_THREADS; i++){
                        
        val = i % NUM_THREADS;        
        if(val == t_id){
            cntr++;
            // printf("After: Tid: %zu, Counter: %d, i: %d\n", t_id, cntr, i);
        }    
        sense->wait(t_id);   
    }
    
    if(t_id == 0){        
        clock_gettime(CLOCK_MONOTONIC,&endTime);
        elapsed_ns = (endTime.tv_sec-startTime.tv_sec)*1000000000 + (endTime.tv_nsec-startTime.tv_nsec);        
    }
    
    return 0;
}


void Sense_func(){
    global_init();
    
    threads.resize(NUM_THREADS);
    Sense_Barrier* sense = new Sense_Barrier(NUM_THREADS);
    size_t i;
    
    for(i = 0; i < NUM_THREADS; ++i){            
        threads[i] = new thread(thread_sense, i, sense);
    }
    
    // join_threads();      
    for(i=0; i<NUM_THREADS; i++){
        threads[i]->join();        
        delete threads[i];
    }
    
    global_cleanup();
}

//////////////////////////////////////////////////////////////////////////////////////


void * thread_bar(size_t t_id){    
    int val;
    if(t_id == 0) clock_gettime(CLOCK_MONOTONIC,&startTime);
    for(int i = 0; i < NUM_ITERATIONS * NUM_THREADS; i++){
                        
        val = i % NUM_THREADS;        
        if(val == t_id){
            cntr++;
            // printf("Bar: %zu, Counter: %d, i: %d\n", t_id, cntr, i);
        }    
        bar->arrive_and_wait();   
    }                
    if(t_id == 0){        
        clock_gettime(CLOCK_MONOTONIC,&endTime);
        elapsed_ns = (endTime.tv_sec-startTime.tv_sec)*1000000000 + (endTime.tv_nsec-startTime.tv_nsec);        
    }
    return 0;
}


void Bar_func(){
    global_init();
    
    threads.resize(NUM_THREADS);    
    size_t i;
    
    for(i = 0; i < NUM_THREADS; ++i){            
        threads[i] = new thread(thread_bar, i);
    }
    
    // join_threads();      
    for(i=0; i<NUM_THREADS; i++){
        threads[i]->join();        
        delete threads[i];
    }
    
    global_cleanup();
}

//////////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char* argv[]){

    // parse args    
    // counter [--name] [-t NUM_THREADS] [-n NUM_ITERATIONS] [--bar=<sense,pthread,senserel>] [--lock=<tas,ttas,ticket,mcs,pthread,peterson,tasrel,ttasrel,mcsrel,petersonrel>] [-o out.txt]
    // ./counter --name     
    // ./counter -t 2 -n 10 --lock=tas -o out.txt
    
    vector<string> arV;
    int arV_size;
    for(int i=0; i<argc; i++) arV.emplace_back(argv[i]);
    
    // for(int i=0; i<argc; i++) cout << i << "I: " << arV[i] << " \n" ;
    
    arV_size = arV.size();
    
    if(arV_size > 2){
        
        // string ip_filename = "ip.txt";
        string op_filename = "./autograde_tests/million.my";
        string lock = "";
        string barrier = "";
        string num_threads = "4";       
        int counter = 1;
        
        while(counter < arV_size){            
            
            if(arV[counter] == "-o"){
                ++counter;
                op_filename = arV[counter];                
                // cout << "In output" << endl;
            }
            
            else if(arV[counter] == "-t"){
                ++counter;
                NUM_THREADS = size_t(stoi(arV[counter]));
                // cout << "In thread" << endl;
            }
            
            else if(arV[counter] == "-n"){
                ++counter;
                NUM_ITERATIONS = stoi(arV[counter]);
                // cout << "In thread" << endl;
            }
            
//             else if(arV[counter].substr(0, 2) == "-t"){
//                 // ++counter;
//                 // NUM_THREADS = stoi(arV[counter].substr(1, arV[counter].size()));
//                 NUM_THREADS = size_t(stoi(arV[counter].substr(2, arV[counter].size())));                
//                 // cout << NUM_THREADS << endl;                
//             }
            
//             else if(arV[counter].substr(0, 2) == "-n"){
//                 // ++counter;
//                 NUM_ITERATIONS = stoi(arV[counter].substr(2, arV[counter].size()));                
//                 // cout << NUM_ITERATIONS << endl;                
//             }
            
            else if(arV[counter].substr(0, 3) == "--b"){
                if(lock != "") {
                    cout << "Incorrect input";
                    return 0;
                }
                barrier = arV[counter].substr(6, arV[counter].size());
                // cout << "In barrier" << endl;
            }
            
            else if(arV[counter].substr(0, 3) == "--l"){
                if(barrier != "") {
                    cout << "Incorrect input";
                    return 0;
                }
                lock = arV[counter].substr(7, arV[counter].size());
                // cout << "In lock" << endl;
            }
            
            counter++;
        }                        
                        
        // NUM_THREADS = 2;
        // tas,ttas,ticket,mcs,pthread,
        if(barrier == ""){
            
            if(lock == "tas"){
                tas_func();          
            }
            else if(lock == "ttas"){
                ttas_func();          
            }
            else if(lock == "ticket"){
                ticket_func();
                // ttas_func();          
            }
            else if(lock == "mcs"){
                MCS_func();
            }
            else if(lock == "pthread"){
                Pthread_func();
            }
        }
        //sense,pthread
        else{
            if(barrier == "sense"){Sense_func();}
            else if(barrier == "pthread"){Bar_func();}
            // else if(lock == ""){}
        }
        
                
        
        // Save output in new file with the "op_filename"        
        ofstream op_file;
        op_file.open(op_filename);     
        cout << elapsed_ns << endl;
        // cout << cntr << endl;

        op_file << cntr << endl;                           
        op_file.close();          
    }
    
    else if(arV_size == 2 && arV[1] == "--name"){
        cout << "Manan Doshi\n";
    }                    
    
    else{
        cout << "Command not recognized as internal or external\n";
    }
    
    return 0;
}