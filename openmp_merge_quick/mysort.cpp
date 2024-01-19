#include <bits/stdc++.h>
#include <omp.h>
/*
Example code:

### Generate sample numbers file
$ shuf -i0-2147483643 -n382 > case1.txt

### Make compilation
$ make

### Running object files
$ ./mysort -t 4 -i case1.txt  -o out.txt --alg=quick
$ ./mysort -t 2 -i case1.txt -o out.txt --alg=merge

### Valgrind tool to check memory leaks (first install to use)
$ valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./mysort -t 2 -i ./autograde_tests/256in1-10000.txt  -o out.txt --alg=merge

*/

using namespace std;

vector<int> v;
int total_threads = 4, v_size = 0;
int merge_threads = total_threads;
map<int, pair<int, int>> position_mapper;
struct timespec startTime, endTime;

// --------------------------------------------------------------------------------------

int get_pivot(int left, int right) {

  int i = left, j = right, pivot = v[left];

  while (i < j) {

    while (v[i] <= pivot && i <= right)
      i++;

    while (v[j] > pivot && j >= left)
      j--;

#pragma omp critical
    {
      if (i < j)
        swap(v[i], v[j]);
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

void quick_ctrl() {
  omp_set_num_threads(total_threads);
#pragma omp parallel
  {
#pragma omp single
    quick(0, v.size() - 1);
  }
}

// ---------------------------------------------------------------------------------------

void merge(int low, int middle, int high) {
  int left_ptr = low, right_ptr = middle + 1;
  vector<int> t;

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

void merge_proc(int low, int high) {
  if (low < high) {
    int mid = (low + high) / 2;
    merge_proc(low, mid);
    merge_proc(mid + 1, high);
    merge(low, mid, high);
  }
}

void merge_ctrl() {

  // Not exceeding less than 2 elements for a section
  if (total_threads > v_size / 2) {
    merge_threads = v_size / 2;
  } else {
    merge_threads = total_threads;
  }
  omp_set_num_threads(merge_threads);
  int NUM_THREADS = merge_threads;
  // printf("merge_threads: %d, no_of_threads: %d", merge_threads, NUM_THREADS);

#pragma omp parallel shared(v)
  // default(none) shared(NUM_THREADS, v)
  {
    int tid;
    tid = omp_get_thread_num();

    // Assign pointers to each thread
    int low, high;
    low = tid * (v_size / NUM_THREADS);
    if (tid == NUM_THREADS - 1)
      high = v_size - 1;
    else
      high = (tid + 1) * (v_size / NUM_THREADS) - 1;
    // printf("%d -- low: %d, high: %d\n", tid, low, high);

    // start sorting
    merge_proc(low, high);
    position_mapper[tid] = make_pair(low, high);
  }

  int low, mid, high;
  int ACTIVE_THREADS = merge_threads;
  while (ACTIVE_THREADS > 0) {

    if (ACTIVE_THREADS % 2 == 1) {
      // cout << endl << "Inside if\n";
      low = position_mapper[ACTIVE_THREADS - 2].first;
      mid = position_mapper[ACTIVE_THREADS - 1].first;
      high = position_mapper[ACTIVE_THREADS - 1].second;
      position_mapper[ACTIVE_THREADS - 2] = {low, high};
      merge(low, mid - 1, high);
      ACTIVE_THREADS--;
      // print_pmapper();
    }

    // #pragma omp for
    for (int k = 0; k < ACTIVE_THREADS / 2; k++) {
      low = position_mapper[k * 2].first;
      mid = position_mapper[(k + 1) * 2 - 1].first;
      high = position_mapper[(k + 1) * 2 - 1].second;
      position_mapper[k] = {low, high};
      merge(low, mid - 1, high);
    }

    ACTIVE_THREADS /= 2;
  }
}

// ------------------------------------------------------------------------------------------

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
    string algo = "merge";
    string num_threads = "4";
    int counter = 1;

    while (counter < arV_size) {
      if (arV[counter] == "-i") {
        ++counter;
        ip_filename = arV[counter];
      }

      else if (arV[counter] == "-o") {
        ++counter;
        op_filename = arV[counter];
      }

      else if (arV[counter] == "-t") {
        ++counter;
        num_threads = arV[counter];
      }

      else if (arV[counter].substr(0, 2) == "--") {
        algo = arV[counter].substr(6, arV[counter].size());
      }

      counter++;
    }

    // cout << "In input: " << ip_filename << endl;
    // cout << "In output: " << op_filename << endl;
    // cout << "In thread: " << num_threads << endl;
    // cout << "In algo: " << algo << endl;

    // Set number of threads
    total_threads = stoi(num_threads);

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

    v_size = v.size();
    file.close();

    // merge and quick
    if (algo == "merge") {
      clock_gettime(CLOCK_MONOTONIC, &startTime);
      merge_ctrl();
      // quick_ctrl();
      clock_gettime(CLOCK_MONOTONIC, &endTime);
    } else {
      clock_gettime(CLOCK_MONOTONIC, &startTime);
      // merge_ctrl();
      quick_ctrl();
      clock_gettime(CLOCK_MONOTONIC, &endTime);
    }

    unsigned long long elapsed_ns;
    elapsed_ns = (endTime.tv_sec - startTime.tv_sec) * 1000000000 + (endTime.tv_nsec - startTime.tv_nsec);
    printf("Elapsed (ns): %llu\n", elapsed_ns);
    double elapsed_s = ((double)elapsed_ns) / 1000000000.0;
    printf("Elapsed (s): %lf\n", elapsed_s);

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