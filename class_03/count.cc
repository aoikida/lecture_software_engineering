#include <iostream>
#include <vector>
using namespace std;

#define NUM_ITEM 8
 
void count_sort(vector<int> &input, vector<int> &output, int count_size)
{   
    int i, j;

    int size = input.size();
    //要素を数えるための配列を作成
    vector<int> count_array(count_size + 1);


    //配列の初期化
    for(i = 0; i < count_size; ++i){
        count_array[i] = 0;
    }
    //配列に数字をカウントする。
    for(j = 1; j < size; ++j){
        count_array[input[j]] = count_array[input[j]] + 1;
    }

    //配列に累計数字をカウントする。
    for(i = 1; i < count_size + 1; ++i){
        count_array[i] += count_array[i - 1];
    }
 
    for (j = size - 1; j > 0; --j) {
        output[count_array[input[j]]] = input[j];
        count_array[input[j]] = count_array[input[j]] - 1;
    }

    return;
}
 
int main()
{   
    vector<int> input_datas = {0, 2, 5, 3, 0, 2, 3, 0, 3};

    vector<int> output(NUM_ITEM);
    
    //入力
    /*
    for(int i = 1; i<NUM_ITEM + 1; i++){
        input_datas[i] = NUM_ITEM + 1 - i;
        cout << input_datas[i];
    }
    cout << endl;
    */

    int max_value = *max_element(input_datas.begin(), input_datas.end());
    
    //カウントソート
    count_sort(input_datas, output, max_value);
    
    //出力
    for(int i = 1; i<NUM_ITEM + 1 ; i++){
        cout << output[i];
    }
    cout << endl;
 
    return 0;
}