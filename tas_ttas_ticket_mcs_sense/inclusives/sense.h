using namespace std;
#define SEQ memory_order_seq_cst
#define RELAX memory_order_relaxed

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