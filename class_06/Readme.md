## Explanation of the code

### counter_non_lock

Couter without lock, so we have lost-update problem.

we expect couter == 1000000, but by using this code, less than 1000000.

### counter_pthread_lock

Couter with pthread_mutex_lock, we have counter == 1000000, but this lock is pretty slow.
This is because pthread_mutex_lock is a centrized counter.

### counter_fetch_and_add

Couter with lock implemented by using "fetch and add" atomic operation. this is faster than pthread_mutex_lock.

### counter_ttas
Couter with lock implemented by using "compare and exchange" atomic operation. this is faster than pthread_mutex_lock.
