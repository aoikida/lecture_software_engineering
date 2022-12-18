## Explanation of the code

### simple_database

I implemented simple_database.

### Function

* search and update by using bptree

* execute transaction (read-only, read-write mixed) to database

* coccurrency control by using SS2PL

### Changeable element

* Number of threads

* Number of records in a database

* Number of transactions

* Workload of transaction

* Number of operations in a transaction

### Result

#### read-only transaction 1 thread vs 4 thread

* 1 thread : 52403 tx/s

* 4 thread : 112456 tx/s

#### read50%-write% transaction 1000 records vs 1M records with 16 threads

* 1000 records : 25086 tx/s

* 1M records : 64230 tx/s


