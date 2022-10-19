#include <iostream>
#include <vector>
using namespace std;

#define NUM_ITEM 5


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

  //入力
  vector<int> input_datas(NUM_ITEM);

  //入力データを作成
  for (int i = 0; i < 5; i++){
    input_datas[i] = NUM_ITEM - i;
    cout << input_datas[i];
  }
  cout << endl;

  //クイックソート
  quick_sort(input_datas, 0, NUM_ITEM);

  //出力
  for(int i = 0; i<NUM_ITEM; i++){
    cout << input_datas[i];
  }
  cout << endl;

}