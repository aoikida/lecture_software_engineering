#include "bptree.h"
#include "procedure.hh"
#include "random.hh"
#include "rwlock.hh"
#include "transaction.hh"

#include <vector>
#include <unistd.h>

void
worker(int thread_id) {

	int i;
	std::vector<DATA> transaction_set;
	std::vector<Procedure> transaction;
	Xoroshiro128Plus rnd;
	std::vector<int> read_set;
	std::vector<int> write_set;
	std::vector<RWLock *> r_lock_list;
  std::vector<RWLock *> w_lock_list;
	bool abort_flag = false;
	
	for(i = 0; i < NUM_TRANSACTION; ++i){
		rnd.init();
		makeTransaction(transaction, rnd);
RETRY :
		for (auto itr = transaction.begin(); itr != transaction.end(); ++itr) {

      if ((*itr).ope_ == Ope::READ) {
				readOperation((*itr).key_, r_lock_list, w_lock_list, read_set, write_set, abort_flag);
			} 
			else if ((*itr).ope_ == Ope::WRITE) {
        writeOperation((*itr).key_, r_lock_list, w_lock_list, read_set, write_set, abort_flag);
      } 

			if (abort_flag == true){ //abort
			unlock(read_set, write_set, r_lock_list, w_lock_list);
			abort_flag = false;
			goto RETRY;
			}
    }
		//commit
		unlock(read_set, write_set, r_lock_list, w_lock_list);
	}
}

int
main(int argc, char *argv[])
{
	
	unsigned int i;
  unsigned int sec;
  int nsec;
  double d_sec;
  struct timespec start_time, end_time;

	std::vector<std::thread> threads;

	//make database
	for(i = 0; i < NUM_RECORD; i++){
		(&record_set[i])->key = i;
		(&record_set[i])->val = 1;
		(&record_set[i])->lock.init();
		(&record_set[i])->next = &record_set[i+1];
	}

	//make index
	init_root();

	for(i = 0; i < NUM_RECORD; i++){
		insert((&record_set[i])->key, &record_set[i]);
	}

	//start measurement
	clock_gettime(CLOCK_REALTIME, &start_time);

	for (int i = 0; i < NUM_THREAD; ++i) {
    threads.emplace_back(worker, i);
  }

	for(auto& thread : threads){
    thread.join();
  }

	clock_gettime(CLOCK_REALTIME, &end_time);

	sec = end_time.tv_sec - start_time.tv_sec;
  nsec = end_time.tv_nsec - start_time.tv_nsec;

  d_sec = (double)sec + (double)nsec / (1000 * 1000 * 1000);
	cout << "--------------------" << endl;
	cout <<"Num Record: " << NUM_RECORD << endl;
	cout <<"Num Transaction: " << NUM_TRANSACTION << endl;
	cout <<"Workload: " << "Read"<< R_RATIO<< "% " << "Write" << 100 - R_RATIO << "%" << endl;
	cout <<"Num Operation: " << NUM_OPERATION << endl;
	cout <<"Num Thread: " << NUM_THREAD << endl;
  printf("Time: %f\n", d_sec);
	printf("Throughput: %f\n", (NUM_TRANSACTION * NUM_THREAD)/d_sec);
	cout << "--------------------" << endl;

  return 0;

}