#include <iostream> //for console io
#include <fstream>  //for file io

//for delays
#include <chrono>
#include <thread>

struct player{
  int ID;
};

using namespace std;

/*  pauses execution for x milliseconds  */
void sleep(int delay){
  this_thread::sleep_for(chrono::milliseconds(delay));
}

string returnPlayerData(player playerData){
  string returnstring;
  returnstring.append(to_string(playerData.ID));
  return(returnstring);
}

void displayPlayes(int selection){
  // Create our objects.
  player temp;
  fstream readstream;
  int index = 0;
  readstream.open ("player.bin", ios::binary | ios::in);
  readstream.read(reinterpret_cast <char *> (&temp), sizeof(temp));
  cout << "<player log>" << endl;
  while(!readstream.fail()){
    if(selection == index){
      cout << "[";
    }
    else{
      cout << " ";
    }
    cout << "position " << index + 1 << " > " << returnPlayerData(temp);
    if(selection == index){
      cout << "]";
    }
    else{
      cout << " ";
    }
    cout << endl;
    //display
    index++; //increment index
    readstream.seekg(index * sizeof(temp), ios::beg); //GO TO NEXT PLAYER
    readstream.read(reinterpret_cast <char *> (&temp), sizeof(temp));
  }
  readstream.close();
}

//updates player information in the player database
bool savePlayer(player playerData){
  cout << "(saving player " << playerData.ID << ")..." << endl;
  fstream readstream;
  readstream.open ("player.bin", ios::binary | ios::in | ios::app); //attempt to open file, and in app mode to make it if its not there
  if(readstream.is_open()){ //if the file opened
    fstream countstream;
    int count = 0;
    countstream.open ("player.bin", ios::binary | ios::in);
    if(countstream.is_open()){
      countstream.seekg(0, ios::end); //set position to end
      count = countstream.tellg()/sizeof(player);
      //retuns number of players in file by getting
      //the index of the position and dividing it by the size of each player
      //no loops required :D
    }
    countstream.close();

    player playerTable[count];
    fstream readstream;
    readstream.open ("player.bin", ios::binary | ios::in);
    //build table and check table then modify if found
    bool found = false;
    for(int i = 0; i < count; i++){
      cout << "*read*" << i << endl;
      displayPlayes(i);
      readstream.seekg(i * sizeof(player), ios::beg); //set position to end
      readstream.read(reinterpret_cast <char *> (&playerTable[i]),sizeof(player));
      if(playerTable[i].ID == playerData.ID){
        found = true;
        playerTable[i] = playerData;
        cout << "*change*" << i << endl;
        displayPlayes(i);
      }
    }
    readstream.close();  //close file stream if its open no more reads

    if(!found){ //if player is not found the index will not be at the end of the file so move it forward one
      // Create our objects.
      fstream appendstream;
      appendstream.open ("player.bin", ios::binary | ios::out | ios::app); //attempt to open file
      if(appendstream.is_open()){
        appendstream.write(reinterpret_cast <char *> (&playerData), sizeof(player)); //write struct
        appendstream.close();  //close file stream if its open
        cout << "*write*" << count << endl;
        displayPlayes(count);
        if(!appendstream.fail()){
          return true;
        }
        else{
          return false;
        }
      }
    }
    else{
      // Create our objects.
      fstream writestream;
      writestream.open ("player.bin", ios::binary | ios::out); // << this line deletes everything in the file
      if(writestream.is_open()){
        for(int i = 0; i < count; i++){
          writestream.seekg(i * sizeof(player), ios::beg); //set position to index
          writestream.write(reinterpret_cast <char *> (&playerTable[i]),sizeof(player));
          cout << "*write*" << i << endl;
          displayPlayes(i);
        }
        writestream.close();
        if(!writestream.fail()){
          return true;
        }
        else{
          return false;
        }
      }
    }
  }
  return false;
}

//generates random and unique ID
int generateID(){
  player player;
  int index = 0;
  int ID = rand() % 1000 + 1; //reserve 0 for null player
  //CREATE OBJECTS AND READ FIRST PLAYER
  fstream readstream;
  readstream.open ("player.bin", ios::binary | ios::in);
  readstream.read(reinterpret_cast <char *> (&player), sizeof(player));
  do{
    index++; //increment index
    readstream.seekg(index * sizeof(player), ios::beg); //GO TO NEXT PLAYER
    readstream.read(reinterpret_cast <char *> (&player), sizeof(player)); //READ NEXT PLAYER
    if(player.ID == ID){ //CHECK PLAYER
      ID = generateID(); //hmmmm RECURSION TO THE RESCUE!
    }
  }while(!readstream.fail()); //while not at end of file
  readstream.close(); //close file
  //print results
  return ID;
}

//generates new player with unique id
player generatePlayer(){
  player playerData;
  playerData.ID = generateID();
  cout << "<created player " << playerData.ID << ">" << endl;
  savePlayer(playerData);
  return playerData;
}

int main(){
  srand(time(NULL));

  remove("player.bin");

  player player1 = generatePlayer();

  cout << endl << endl;
  player player2 = generatePlayer();

  cout << endl << endl;
  savePlayer(player1);

  cout << endl << endl;
  savePlayer(player2);

  cout << endl << endl;
  savePlayer(player2);

  return 0;
}
