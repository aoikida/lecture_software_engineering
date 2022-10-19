#include <iostream>
#include <vector>
using namespace std;

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
        for (i = 0; i < size; ++i) cout << A[i] << endl;
    }

}

int main(){

    vector<int> array = {5, 4, 3, 2, 1};

    int N = array.size();

    //挿入ソート
    insertion_sort(array, N);

    //出力
    for (int i = 0; i < N; ++i) cout << array[i] << endl;
    

}