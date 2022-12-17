#include "bptree.h"
#include "procedure.hh"
#include "random.hh"
#include "rwlock.hh"

#include <vector>
#include <unistd.h>

#define NUM_RECORD 10
#define NUM_THREAD 2
#define NUM_TRANSACTION 1
#define NUM_OPERATION 16
#define R_RATIO 100

std::vector<DATA> record_set(NUM_RECORD);


void unlock(std::vector<RWLock *>& r_lock_list, std::vector<RWLock *>& w_lock_list) {
  for (auto itr = r_lock_list.begin(); itr != r_lock_list.end(); ++itr)
    (*itr)->r_unlock();

  for (auto itr = w_lock_list.begin(); itr != w_lock_list.end(); ++itr)
    (*itr)->w_unlock();

  /**
   * Clean-up local lock set.
   */
  r_lock_list.clear();
  w_lock_list.clear();
}

void readOperation(int key, std::vector<RWLock *>& r_lock_list, std::vector<RWLock *>& w_lock_list, bool &abortFlag){

	DATA * record;

	record = search_key(Root, key);

	if (record->lock.r_trylock()) {
		r_lock_list.emplace_back(&record->lock);
	} else {
		abortFlag = true;
		goto FINISH_READ;
	}

	//read operation
FINISH_READ:

	return;

}

void writeOperation(int key, std::vector<RWLock *>& r_lock_list, std::vector<RWLock *>& w_lock_list, bool &abortFlag){

	DATA * record;

	record = search_key(Root, key);

	if (record->lock.w_trylock()) {
		w_lock_list.emplace_back(&record->lock);
	} 
	else {
		abortFlag = true;
		goto FINISH_WRITE;
	}

	//write operation
FINISH_WRITE:

	return;
	
}

void
makeTransaction(std::vector <Procedure> &transaction, Xoroshiro128Plus &rnd){
	transaction.clear();
	for (int i = 0; i < NUM_OPERATION; ++i) {
    uint64_t record_key;
    // decide access destination key.
    record_key = rnd.next() % NUM_RECORD;

    // decide operation type.プロシージャに代入している。
    if ((rnd.next() % 100) < R_RATIO) {
      transaction.emplace_back(Ope::READ, record_key);
    } else {
      transaction.emplace_back(Ope::WRITE, record_key);
    }
  }
}


void
worker(int thread_id) {

	int i;
	std::vector<DATA> transaction_set;
	std::vector<Procedure> transaction;
	Xoroshiro128Plus rnd;
	std::vector<RWLock *> r_lock_list;
  std::vector<RWLock *> w_lock_list;
	bool abort_flag = false;
	
	for(i = 0; i < NUM_TRANSACTION; ++i){
		rnd.init();
		makeTransaction(transaction, rnd);
RETRY :
		for (auto itr = transaction.begin(); itr != transaction.end(); ++itr) {

      if ((*itr).ope_ == Ope::READ) {
				readOperation((*itr).key_, r_lock_list, w_lock_list, abort_flag);
			} 
			else if ((*itr).ope_ == Ope::WRITE) {
        writeOperation((*itr).key_, r_lock_list, w_lock_list, abort_flag);
      } 

			if (abort_flag == true){ //abort
			unlock(r_lock_list, w_lock_list);
			//cout << thread_id <<  "abort" << endl;
			abort_flag = false;
			sleep(1);
			goto RETRY;
			}
    }
		//commit
		unlock(r_lock_list, w_lock_list);
	}
	cout << thread_id <<  "end" << endl;
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

	init_root();

	//make database
	for(i = 0; i < NUM_RECORD; i++){
		(&record_set[i])->key = i;
		(&record_set[i])->val = 1;
		(&record_set[i])->lock.init();
		(&record_set[i])->next = &record_set[i+1];
	}

	printf("-----Insert-----\n");
	
	for(i = 0; i < NUM_RECORD; i++){
		insert((&record_set[i])->key, &record_set[i]);
	}

	//print_tree(Root);

	/*
	std::vector<std::thread> worker_threads;

	
    for (size_t i = 0; i < NUM_THREAD; ++i) {
      threads.push_back(worker);
    }

	for(size_t i = 0; i < NUM_THREAD; ++i) {
		threads[i].join();
	}
	*/

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

  printf("time:%f\n", d_sec);
	printf("throughput:%f\n", (NUM_TRANSACTION * NUM_THREAD)/d_sec);

	/*
	
	printf("-----Search-----\n");

	for(i = 0; i < DATA_NUMBER; i++){
		search_key(Root, (&record_set[i])->key);
	}

	printf("%d\n", count);

	

	printf("------UPDATE------\n");
	//before UPDATE
	for(i = 0; i < DATA_NUMBER; i++){
		cout << (&record_set[i])->val;
	}
	cout << endl; 
	//UPDATE
	for(i = 0; i < DATA_NUMBER; i++){
		update(Root, &record_set[i]);
	}
	//after UPDATE
	for(i = 0; i < DATA_NUMBER; i++){
		cout << (&record_set[i])->val;
	}
	cout << endl;

	*/

  return 0;

}