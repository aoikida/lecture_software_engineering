#include "bptree.hh"
#include "procedure.hh"
#include "random.hh"
#include "rwlock.hh"
#include "common.hh"
#include "record.hh" 
#include "transaction.hh"
#include <vector>
#include <unistd.h> 

Tuple *Table;

void
makeTransaction(std::vector <Procedure> &pro, Xoroshiro128Plus &rnd){
	pro.clear();
	for (int i = 0; i < NUM_OPERATION; ++i) {
    uint64_t record_index;
    // decide access destination key.
    record_index = rnd.next() % NUM_RECORD;

    // decide operation type.
    if ((rnd.next() % 100) < R_RATIO) {
      pro.emplace_back(Ope::READ, record_index);
    } else {
      pro.emplace_back(Ope::WRITE, record_index);
    }
  } 
}

void
worker(int thread_id) { 

	int i;
	TxExecutor tx(thread_id);
	Xoroshiro128Plus rnd;
	
	for(i = 0; i < NUM_TRANSACTION; ++i){
		rnd.init();
		makeTransaction(tx.pro_set_, rnd);
RETRY :
		//Execute transaction
		tx.begin();
		for (auto itr = tx.pro_set_.begin(); itr != tx.pro_set_.end(); ++itr) {

      if ((*itr).ope_ == Ope::READ) {
				tx.read((*itr).key_);
			} 
			else if ((*itr).ope_ == Ope::WRITE) {
        tx.write((*itr).key_);
      } 

			//Abort
			if (tx.status_ == TransactionStatus::aborted){
				tx.abort();
				goto RETRY;
			}
    }
		//Commit
		tx.commit();
	}
}

int
main(int argc, char *argv[])
{
  unsigned int sec;
  int nsec;
  double d_sec;
  struct timespec start_time, end_time;

	std::vector<std::thread> threads;

	//Make database
	posix_memalign((void **) &Table, 4096, NUM_RECORD * sizeof(Tuple));

	//Start measurement
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