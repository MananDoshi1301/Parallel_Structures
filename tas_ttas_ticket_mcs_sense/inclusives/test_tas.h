using namespace std;
#define SEQ memory_order_seq_cst
#define RELAX memory_order_relaxed

bool test_tas_impl(atomic<bool> &x, memory_order MEM){
    bool exp = false;
    bool desired = true;
    return x.compare_exchange_strong(exp, desired, MEM);
}

void test_tas_lock(atomic<bool> &test_tas_flag){
    while(test_tas_flag.load(SEQ) == true || test_tas_impl(test_tas_flag, SEQ) == false){}
}

void test_tas_unlock(atomic<bool> &test_tas_flag){
    test_tas_flag.store(false, SEQ);
}