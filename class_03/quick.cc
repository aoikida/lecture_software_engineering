#include <iostream>
#include <vector>
using namespace std;

#define NUM_ITEM 100000


void quick_sort(vector<int> &array, int left, int right){

    if (right - left <= 1) return; 

    int pivot_index = right - 1;

    int pivot = array[pivot_index];

    int i = left; //iはsecond_halfの最初の数

    for(int j = left; j < right - 1; ++j){
        if (array[j] < pivot){
            swap(array[i++], array[j]);
        }
    }

    swap(array[i], array[right - 1]);

    quick_sort(array, left, i);
    quick_sort(array, i+1, right);
}

int main(){
  int i;
  unsigned int sec;
  clock_t start_clock, end_clock;

  //入力
  vector<int> input_datas(NUM_ITEM);

  //入力データを作成
  for (int i = 0; i < NUM_ITEM; i++){
    input_datas[i] = NUM_ITEM - i;
  }

  start_clock = clock();

  //クイックソート
  quick_sort(input_datas, 0, NUM_ITEM);

  end_clock = clock();

  //出力
  for(int i = 0; i<NUM_ITEM; i++){
    cout << input_datas[i] << ", ";
  }
  cout << endl;

  printf("clock:%f\n", (double)(end_clock - start_clock) / CLOCKS_PER_SEC);

}