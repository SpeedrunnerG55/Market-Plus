#include <iostream>
#include <iomanip>
#include <cmath>

//for memset
#include <cstring>

const int x = 10, y = 2, z = 12;
const int display_Width = 76;

struct Hash{
  char tag[x * y * z];
};

struct data{
  int table[y][x][z];
};

data generateData();
void displayData(data dataTable);

Hash hashData(data dataTable);
void displayHash(Hash hashtag);

data unhashData(Hash hashTag);

void printLine(char weight);

using namespace std;

int main(){
  getchar();

  srand(time(NULL));

  data dataTable = generateData();
  displayData(dataTable);

  Hash hashtag = hashData(dataTable);
  // displayHash(hashtag);

  data dataTable2 = unhashData(hashtag);
  displayData(dataTable2);

  return 0;
}

void displayHash(Hash hashtag){
  for(int i = 0; i < x * y * z; i++){
    cout << hashtag.tag[i];
  }
  cout << endl << endl;
}

Hash hashData(data dataTable){
  Hash hashTag;
  int counter = 0;
  for(int i = 0; i < y; i++){
    for(int j = 0; j < x; j++){
      for(int k = 0; k < z; k++){
        hashTag.tag[counter] = (char)dataTable.table[i][j][k];
        counter++;
      }
    }
  }
  return hashTag;
}

data unhashData(Hash hashTag){
  data dataTable;
  int counter = 0;
  for(int i = 0; i < y; i++){
    for(int j = 0; j < x; j++){
      for(int k = 0; k < z; k++){
        dataTable.table[i][j][k] = (int)hashTag.tag[counter];
        counter++;
      }
    }
  }
  return dataTable;
}

data generateData(){
  data dataTable;
  for(int i = 0; i < y; i++){
    for(int j = 0; j < x; j++){
      for(int k = 0; k < z; k++){
        dataTable.table[i][j][k] = rand() % 30;
      }
    }
  }
  return dataTable;
}

void displayData(data dataTable){
  for(int k = 0; k < z; k++){
    cout << k << endl;
    for(int i = 0; i < y; i++){
      cout << "| ";
      for(int j = 0; j < x; j++){
        cout << setw(2) << dataTable.table[i][j][k];
        cout << ",";
      }
      cout << " |" << endl;
    }
    cout << endl;
  }
  printLine('-');
}

/* prints a chatacter string across the display */
void printLine(char weight){
  cout << "|";
  cout << string(display_Width + 2, weight);
  cout << "|" << endl;
}
