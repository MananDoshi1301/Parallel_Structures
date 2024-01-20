#include <bits/stdc++.h>

#include <barrier>

#include "./inclusives/MCS.h"
#include "./inclusives/tas.h"
#include "./inclusives/test_tas.h"
#include "./inclusives/ticket_lock.h"

using namespace std;
#define SEQ memory_order_seq_cst
#define RELAX memory_order_relaxed

// ./mysort -i autograde_tests/6200000in1-10000000.txt -o out.txt -t 50 --algo=lkbucket => 42639055583

// ./mysort -i autograde_tests/6200000in1-10000000.txt -o out.txt -t 200 --algo=lkbucket => 43037070981 Faster

// ./mysort -i autograde_tests/10in0-20.txt -o out.txt -t 50 --algo=lkbucket => 1010416

// ./mysort -i autograde_tests/550000in1-1000000skew.txt -o out.txt -t 200 --algo=lkbucket => 3641830846 Faster

// Global operations ----------------------------------------------------------------------------------------------------

// Create Thread arr and num_threads ------------------------------------------------
vector<thread *> threads;
size_t NUM_THREADS, ACTIVE_THREADS;

int v_size;
multiset<int> s;

//////////////////////////////////////////////////////////////////////

// Tas
atomic<bool> tas_flag = 0;

// Test_Tas
atomic<bool> test_tas_flag = 0;

// Ticket Lock
atomic<int> next_number = 0;
atomic<int> now_serving = 0;

// MCS Lock
atomic<Node *> tail = nullptr;

//////////////////////////////////////////////////////////////////////

// Create barriers and mutex locks ------------------------------------------------
barrier<> *bar;
mutex *lk;

// Create time structs------------------------------------------------
struct timespec startTime, endTime;
unsigned long long elapsed_ns;

void global_init() {
  bar = new barrier(NUM_THREADS);
  lk = new mutex();
}

void global_cleanup() {
  delete bar;
  delete lk;
}

// Global vectors and ints ------------------------------------------------
vector<int> v;

// Local initializers ------------------------------------------------

void local_init() {}
void local_cleanup() {}

// ------------------------------------------------------------------------------------------------

void bucket_tas(int low, int high) {

  // void tas_lock(atomic<bool> & tas_flag)
  // void tas_unlock(atomic<bool> & tas_flag)

  for (int itr = low; itr <= high; itr++) {

    // Lock
    tas_lock(tas_flag);
    // cout << "tas" << endl;
    s.insert(v[itr]);

    // Unlock
    tas_unlock(tas_flag);
  }
}

void bucket_ttas(int low, int high) {
  // void test_tas_lock(atomic<bool> &test_tas_flag)
  // void test_tas_unlock(atomic<bool> &test_tas_flag)

  for (int itr = low; itr <= high; itr++) {

    // Lock
    test_tas_lock(test_tas_flag);
    // cout << "ttas" << endl;
    s.insert(v[itr]);

    // Unlock
    test_tas_unlock(test_tas_flag);
  }
}

void bucket_ticket(int low, int high) {
  // void ticket_unlock(atomic<int> &now_serving)
  // void ticket_lock(atomic<int> &next_number, atomic<int> &now_serving)
  for (int itr = low; itr <= high; itr++) {

    // Lock
    ticket_lock(next_number, now_serving);
    // cout << "ticket" << endl;
    s.insert(v[itr]);

    // Unlock
    ticket_unlock(now_serving);
  }
}

void bucket_MCS(int low, int high) {
  // void release(Node* &myNode, atomic <Node*>& tail)
  // void acquire(Node* &myNode, atomic <Node*>& tail)
  thread_local Node *k;

  for (int itr = low; itr <= high; itr++) {

    // Lock
    acquire(k, tail);
    // cout << "mcs" << endl;
    s.insert(v[itr]);

    // Unlock
    release(k, tail);
  }
}

void bucket_pthread(int low, int high) {
  for (int itr = low; itr <= high; itr++) {

    // Lock
    lk->lock();
    // cout << "pthread" << endl;
    s.insert(v[itr]);

    // Unlock
    lk->unlock();
  }
}

///////////////////////////////////////////////////////////////////////////////////

class Sense_Barrier {
  public:
  atomic<bool> sense;
  int cnt = 0;
  mutex *bar_lk;
  int N;

  Sense_Barrier(int NUM_THREADS) {
    bar_lk = new mutex();
    N = NUM_THREADS;
  }

  void wait(size_t t_id) {
    thread_local bool my_sense = 0;

    if (my_sense == false)
      my_sense = true;
    else
      my_sense = false;

    bar_lk->lock();
    cnt++;
    if (cnt == N) {
      cnt = 0;
      bar_lk->unlock();
      sense.store(my_sense, SEQ);
    }

    else {
      bar_lk->unlock();
      while (my_sense != sense.load(SEQ)) {
      }
    }
  }
};

void buck_sense(int low, int high, size_t t_id, Sense_Barrier *sense) {

  for (int itr = 0; itr < v_size; itr++) {

    if (itr >= low && itr <= high) {
      s.insert(v[itr]);
      // cout << "sense" << endl;
      // cout << t_id << endl;
    }

    sense->wait(t_id);
  }
}

void buck_pthread_bar(int low, int high) {
  for (int itr = 0; itr < v_size; itr++) {

    if (itr >= low && itr <= high) {
      s.insert(v[itr]);
      // cout << "Pthread: " << endl;
    }

    bar->arrive_and_wait();

    // Add buck_pthread_bar in indep_separating_sense
  }
}

void *buck_independent_separating_sense(size_t t_id, int parse, Sense_Barrier *sense, void (*operation)(int, int, size_t, Sense_Barrier *)) {

  int low = t_id * (v_size / ACTIVE_THREADS);
  int high, mid;
  if (t_id == size_t(ACTIVE_THREADS - 1)) {
    high = v_size - 1;
  } else {
    high = (t_id + 1) * (v_size / ACTIVE_THREADS) - 1;
  }

  if (t_id == 0) {
    clock_gettime(CLOCK_MONOTONIC, &startTime);
  }

  // void (*operation)(int, int)
  operation(low, high, t_id, sense);

  if (t_id == 0) {
    clock_gettime(CLOCK_MONOTONIC, &endTime);
    elapsed_ns = (endTime.tv_sec - startTime.tv_sec) * 1000000000 + (endTime.tv_nsec - startTime.tv_nsec);
    // cout << elapsed_ns << endl;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////

void *buck_independent_separating(size_t t_id, int parse, void (*operation)(int, int)) {

  int low = t_id * (v_size / ACTIVE_THREADS);
  int high, mid;

  // Last thread if
  if (t_id == size_t(ACTIVE_THREADS - 1)) {
    high = v_size - 1;
  } else {
    high = (t_id + 1) * (v_size / ACTIVE_THREADS) - 1;
  }

  if (t_id == 0) {
    clock_gettime(CLOCK_MONOTONIC, &startTime);
  }

  // void (*operation)(int, int)
  operation(low, high);

  if (t_id == 0) {
    clock_gettime(CLOCK_MONOTONIC, &endTime);
    elapsed_ns = (endTime.tv_sec - startTime.tv_sec) * 1000000000 + (endTime.tv_nsec - startTime.tv_nsec);
    cout << elapsed_ns << endl;
  }
  return 0;
}

// -----------------------------------------------------------------------

void buck_sort(string algorithm = "tas", bool use_sense = false) {
  // Get input thred num
  v_size = v.size();
  if (NUM_THREADS >= v_size)
    NUM_THREADS = v_size - 1;
  ACTIVE_THREADS = NUM_THREADS;

  global_init();

  threads.resize(NUM_THREADS);

  size_t i;

  void (*al)(int, int);

  if (algorithm == "tas") {
    al = bucket_tas;
  } else if (algorithm == "ttas") {
    al = bucket_ttas;
  } else if (algorithm == "ticket") {
    al = bucket_ticket;
  } else if (algorithm == "mcs") {
    al = bucket_MCS;
  } else if (algorithm == "pthread") {
    al = bucket_pthread;
  } else if (algorithm == "pthread_bar") {
    al = buck_pthread_bar;
  }

  if (use_sense) {
    Sense_Barrier *sense = new Sense_Barrier(NUM_THREADS);
    // void* buck_independent_separating_sense(size_t t_id, int parse, Sense_Barrier* sense, void (*operation)(int, int, size_t, Sense_Barrier*)){
    for (i = 0; i < NUM_THREADS; ++i) {
      threads[i] = new thread(buck_independent_separating_sense, i, 0, sense, buck_sense);
    }
  } else {
    for (i = 0; i < NUM_THREADS; ++i) {
      threads[i] = new thread(buck_independent_separating, i, 0, al);
    }
  }

  // join_threads();
  for (i = 0; i < NUM_THREADS; i++) {
    threads[i]->join();
    delete threads[i];
  }

  v.clear();
  for (auto i : s)
    v.emplace_back(i);
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

    string ip_filename = "321.txt";
    string op_filename = "out.txt";
    string lock = "";
    string barrier = "";
    string num_threads = "4";
    int counter = 1;

    // mysort [--name] [-i source.txt] [-o out.txt] [-t NUM_THREADS] [--bar=<sense,pthread>] [--lock=<tas,ttas,ticket,mcs,pthread,petersonseq,petersonrel>]

    // ./mysort -i 321.txt -o out.txt -t 2 --lock=tas

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

      else if (arV[counter].substr(0, 3) == "--b") {
        if (lock != "") {
          cout << "Incorrect input";
          return 0;
        }
        barrier = arV[counter].substr(6, arV[counter].size());
        // cout << "In barrier" << endl;
      }

      else if (arV[counter].substr(0, 3) == "--l") {
        if (barrier != "") {
          cout << "Incorrect input";
          return 0;
        }
        lock = arV[counter].substr(7, arV[counter].size());
        // cout << "In lock" << endl;
      }

      counter++;
    }

    NUM_THREADS = size_t(stoi(num_threads));
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

    // lkbucket
    if (lock != "") {
      buck_sort(lock, false);
    } else {
      if (barrier == "sense")
        buck_sort("sense", true);
      else
        buck_sort("pthread_bar", false);
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