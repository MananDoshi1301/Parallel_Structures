#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

vector <int> v = {10, 3, 2, 1, 13, 27, 91, 1000, 2000, 100, 400, 80, 5, 12};
// vector <int> v = {10, 1, 13, 3};


int get_pivot(int left, int right){    

    int i = left, j = right, pivot = v[left];

    while(i < j) {

        while(v[i] <= pivot && i <= right) i++;

        while(v[j] > pivot && j >= left) j--;

        #pragma omp critical
        {
            if(i < j) swap(v[i], v[j]);
        }        
    }

    swap(v[j], v[left]);

    return j;

}


void quick(int low, int high) {
    if (low < high) {
        int pivot = get_pivot(low, high);

        #pragma omp parallel sections
        {
            #pragma omp section
            {
                quick(low, pivot - 1);
            }

            #pragma omp section
            {
                quick(pivot + 1, high);
            }
        }
    }
}

int main(){
    
    omp_set_num_threads(4);   
    
    int NUM_THREADS = omp_get_num_threads();
    #pragma omp parallel
    {
        #pragma omp single
        quick(0, v.size() - 1);
    }        
        
    cout << endl;
    for(int i=0; i<v.size(); i++) cout << v[i] << " "; 
    cout << endl;    
    
    return 0;
}
