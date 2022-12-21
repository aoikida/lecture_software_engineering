#include <vector>

#include "transaction.hh"
#include "procedure.hh"
#include "rwlock.hh"
#include "record.hh"
#include "common.hh"
#include "bptree.hh"

extern Bptree bptree;


void TxExecutor::begin() { 
  this->status_ = TransactionStatus::inFlight; 
}

void TxExecutor::read(int key){

  DATA * record = bptree.search_key(bptree.Root, key);

  //the key is already read or written by this transaction
  for (auto itr = read_set_.begin(); itr != read_set_.end(); ++itr){
		if(key == *itr)
      goto FINISH_READ;
  } 

  for (auto itr = write_set_.begin(); itr != write_set_.end(); ++itr){
		if(key == *itr)
      goto FINISH_READ;
  }

  if (record->lock.r_trylock()) {
    read_set_.emplace_back(key);
    r_lock_list_.emplace_back(&record->lock);
  } else {
    status_ = TransactionStatus::aborted;
    goto FINISH_READ;
  }

	//read operation
FINISH_READ:

  return;
}

void TxExecutor::write(int key){

  DATA * record = bptree.search_key(bptree.Root, key);

	//the key is already written by this transaction
  for (auto itr = write_set_.begin(); itr != write_set_.end(); ++itr){
		if(key == *itr)
      goto FINISH_WRITE;
  }

	//the key is already read by this transaction, so need to upgrade lock
  for (auto itr = read_set_.begin(); itr != read_set_.end(); ++itr){
		if(key == *itr){
      if (!record->lock.tryupgrade()) {
        status_ = TransactionStatus::aborted;
        goto FINISH_WRITE;
      }
      for (auto lItr = r_lock_list_.begin(); lItr != r_lock_list_.end(); ++lItr) {
        if (*lItr == &record->lock) {
          write_set_.emplace_back(key);
          w_lock_list_.emplace_back(&record->lock);
          r_lock_list_.erase(lItr);
          break;
        }
      }

      read_set_.erase(itr);
      goto FINISH_WRITE;
    }
  }

	
	  //the key is not read or written by this transaction.
  if (record->lock.w_trylock()) {
    write_set_.emplace_back(key);
    w_lock_list_.emplace_back(&record->lock);
  } 
  else {
    status_ = TransactionStatus::aborted;
    goto FINISH_WRITE;
  }

	//write operation
FINISH_WRITE:

  return;
}

void TxExecutor::unlock(){
  for (auto itr = r_lock_list_.begin(); itr != r_lock_list_.end(); ++itr)
    (*itr)->r_unlock();

  for (auto itr = w_lock_list_.begin(); itr != w_lock_list_.end(); ++itr)
    (*itr)->w_unlock();

  r_lock_list_.clear();
  w_lock_list_.clear();

  read_set_.clear();
  write_set_.clear();
}

void TxExecutor::abort(){
  unlock();

  read_set_.clear();
  write_set_.clear();
}

void TxExecutor::commit(){
  unlock();

  read_set_.clear();
  write_set_.clear();
}