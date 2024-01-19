#include <bits/stdc++.h>

using namespace std;

// Global operations
// Create Thread arr and num_threads
vector<thread *> threads;
size_t NUM_THREADS, ACTIVE_THREADS;

// Create barriers and mutex locks
barrier<> *bar;
mutex *lk;

// Create time structs
struct timespec startTime, endTime;

void global_init() {
  bar = new barrier(NUM_THREADS);
  lk = new mutex();
}

void global_cleanup() {
  delete bar;
  delete lk;
}

// Global vectors and ints
vector<int> v = {100, 400, 80, 5, 12, 9, 123, 120, 121};
map<int, pair<int, int>> position_mapper;
int temp;
int v_size;

// Local initializers

void local_init() {}

void local_cleanup() {}

// Merge functions start

void merge(int low, int middle, int high) {
  int left_ptr = low, right_ptr = middle + 1;
  vector<int> t;

  lk->lock();
  for (auto i : v)
    cout << i << " ";
  printf("\nlow: %d, mid: %d, high: %d\n", low, middle, high);
  // cout << endl;
  lk->unlock();

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

void mergesort(int low, int high) {

  if (low < high) {
    int middle = (low + high) / 2;
    mergesort(low, middle);
    mergesort(middle + 1, high);
    merge(low, middle, high);
  }
}

void merge_parallel(int low, int high) {

  mergesort(low, high);
}

void *independent_separating(size_t t_id, int parse) {

  // Assign blocks

  // even
  int low = t_id * (v_size / NUM_THREADS);
  int high, mid;
  if (t_id == NUM_THREADS - 1) {
    high = v_size - 1;
  } else {
    high = (t_id + 1) * (v_size / NUM_THREADS) - 1;
  }
  mid = (high + low) / 2;

  switch (parse) {
  case 0:
    merge_parallel(low, high);
    break;

  case 1:
    // Never use mid here
    if (NUM_THREADS == 1) {
      cout << endl
           << endl
           << endl;
      cout << t_id << " " << position_mapper[1].first << " " << position_mapper[1].second;
      cout << endl
           << endl
           << endl;
      mid = position_mapper[1].first;
    } else {
      position_mapper[t_id] = {low, high};
    }
    merge(low, mid, high);
    break;
  }

  return 0;
}

// Merge functions end

void join_threads() {

  size_t i;
  for (i = 0; i < NUM_THREADS; i++) {
    threads[i]->join();
    delete threads[i];
  }
}

int main() {

  // For added elements insert using v.emplace_back()
  v_size = v.size();

  // Create threads

  NUM_THREADS = 4;

  // Limiting the number of threads to be used
  if (NUM_THREADS > v_size / 2)
    NUM_THREADS = v_size / 2;

  global_init();
  ACTIVE_THREADS = NUM_THREADS;

  threads.resize(NUM_THREADS);
  size_t i;
  for (i = 0; i < NUM_THREADS; ++i) {
    // For merge_parallel: arg -> 0
    threads[i] = new thread(independent_separating, i, 0);
  }

  join_threads();

  // Starting merge only
  printf("\nStarting merge only\n");

  while (NUM_THREADS > 0) {
    NUM_THREADS /= 2;
    for (i = 0; i < NUM_THREADS; ++i) {
      // For merge_sort: arg -> 1
      threads[i] = new thread(independent_separating, i, 1);
    }
    join_threads();
  }

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

  cout << endl;
  for (auto i : v)
    cout << i << " ";
  cout << endl;

  global_cleanup();
}