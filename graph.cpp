#include <iostream> //for console io
#include <math.h>

using namespace std;

void displayGraph(int length){

  srand(time(NULL));

  //limmit table to size of file
  //declare table object
  float valueTable[length];
  //build table of the last length market items
  for(int i = 0; i < length; i++){
    valueTable[i] = rand() % 1000;
  }

  //get its limits
  float value,base,cealing;
  //set limmits to the first value
  value = base = cealing = valueTable[0];
  //run though table getting the gighest and lowest value
  for(int i = 0; i < length; i++){
    value = valueTable[i];
    if(base > value){
      base = value;
    }
    if(cealing < value){
      cealing = value;
    }
  }
  cealing++;
  base--;
  getchar();

  cealing -= base;
  int size = 10;
  string grid[length];
  for(int i = 0; i < length; i++){
    valueTable[i] -= base;
    valueTable[i] /= cealing;
    string block;
    block.append(string(size - ceil(valueTable[i] * size ),' '));
    block.push_back('#');
    block.append(string(ceil(valueTable[i] * size ),'-'));
    block.append(string(2,'#'));
    grid[i] = block;
  }

  //turn grid sideways and dsiplay it character by character
  for(int j = 0; j < size + 3; j++){
    for(int i = 0; i < length; i++){
      cout << grid[i][j];
    }
    cout << endl;
  }
}


int main(){
  srand(time(NULL));

  displayGraph(80);

  return 0;
}
