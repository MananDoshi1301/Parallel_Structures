#include <bits/stdc++.h>
using namespace std;

/*

This code contains mergesort and bucketsort using pthread locks, barriers leveraging fork join parallelism.

To run this code(check README.md for complete details):
$ make

$ shuf -i0-2147483643 -n382 > case1.txt

$ ./mysort -i case1.txt -o your1.txt --alg=forkjoin -t 4

$ ./mysort -i case1.txt -o your1.txt --alg=lkbucket -t 4


Code is divided as follows:
1. Global initializations
2. Custom repetitive functions
3. Mergesort functions
4. Fork_join_merge_function (forks and joins thread while calling mergesort functions)
5. Bucketsort functions
6. Fork_join_bucket_function (forks and joins thread while calling bucketsort functions)
7. int main to handle command-line querying

*/

// Global operations ----------------------------------------------------------------------------------------------------
// Create Thread arr and num_threads ------------------------------------------------
vector<thread *> threads;
size_t NUM_THREADS, ACTIVE_THREADS;

// vector<int> v;
int v_size, max_val_v, min_val_v, part_val_v, num_of_bucks;
// vector<multiset<int>> b;
multiset<int> s;

// Create barriers and mutex locks ------------------------------------------------
barrier<> *bar;
mutex *lk;
// vector<mutex*> arr_lk;

// Create time structs------------------------------------------------
struct timespec startTime, endTime;

void global_init() {
  bar = new barrier(NUM_THREADS);
  lk = new mutex();

  // arr_lk.resize(NUM_THREADS + 1);
  // for(int m = 0; m < arr_lk.size(); ++m) arr_lk[m] = new mutex();
  // b.resize(NUM_THREADS + 1);
  // for(int i=0; i<NUM_THREADS + 1; i++) b[i] = {};
}

void global_cleanup() {
  delete bar;
  delete lk;
  // for(int i = 0; i < arr_lk.size(); ++i) delete arr_lk[i];
}

// Global vectors and ints ------------------------------------------------
vector<int> v;
map<size_t, pair<int, int>> position_mapper;
int temp;

// Local initializers ------------------------------------------------

void local_init() {}
void local_cleanup() {}

// Custom Fucntions ------------------------------------------------
void join_threads() {
  size_t i;
  for (i = 0; i < NUM_THREADS; i++) {
    threads[i]->join();
    delete threads[i];
  }
}

// checking position of mapper
void print_pmapper() {
  cout << endl
       << endl;
  for (auto t : position_mapper) {
    printf("l: %d, h: %d\n", t.second.first, t.second.second);
  }
  cout << endl
       << endl;
}

// Merge functions start ------------------------------------------------------------------------------------------------

// Print all elements in vector v;
void show_elems() {
  for (auto i : v)
    cout << i << " ";
  cout << endl;
}

// Actual merge function
void merge(int low, int middle, int high, size_t t_id) {
  int left_ptr = low, right_ptr = middle + 1;
  vector<int> t;

  // lk->lock();
  // // show_elems();
  // lk->unlock();

  while (left_ptr <= middle && right_ptr <= high) {
    if (v[left_ptr] <= v[right_ptr]) {
      t.emplace_back(v[left_ptr]);
      left_ptr++;
    } else {
      t.emplace_back(v[right_ptr]);
      right_ptr++;
    }
  }

  for (; left_ptr <= middle; left_ptr++) {
    t.emplace_back(v[left_ptr]);
  }

  for (; right_ptr <= high; right_ptr++) {
    t.emplace_back(v[right_ptr]);
  }

  for (int i = 0; i <= high - low; i++) {
    v[low + i] = t[i];
  }
}

void mergesort(int low, int high, size_t t_id) {

  // First round merge sort pseudo
  if (low < high) {
    int middle = (low + high) / 2;
    mergesort(low, middle, t_id);
    mergesort(middle + 1, high, t_id);
    merge(low, middle, high, t_id);
  }
}

void *independent_separating(size_t t_id, int parse) {

  bar->arrive_and_wait();
  if (t_id == 0) {
    clock_gettime(CLOCK_MONOTONIC, &startTime);
  }

  if (t_id < ACTIVE_THREADS) {

    int low = t_id * (v_size / ACTIVE_THREADS);
    int high, mid;
    if (t_id == size_t(ACTIVE_THREADS - 1)) {
      high = v_size - 1;
    } else {
      high = (t_id + 1) * (v_size / ACTIVE_THREADS) - 1;
    }
    mid = (high + low) / 2;

    switch (parse) {
    case 0:
      position_mapper[t_id] = {low, high};
      mergesort(low, high, t_id);
      break;
    }

    bar->arrive_and_wait();
  }

  return 0;
}

// Fork and Join ------------------------------------------------------------------------------------------------

void fork_and_join_merge() {
  v_size = v.size();
  // cout << "V Size: " << v_size << endl;

  // Create threads

  // NUM_THREADS = stoi(argv[2]);
  // NUM_THREADS = 7;

  // Limiting the number of threads to be used
  if (NUM_THREADS > v_size / 2)
    NUM_THREADS = v_size / 2;

  ACTIVE_THREADS = NUM_THREADS;

  global_init();

  threads.resize(NUM_THREADS);
  size_t i;
  for (i = 0; i < NUM_THREADS; ++i) {
    // For merge_parallel: arg -> 0
    threads[i] = new thread(independent_separating, i, 0);
  }

  // join_threads();
  for (i = 0; i < NUM_THREADS; i++) {
    threads[i]->join();
    delete threads[i];
  }
  // print_pmapper();

  // Starting merge only
  int low, mid, high;
  size_t s = ACTIVE_THREADS * 2;

  while (ACTIVE_THREADS > 0) {

    if (ACTIVE_THREADS % 2 == 1) {
      // cout << endl << "Inside if\n";
      low = position_mapper[ACTIVE_THREADS - 2].first;
      mid = position_mapper[ACTIVE_THREADS - 1].first;
      high = position_mapper[ACTIVE_THREADS - 1].second;
      position_mapper[ACTIVE_THREADS - 2] = {low, high};
      merge(low, mid - 1, high, s);
      ACTIVE_THREADS--;
      // print_pmapper();
    }

    for (int k = 0; k < ACTIVE_THREADS / 2; k++) {
      low = position_mapper[k * 2].first;
      mid = position_mapper[(k + 1) * 2 - 1].first;
      high = position_mapper[(k + 1) * 2 - 1].second;
      position_mapper[k] = {low, high};
      merge(low, mid - 1, high, s);
    }

    ACTIVE_THREADS /= 2;
  }

  clock_gettime(CLOCK_MONOTONIC, &endTime);

  // cout << endl;
  // for(auto i: v) cout << i << " ";
  // cout << endl;

  unsigned long long elapsed_ns;
  elapsed_ns = (endTime.tv_sec - startTime.tv_sec) * 1000000000 + (endTime.tv_nsec - startTime.tv_nsec);
  printf("%llu\n", elapsed_ns);
  // double elapsed_s = ((double)elapsed_ns)/1000000000.0;
  // printf("Elapsed (s): %lf\n",elapsed_s);

  global_cleanup();
}

// ------------------------------------------------------------------------------------------------

void bucket(int low, int high) {

  // int dig;
  // multiset<int> st;
  for (int itr = low; itr <= high; itr++) {
    // dig = v[itr] / part_val_v;

    // printf("Bucket Before: Dig: %d, low: %d, high: %d, val: %d\n\n", dig, low, high, v[itr]);
    // apply lock
    // arr_lk[dig]->lock();
    lk->lock();

    // access set and insert
    // if(dig > NUM_THREADS) dig = NUM_THREADS;
    // if(b[dig].empty() == false){
    //     st = b[dig];
    // }
    // st.insert(v[itr]);
    // b[dig] = st;
    // st.clear();
    s.insert(v[itr]);

    // printf("Bucket After: Dig: %d, low: %d, high: %d, val: %d\n\n", dig, low, high, v[itr]);

    // release
    // arr_lk[dig]->unlock();
    lk->unlock();
  }
}

void *buck_independent_separating(size_t t_id, int parse) {

  // printf("Thread %zu reporting for duty\n",t_id);

  int low = t_id * (v_size / ACTIVE_THREADS);
  int high, mid;
  if (t_id == size_t(ACTIVE_THREADS - 1)) {
    high = v_size - 1;
  } else {
    high = (t_id + 1) * (v_size / ACTIVE_THREADS) - 1;
  }

  // printf("buck_indep_sort: id: %zu, low: %d, high: %d\n", t_id, low, high);
  bucket(low, high);

  return 0;
}

// -----------------------------------------------------------------------

void buck_sort() {
  // Get input thred num
  // for(auto i: v) cout << i << " ";
  // cout << endl;
  v_size = v.size();
  if (NUM_THREADS >= v_size)
    NUM_THREADS = v_size - 1;
  ACTIVE_THREADS = NUM_THREADS;

  // max_val_v = *max_element(v.begin(), v.end());
  // min_val_v = *min_element(v.begin(), v.end());
  // if(min_val_v > 0) min_val_v = 0;
  // part_val_v = ceil(float(max_val_v - min_val_v) / NUM_THREADS);
  // printf("\n\nMax: %d, min: %d, part: %d, threads: %zu\n\n", max_val_v, min_val_v, part_val_v, NUM_THREADS);
  global_init();

  threads.resize(NUM_THREADS);
  size_t i;

  clock_gettime(CLOCK_MONOTONIC, &startTime);

  for (i = 0; i < NUM_THREADS; ++i) {
    // For merge_parallel: arg -> 0
    threads[i] = new thread(buck_independent_separating, i, 0);
  }

  // join_threads();
  for (i = 0; i < NUM_THREADS; i++) {
    threads[i]->join();
    delete threads[i];
  }

  // printf("Adding to vector");
  v.clear();
  // for (int id = 0; id < b.size(); id++) {
  // auto itr = b[id].begin();

  // if(b[id].empty() == false){
  // for (;itr != b[id].end(); itr++){
  // cout << *itr << " ";
  // v.emplace_back(*itr);
  // }
  // cout << endl;
  // }
  // }
  for (auto i : s)
    v.emplace_back(i);

  clock_gettime(CLOCK_MONOTONIC, &endTime);

  // cout << endl;
  // for(auto i: v) cout << i << " ";
  // cout << endl;
  // for(auto i: v) cout << i << " ";
  // cout << endl;
  unsigned long long elapsed_ns;
  elapsed_ns = (endTime.tv_sec - startTime.tv_sec) * 1000000000 + (endTime.tv_nsec - startTime.tv_nsec);
  printf("%llu\n", elapsed_ns);
  // double elapsed_s = ((double)elapsed_ns)/1000000000.0;
  // printf("Elapsed (s): %lf\n",elapsed_s);
}

// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------

int main(int argc, const char *argv[]) {

  vector<string> arV;
  int arV_size;
  for (int i = 0; i < argc; i++)
    arV.emplace_back(argv[i]);

  // for(int i=0; i<argc; i++) cout << arV[i] << " " ;

  arV_size = arV.size();

  if (arV_size > 2) {

    string ip_filename = "ip.txt";
    string op_filename = "out.txt";
    string algo = "forkjoin";
    string num_threads = "4";
    int counter = 1;

    while (counter < arV_size) {
      if (arV[counter] == "-i") {
        ++counter;
        ip_filename = arV[counter];
        // cout << "In input" << endl;
      }

      else if (arV[counter] == "-o") {
        ++counter;
        op_filename = arV[counter];
        // cout << "In output" << endl;
      }

      else if (arV[counter] == "-t") {
        ++counter;
        num_threads = arV[counter];
        // cout << "In thread" << endl;
      }

      else if (arV[counter].substr(0, 2) == "--") {
        algo = arV[counter].substr(6, arV[counter].size());
        // cout << "In algo" << endl;
      }

      counter++;
    }

    NUM_THREADS = stoi(num_threads);

    // Open, read and store integers
    ifstream file;
    string line;
    file.open(ip_filename);
    int t;
    while (file.good()) {
      getline(file, line);
      try {
        if (line != "") {
          t = stoi(line);
          v.emplace_back(t);
        }
      } catch (int e) {
        cout << "An exception occurred. Exception Nr. " << e << '\n';
      }
    }

    file.close();

    // forkjoin,lkbucket
    if (algo == "forkjoin") {
      fork_and_join_merge();
    } else {
      buck_sort();
    }

    // Save output in new file with the "op_filename"
    ofstream op_file;
    op_file.open(op_filename);
    for (int i = 0; i < v.size(); i++) {
      op_file << v[i] << endl;
    }
    op_file.close();

    // cout << ip_filename << " " << op_filename << " " << num_threads << " " << algo << endl;
  }

  else if (arV_size == 2 && arV[1] == "--name") {
    cout << "Manan Doshi\n";
  }

  else {
    cout << "Command not recognized as internal or external\n";
  }

  return 0;
}