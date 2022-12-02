#include "lock.h"
uint Counter = 0;
std::atomic<uint> Mutex;

bool my_lock_core() {
  auto lock = Mutex.load(); //lockの状態を確認する
  if(lock == LockBit){ //他のスレッドによってロックされていたら、falseを返す。
    usleep(1);
    return false;
  }
  return Mutex.compare_exchange_weak(lock, LockBit);//ロックが取られていないときは、lockの値にLockBitの値を代入し、ロックを取得する。
}

void my_lock(void) {
  while (true) {
    bool ok = my_lock_core();
    if (ok == true) break;
  }
}

void my_unlock() { 
  Mutex.store(UnlockBit);  
}

void *worker(void *arg) {
  for (uint i = 0; i < NbLoop; i++) {
    my_lock();
    Counter++;
    my_unlock();    
  }
  return NULL;
}

int
main(void)
{ 

  unsigned int sec;
  int nsec;
  double d_sec;
  struct timespec start_time, end_time;
  clock_t start_clock, end_clock;
  pthread_t thread[NbThread];
  if ((pthread_mutex_init(&Lock, NULL)) == -1) ERR;

  /*時間の計測の開始時点*/
  clock_gettime(CLOCK_REALTIME, &start_time);
  start_clock = clock();

  for (uint i = 0; i < NbThread; i++) {
    pthread_create(&thread[i], NULL, worker, NULL);
  }
  for (uint i = 0; i < NbThread; i++) {
    pthread_join(thread[i], NULL);
  }

   /*時間の計測の終了時点*/
  clock_gettime(CLOCK_REALTIME, &end_time);
  end_clock = clock();

  printf("Counter: %u (Ref. %u)\n", Counter, NbThread * NbLoop);

  sec = end_time.tv_sec - start_time.tv_sec;
  nsec = end_time.tv_nsec - start_time.tv_nsec;

  d_sec = (double)sec
        + (double)nsec / (1000 * 1000 * 1000);

  printf("time:%f\n", d_sec);
  printf("clock:%f\n", (double)(end_clock - start_clock) / CLOCKS_PER_SEC);

  return 0;
}
