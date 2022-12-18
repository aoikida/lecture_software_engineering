#include "bptree.h"
#include "procedure.hh"
#include "random.hh"
#include "rwlock.hh"

#include <vector>
#include <unistd.h>

#define NUM_RECORD 1000
#define NUM_THREAD 4
#define NUM_TRANSACTION 10000
#define NUM_OPERATION 16
#define R_RATIO 100

std::vector<DATA> record_set(NUM_RECORD);

void
makeTransaction(std::vector <Procedure> &transaction, Xoroshiro128Plus &rnd){
	transaction.clear();
	for (int i = 0; i < NUM_OPERATION; ++i) {
    uint64_t record_index;
    // decide access destination key.
    record_index = rnd.next() % NUM_RECORD;

    // decide operation type.プロシージャに代入している。
    if ((rnd.next() % 100) < R_RATIO) {
      transaction.emplace_back(Ope::READ, (&record_set[record_index])->key);
    } else {
      transaction.emplace_back(Ope::WRITE, (&record_set[record_index])->key);
    }
  }
}

void unlock(std::vector<int>& read_set, std::vector<int>& write_set, std::vector<RWLock *>& r_lock_list, std::vector<RWLock *>& w_lock_list) {
  for (auto itr = r_lock_list.begin(); itr != r_lock_list.end(); ++itr)
    (*itr)->r_unlock();

  for (auto itr = w_lock_list.begin(); itr != w_lock_list.end(); ++itr)
    (*itr)->w_unlock();

  /**
   * Clean-up local lock set.
   */
  r_lock_list.clear();
  w_lock_list.clear();

	read_set.clear();
	write_set.clear();
}


void readOperation(int key, std::vector<RWLock *>& r_lock_list, std::vector<RWLock *>& w_lock_list, std::vector<int>& read_set, std::vector<int>& write_set, bool &abortFlag){

	DATA * record;

	record = search_key(Root, key);

	//the key is already read or written by this transaction
	for (auto itr = read_set.begin(); itr != read_set.end(); ++itr){
		if(key == *itr)
			goto FINISH_READ;
	}

	for (auto itr = write_set.begin(); itr != write_set.end(); ++itr){
		if(key == *itr)
			goto FINISH_READ;
	}

	if (record->lock.r_trylock()) {
		read_set.emplace_back(key);
		r_lock_list.emplace_back(&record->lock);
	} else {
		abortFlag = true;
		goto FINISH_READ;
	}

	//read operation
FINISH_READ:

	return;

}


void writeOperation(int key, std::vector<RWLock *>& r_lock_list, std::vector<RWLock *>& w_lock_list, std::vector<int>& read_set, std::vector<int>& write_set, bool &abortFlag){

	DATA * record;

	record = search_key(Root, key);

	//the key is already written by this transaction
	for (auto itr = write_set.begin(); itr != write_set.end(); ++itr){
		if(key == *itr)
			goto FINISH_WRITE;
	}

	//the key is already read by this transaction, so need to upgrade lock
	for (auto itr = read_set.begin(); itr != read_set.end(); ++itr){
		if(key == *itr){
			if (!record->lock.tryupgrade()) {
        abortFlag = true;
        goto FINISH_WRITE;
      }
			for (auto lItr = r_lock_list.begin(); lItr != r_lock_list.end(); ++lItr) {
        if (*lItr == &record->lock) {
          write_set.emplace_back(key);
          w_lock_list.emplace_back(&record->lock);
          r_lock_list.erase(lItr);
          break;
        }
      }

      read_set.erase(itr);
      goto FINISH_WRITE;
		}
	}

	
	//the key is not read or written by this transaction.
	if (record->lock.w_trylock()) {
		write_set.emplace_back(key);
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
