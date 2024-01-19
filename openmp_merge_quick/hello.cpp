#include <bits/stdc++.h>
#include <omp.h>
omp_lock_t my_lock;

int counter = 0;

void hello(int tid){
    // printf("Hello from hello! from %d\n", tid);
    # pragma omp critical
    {
        counter++;
        printf("%d: %d\n", tid, counter);
    }
}

int main(){
    
    omp_set_num_threads(100);
    omp_init_lock(&my_lock);
    # pragma omp parallel 
    {
        int tid;
        tid = omp_get_thread_num();
        // printf("Hello World! from %d\n", tid);
        // hello(tid);
        omp_set_lock(&my_lock);
        counter++;
        printf("%d: %d\n", tid, counter);
        omp_unset_lock(&my_lock);
    }
    
    
    return 0;
}