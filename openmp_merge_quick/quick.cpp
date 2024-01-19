#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

vector <int> v = {10, 3, 2, 1, 13, 27, 91, 1000, 2000, 100, 400, 80, 5, 12};
// vector <int> v = {10, 1, 13};
queue <pair<int, int>> tasks;


int get_pivot(int left, int right){    

    int i = left, j = right, pivot = v[left];

    while(i < j) {

        while(v[i] <= pivot && i <= right) i++;

        while(v[j] > pivot && j >= left) j--;

        if(i < j) swap(v[i], v[j]);
    }

    swap(v[j], v[left]);

    return j;

}

pair<int, int> read_task(int tid){
    pair<int, int> q;
    printf("Beginning read for %d\n", tid);
    # pragma omp critical    
    {
        q = tasks.front();
        printf("Read for %d\n", tid);
        tasks.pop();              
    }
    printf("Returning for %d\n", tid);
    return q;
}

void push_task(int a, int b){
    # pragma omp critical
    {
        tasks.push(make_pair(a, b));
    }
}

int main(){
    
    omp_set_num_threads(4);    
    // Not exceeding less than 2 elements for a section     
    // if(omp_get_num_threads() > v.size() / 2) omp_set_num_threads(v.size() / 2);
    
    int NUM_THREADS = omp_get_num_threads();
    #pragma omp parallel default(none) shared(NUM_THREADS, v, tasks)
    {
        int tid;
        tid = omp_get_thread_num();        
        
        // Get master to input the first thread
        #pragma omp master
        {
            // printf("Master executing\n");
            int low = 0, high = v.size() - 1;
            int pivot = get_pivot(low, high);            
            tasks.push(make_pair(low, pivot - 1));
            tasks.push(make_pair(pivot + 1, high));            
        }
                
        // Get pivots and input into the task                                 
        pair <int, int> p1, p2;        
        int pivot1, pivot2; 
        while(tasks.empty() != true){
            #pragma omp master
            {
                // Get the pair to process               
                printf("Entered\n");   
                p1 = read_task(tid);                              
                
                // Find pivot on that [low, high]
                pivot1 = get_pivot(p1.first, p1.second);                
                printf("%d: %d\n", p1.first, p1.second);   
                // if(high - low > 3){
                //     push
                // }
            }
                        
            #pragma omp single
            {
                
                // Get the pair to process                
                printf("Beginning read 2\n");
                p2 = read_task(tid);                                

                // Find pivot on that [low, high]
                pivot2 = get_pivot(p2.first, p2.second); 
                printf("%d: %d\n", p2.first, p2.second);
                
            }            
            // # pragma omp barrier
        }
        
        
    }
    
    pair <int, int> p;
    while(tasks.empty() != true) {
        p = tasks.front();
        printf("Pair: %d %d\n", p.first, p.second);
        tasks.pop();
    }
        
    cout << endl;
    for(int i=0; i<v.size(); i++) cout << v[i] << " "; 
    cout << endl;    
    
    return 0;
}
