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

#### read-only transaction 1 thread vs 4 thread vs 16 threads by using server

* 1 thread : 97337 tx/s

* 16 thread : 814175 tx/s

#### read-only transaction vs write-only transaction with 16 threads and 1000 records

* read-only : 68794 tx/s

* write-only : 47817 tx/s

#### read50%-write% transaction 1000 records vs 1M records with 16 threads

* 1000 records :  57391 tx/s

* 1M records : 67125 tx/s
