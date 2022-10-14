#include <iostream>
#include <vector>
using namespace std;

#define NUM_BUCKET 50
#define NUM_ITEM 10
#define DELTA 1

int array[NUM_BUCKET];

int data_to_hash(int data) {

    return data % NUM_BUCKET;
}

int rehash(int old_hash) {

    return (old_hash + DELTA) % NUM_BUCKET;
}


int add_to_table(int data, int *hash_table) {

    int i = data_to_hash(data);

    /* 空いているバケットを探す */
    while (hash_table[i] != 0) {
        /* 新たなハッシュ値を算出 */
        i = rehash(i);
    }

    /* ハッシュ値の位置のバケットにデータ格納 */
    hash_table[i] = data;

    return i;
}


int search_key(int data, int *hash_table) {

    int i;
    int step = 0;

    /* ハッシュ値の算出 */
    i = data_to_hash(data);

    while (hash_table[i] != 0 && step < NUM_BUCKET) {
        step++;
        if (hash_table[i] == data) {

            /* dataが見つかった */
            return step;
        }

        /* 新たなハッシュ値を算出 */
        i = rehash(i);

    }

    return 0;
}

int main(void) {

    int i, key;

    vector<int> input_datas(NUM_ITEM);
    int hash_table[NUM_BUCKET];

    //入力データを作成
    for (i = 0; i<NUM_ITEM; i++){
      input_datas[i] = i+1;
      cout << input_datas[i];
    }
    cout << endl;

    //ハッシュテーブルを作成
    for (i = 0; i<NUM_BUCKET; i++){
      hash_table[i] = 0;
    }

    /* テーブルにデータの格納 */
    for (i = 0; i < NUM_ITEM; i++) {
        add_to_table(input_datas[i], hash_table);
    }
    cout << "key?";
    cin >> key;

    /* データの探索 */
    int step = search_key(key, hash_table);
    if (step) {
        printf("FOUND (Step = %d) \n", step);
    }
    else {
        printf("not FOUND \n");
    }

    return 0;
}