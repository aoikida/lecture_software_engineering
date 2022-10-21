#include <iostream>
#include <vector>
#include <cstdlib>
#include <sys/time.h>
using namespace std;

#define NUM_DATA 100000

void insertion_sort(vector<int> &A, int size){
    int i, j, key;

    for (j = 1; j < size; ++j){
        key = A[j];
        i = j -1;
        while(i >= 0 && A[i] > key){
            A[i+1] = A[i];
            i = i-1;
        }
        A[i + 1] = key;
    }
}

int main(){

    int i;
    unsigned int sec;
    clock_t start_clock, end_clock;

    vector<int> array(NUM_DATA);

    for(i = 0; i<NUM_DATA; i++){
        array[i] = NUM_DATA - i;
    }

    start_clock = clock();

    //挿入ソート
    insertion_sort(array, NUM_DATA);

    end_clock = clock();

    //出力
    for (i = 0; i < NUM_DATA; ++i) {
        cout << array[i] << ",";
    }
    cout << endl;

    printf("clock:%f\n", (double)(end_clock - start_clock) / CLOCKS_PER_SEC);
    
}