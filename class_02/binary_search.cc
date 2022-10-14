#include <iostream>
#include <vector>
using namespace std;

int binary_search(vector<int> data, int min, int max, int key){
  int step = 0;

  while (max >= min){
    step++;
    int mid = min + (max - min) / 2; //avoid max+min exceeding maximum value of int
    if (data[mid] == key)
      return step;
    else if (data[mid] > key)
      max = mid - 1;
    else if (data[mid] < key)
      min = mid + 1;
  }
    
}


int main(void){

  vector<int> sequence(10);
  int key;


  for (int i = 0; i< sequence.size(); ++i){
    sequence[i] = i + 1;
    cout << sequence[i];
  }
  cout << endl;
  
  cout << "key?";
  cin >> key;

  int min = 0, max = (int)sequence.size() - 1;

  printf("Found (Step = %d)\n", binary_search(sequence, min, max, key));

  return 0;

}