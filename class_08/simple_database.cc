#include "bptree.h"

#include <vector>
#include <random>


#define DATA_NUMBER 100000
#define NUM_THREAD 4
#define NUM_TRANSACTION 1000
#define NUM_OPERATION 16

std::vector<DATA> record_set(DATA_NUMBER);


void
read_only_worker(int thread_id) {
	
	for(int i = 0; i < NUM_TRANSACTION; ++i){
		std::vector<int> index_set;
		std::random_device rnd;    
		std::mt19937 mt(rnd());     
		std::uniform_int_distribution<> random(0, DATA_NUMBER-1);
		for (int k = 0; k < NUM_OPERATION; ++k){
			index_set.emplace_back(random(mt));
		}      
		RETRY :
		for(int j = 0; j < NUM_OPERATION; ++j){
			DATA * record = &record_set[index_set[j]];
			if (record->rwlock.r_trylock()) {
				if (search_key(Root, record->key) == 0) {
					//cout << record->key << "is found" << endl;
				}
				else {
					//cout << record->key << "is not found" << endl;
				}
			}
			else { 
				goto RETRY;
			}
    	}
		for(int j = 0; j < NUM_OPERATION; ++j){
			(&record_set[index_set[j]])->rwlock.r_unlock();
		}
	}

	cout  << "thread" << thread_id << " : finish read-only transaction" << endl;
}

int
main(int argc, char *argv[])
{
	
	unsigned int i;
    unsigned int sec;
    int nsec;
    double d_sec;
    struct timespec start_time, end_time;

	init_root();

	//make database
	for(i = 0; i < DATA_NUMBER; i++){
		(&record_set[i])->key = i+1;
		(&record_set[i])->val = 1;
		(&record_set[i])->rwlock.init();
		(&record_set[i])->next = &record_set[i+1];
	}

	printf("-----Insert-----\n");
	
	for(i = 0; i < DATA_NUMBER; i++){
		insert((&record_set[i])->key, &record_set[i]);
	}

	print_tree(Root);

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

	std::vector<std::thread> threads;

	for (size_t i = 0; i < NUM_THREAD; ++i) {
        threads.emplace_back(read_only_worker, i);
    }

	for(auto& thread : threads){
        thread.join();
    }

	clock_gettime(CLOCK_REALTIME, &end_time);

	sec = end_time.tv_sec - start_time.tv_sec;
    nsec = end_time.tv_nsec - start_time.tv_nsec;

    d_sec = (double)sec
        + (double)nsec / (1000 * 1000 * 1000);

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