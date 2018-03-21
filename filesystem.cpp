

#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstring>

using namespace std;

const int x = 5, y = 5, z = 5;
const int display_Width = 76;

struct table{
  char name[20];
  int table[y][x][z];
};

const int numberOfItems = 22;

struct config{
  bool blackList[numberOfItems];
  int priceBase[numberOfItems];
};

//returns the name of the item
bool blackList(int id){
  bool table[numberOfItems] = {1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0};
  return table[id];
}

//is base for price (will change to config file later)
int priceBase(int id){
  int table[numberOfItems] = {0,2,2,3,4,6,10,0,0,0,0,0,3,2,20,15,10,7,2,5,13,43};
  return table[id];
}

bool createConfig(){
  // Create our objects.
  fstream WriteStream;
  WriteStream.open ("file.cfg", ios::binary | ios::out); //attempt to open file
  if(!WriteStream.is_open()){ //check if empty
    return false;
  }
  else{
    config temp;
    cout << "Write" << endl;
    for(int i = 0; i < numberOfItems; i++){
      //for reference
      // bool blackList[numberOfItems];
      // int priceBase[numberOfItems];
      temp.blackList[i] = blackList(i); //lol
      temp.priceBase[i] = priceBase(i); //lol
      cout << i * sizeof(config) << " :" << temp.blackList[i] << "|" << temp.priceBase[i] << endl;
    }
    WriteStream.write(reinterpret_cast <char *> (&temp), sizeof(config));
    WriteStream.close();  //close file
    return true;
  }
}

//attempt at config file indexer
config getConfig(){
  // Create our objects.
  config temp;
  fstream readstream;
  readstream.open ("file.cfg", ios::binary | ios::in); //attempt to open file
  if(!readstream.is_open()){ //check if empty
    return temp;
  }
  else{
    readstream.read(reinterpret_cast <char *> (&temp), sizeof(config));
    readstream.close();  //close file
    cout << "Read" << endl;
    for(int i = 0; i < numberOfItems; i++){
      cout << i * sizeof(config) << " :" << temp.blackList[i] << "|" << temp.priceBase[i] << endl;
    }
    return temp;
  }
}

bool WriteTest(table dataTable){
  // Create our objects.
  fstream writeStream;
  writeStream.open ("file.bin", ios::binary | ios::out);
  writeStream.write(reinterpret_cast <char *> (&dataTable), sizeof(dataTable));
  writeStream.close();
  cout << "write sucsess" << endl;
  if(writeStream.fail()){
    //create file if there is no file
    cout << "write open failed" << endl;
    writeStream.close();
    return false;
  }
  return true;
}

table ReadTest(){
  table dataTable;
  // Create our objects.
  fstream filestream;
  //attempt to open file and then read first table
  filestream.open ("file.bin", ios::binary | ios::in);
  filestream.read(reinterpret_cast <char *> (&dataTable), sizeof(dataTable));
  if(filestream.fail()){
    //create file if there is no file
    cout << "read open failed" << endl;
    filestream.close();
    return dataTable;
  }
  filestream.close();
  cout << "read sucsess" << endl;
  return dataTable;
}

void displayDataTable(table dataTable){
  cout << " Name :" << dataTable.name << endl;
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
}

table generateData(){
  table dataTable;
  strncpy(dataTable.name, "bap", sizeof(dataTable.name));
  for(int i = 0; i < y; i++){
    for(int j = 0; j < x; j++){
      for(int k = 0; k < z; k++){
        dataTable.table[i][j][k] = rand() % 30;
      }
    }
  }
  return dataTable;
}

int main(){
  createConfig();
  getConfig();
  return 0;
}
