#pragma once

#include <vector>

#include "procedure.hh"
#include "rwlock.hh"
#include "common.hh"


enum class TransactionStatus : int {
  inFlight,
  committed,
  aborted,
};


class TxExecutor {
public:
  int thid_;
  TransactionStatus status_ = TransactionStatus::inFlight;
  std::vector<RWLock *> r_lock_list_;
  std::vector<RWLock *> w_lock_list_;
  std::vector<int> read_set_;
	std::vector<int> write_set_;
  std::vector <Procedure> pro_set_;

  TxExecutor(int thid) : thid_(thid) {
    read_set_.reserve(NUM_OPERATION);
    write_set_.reserve(NUM_OPERATION);
    pro_set_.reserve(NUM_OPERATION);
    r_lock_list_.reserve(NUM_OPERATION);
    w_lock_list_.reserve(NUM_OPERATION);
  }

  void begin();

  void read(int key);

  void write(int key);

  void unlock();

  void abort();

  void commit();

  Tuple *get_tuple(Tuple *table, uint64_t key) { return &table[key]; }
};