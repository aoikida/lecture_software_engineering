#pragma once

#include <atomic>
#include <mutex>

#include "rwlock.hh"

using namespace std;

class Tuple {
public:
  RWLock lock;
  char val_[100];
};
