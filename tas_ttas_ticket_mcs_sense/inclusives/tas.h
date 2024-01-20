using namespace std;
#define SEQ memory_order_seq_cst
#define RELAX memory_order_relaxed

bool tas_impl(atomic<bool> &x, memory_order MEM){
    bool exp = false;
    bool desired = true;
    return x.compare_exchange_strong(exp, desired, MEM);
}

void tas_lock(atomic<bool> & tas_flag){
    while(tas_impl(tas_flag, SEQ) == false){}
}

void tas_unlock(atomic<bool> & tas_flag){
    tas_flag.store(false, SEQ);
}
