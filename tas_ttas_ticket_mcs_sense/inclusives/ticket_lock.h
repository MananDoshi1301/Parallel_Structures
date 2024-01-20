using namespace std;
#define SEQ memory_order_seq_cst
#define RELAX memory_order_relaxed


void ticket_lock(atomic<int> &next_number, atomic<int> &now_serving){
    int num = next_number.fetch_add(1, SEQ);
    while(num != now_serving.load(SEQ)){}
}

void ticket_unlock(atomic<int> &now_serving){
    now_serving.fetch_add(1, SEQ);
}