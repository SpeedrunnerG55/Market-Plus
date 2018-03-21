/*
a simulation/representation of the algorithms involved in an enhanced market plugin concept for minecraft
@file    market.cpp
@author  Speed
*/

#include <iostream> //for console io
#include <fstream>  //for file io
#include <cstring>  //for memset
#include <vector>

//for delays
#include <chrono>
#include <thread>

#include <math.h> //for floor

/*!**********************************************************************
@file    main.cpp
@author  rwmc
@date    4/6/2016

@brief
Testing main for the console-input.h file. A reasonable reference for
using this application on a few different systems.

@copyright See LICENSE.md
************************************************************************/
#include "console-input.h" // KeyHit() and GetKey() and others

/*
uses elements from c++11
if std::to_string is not declared in scope
add -std=c++11 as a compiler option in g++ or gcc
*/

//custom input function
#define getInput(output,input) cout << "| " << output << " >"; cin>>input

const int numberOfItems = 22; //represents the quantity of items that exist
const int marketItems = 3; //represents the quantity of items that exist
const int IDofEmranlds = 21;
const int nameLength = 20;

struct item {
  float noise;
  float salePrice; //only store the sale price because the buy price is based on sell salePrice
  int stock;
  int quantityChange;
};

struct market{
  float ballance;
  item thing[numberOfItems];
};

const int x = 9, y = 3, z = 2;

struct InvHash{
  char InvTag[x * y * z];
  char HotTag[x * z];
};

struct Inventory{
  int item[y][x][z]; //Inventory [slot y][slot x][id,quantity]
  int hotbar[x][z]; //hotbar [slot x][id][quantity] (why did i include this? for continuity i guess)
};

//Hash FUNCTIONS
// these are probably most likley very mutch unneded but represent an extera step that might be needed to edit the player inventory

Inventory unhashInv (InvHash hashTag);
InvHash hashInv     (Inventory dataTable);

struct player{
  InvHash inventoryHash;
  int UUID;
  char name[nameLength];
  float cashBallance;   //amount of cash
  float creditBallance; //should be left out in implimentation (crafteconomy should store this)
  int quantityMined;
};

//main FUNCTIONS
//actual functions that would exist
void newDay(); //mid day each day
bool processItemTransaction (int UUID, int ID, int quantity);                   //for every time something is bought or sold for cash
bool printCash              (int UUID, int quantity);                           //print money for emralds
bool exchangeChash          (int UUID, float amount);                           //exchange cash for credit not the other way around
//edit sub group
void addToInventory (InvHash &inventoryHash, int ID, int &quantity);    // edits the player inventory to add or remove items from it
bool addToSlot      (int &slotID, int &slotAmt, int &quantity, int ID); // adds quantity of id to slot
void editSlot       (int &slotID, int &slotAmt, int &quantity, int ID); // overwrites quantity of id to slot
void editPrice      (float &price, float arg);                          // overwrite price for item
int retrievePlayerUUID();

bool makeAmo                (int UUID, int quantity); //exchange credit for amo (am unsure about this one)

// program functions
void functionTester();                // tests each actual functions seperatly
void Simulation();                    // simulates them how they would work together
void EditMarket(market &marketData);  // handles manually editing market data
void EditPlayer(player &playerData);  // handles manually editing player data
void displayMemory(player playerdata, market marketData); //displays a snapshot of the memory

//  Generate FUNCTIONS

// for market
market generateMarket();   //generates random market information
item generateItem(int ID); //generates random market information for specific item or random item if passed -1
item createItem(int id);   //generates blank market information for specific item

// for player
player generatePlayer(int UUID); //generates new player with givven UUID or random one if givver -1
int generateUUID();              //generates random UUID for simulation purposes
Inventory generateInventory();   //generates random inventory for simulation purposes

using namespace std; // i keep on forgetting string is in the std namespace

//  Display FUNCTIONS

// for market
void displayMarketData  (market marketData, int selY, int selX); //displays market information on every item with a selector
void displayMarketHeadder();                      //displays labels for each peice of information of the market data
string getItemData (item itemData, int current, int selY, int selX);   //Gets item data for item and returns it as a formatted string with a selector :o

// for Player
string returnPlayerData  (player playerData);  //returns basic player data for cash and creddit ballance and quantity minned as a string
string returnPlayerHeadder();                 //returns labels for each peice of information of the player data as a string
void displayInventory   (Inventory invData, int selY, int selX);
void displayHash        (InvHash hashtag); //probably wont ever get this running correctly

// displaying filesystem functions.....yeah i dont know where these go either
void displayMarketLog(int ID);        // -2 for just the ballance, -1 for full log, and some number for that item
void displayPlayerLog(int verbosity); //1 for basic info 2 for basic info and inventory

// reference FUNCTIONS (config? ¯\_(ツ)_/¯)

//probably wont need this one
string lookupItem(int id); //returns the name of the item (will change to config file later...mabe)
//or this
int stackLimmit  (int ID); //returns stack limmit for item (probably should be in a config)

struct config{
  bool blackList[numberOfItems];
  float priceBase[numberOfItems];
};

config getConfig(); //attempt at config file indexer
bool createConfig();      //creates template config

bool blackList(int id); //list of items illigal to buy or sell (will change to config file later)
int priceBase (int id); //is base for price (will change to config file later)

//  Filesystem FUNCTIONS

//for market
market retrieveMarketToday();           //retrieve market object from market log
bool updateMarketToday(market today);   //overwrites market today
bool logMarket        (market market);  //saves market object to market log and overwrites market today

// for player
player retrievePlayer(int UUID);  // retrieve player information out of player database and player ballance log
bool updatePlayer(player data);   // updates player information in player log
bool purgeMining();               // purges daily mining log
int getQuantityFromInv(Inventory inventoryData, int ID); //returns quantity of specific item in player inventory

/*  pauses execution for x milliseconds  */
void sleep(int delay){
  this_thread::sleep_for(chrono::milliseconds(delay));
}

//custom input function
#define getInput(output,input) cout << "| " << output << " >"; cin>>input

//custom graphical functions (this could be a class probably)
const int display_Width = 77;
void printMultiString(string strArray[],int sizeOfStrArray);
void printLong       (string longString);
void displayMenue    (string title, string discription, string options[],int sizeofOptions);
void printLine       (char weight);
void titleLine       (string str, char weight);
void LeftString      (string str);
void CenterString    (string str);
void RightString     (string str);
//formatting
string shorten(char name[nameLength]); //removes trailing whitespace
inline string build(string sA, float sB, string sC);
string String_2Decimals(string arg);
string bindCenter(string arg, int length);
string bindRight(string arg, int length);
string bindLeft(string arg, int length);
string select(string arg,int selY, int selX, int posY, int posX);

config configTable = getConfig();

int main(){
  srand(time(NULL));

  //menue settup
  string title = "Main Menue";
  string discription = "Market Plus, a simulation/representation of the algorithms involved in an enhanced market plugin concept for minecraft. By Speed";
  string options[]= {"1 Function-tester","Esc Quit"};

  //display menue all in one line :D
  displayMenue(title,discription,options,sizeof(options));

  bool running = true;
  do{
    if(KeyHit()){
      cout << string(100,'\n');
      char menue = GetChar();
      switch (menue) {

        //quit
        case KEY_ESCAPE: running = false; break;

        case '1': functionTester();  break;
        // case '2': Simulation();   break; //still putting this off
        case '9': running = false;   break;
        default:
        printLine('#');
        printLine(' ');
        CenterString("invalid input");
        string key;
        key.push_back(menue);
        CenterString(key); // << shows incorrect key pressed
      }
      //display menue all in one line :D
      displayMenue(title,discription,options,sizeof(options));
    }
  }while(running);
  CenterString("Bye!");
  printLine('#');
  return 0;
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
    for(int i = 0; i < numberOfItems; i++){
      //for reference
      // bool blackList[numberOfItems];
      // int priceBase[numberOfItems];
      temp.blackList[i] = blackList(i); //lol
      temp.priceBase[i] = priceBase(i); //lol
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
    createConfig();
    return getConfig();
  }
  else{
    readstream.read(reinterpret_cast <char *> (&temp), sizeof(config));
    readstream.close();  //close file
    for(int i = 0; i < numberOfItems; i++){
    }
    return temp;
  }
}

market createMarket(){
  market marketData;
  memset(marketData.thing,0,sizeof(marketData.thing));
  for(int i = 0; i < numberOfItems; i++){
    marketData.thing[i] = createItem(i);
  }
  marketData.ballance = 0;
  return marketData;
}

player createPlayer(){
  player playerData;
  memset(playerData.inventoryHash.HotTag,0,sizeof(playerData.inventoryHash.HotTag));
  memset(playerData.inventoryHash.InvTag,0,sizeof(playerData.inventoryHash.InvTag));
  playerData.UUID = 0;
  memset (playerData.name,' ',sizeof(playerData.name));
  playerData.cashBallance = 0;
  playerData.creditBallance = 0;
  playerData.quantityMined = 0;
  return playerData;
}

void displayMemory(market marketData,player playerData){
  printLine('=');
  CenterString("MEMORY");
  printLine('-');
  CenterString(returnPlayerHeadder());
  printLine('-');
  CenterString(returnPlayerData(playerData));
  displayInventory(unhashInv(playerData.inventoryHash),-1,-1);
  displayMarketData(marketData,-1,-1);
}

void functionTester(){
  // this is whats stored in memory, in a semi perminant basis. untill the program closes.
  // data retrieved will persist untill new data is read
  //SETTUP OBJECTS
  market marketData = createMarket();
  player playerData = createPlayer();



  //menue settup
  string title = "Function-tester";
  string discription = "test each function manually, and see their effects, generate functions generate data and stores it in memory, retrieve functions retrieve data stored on disc (database), update functions store and/or updates information stored on disc (database)";
  string options[]= {"1 Main group","2 generate group","3 Edit group","4 retrieve group","5 Display Log","6 Display group","7 Update group","Esc Quit"};

  //Group menue settup
  string GroupTitle;
  string groupDiscription;
  string groupOptions[4];

  //SubGroup settup
  string subtitle;
  string subdiscription;
  string subOptions[3];

  displayMemory(marketData,playerData);
  displayMenue(title,discription,options,sizeof(options)); //display menue all in one line :D

  int parameter;
  int UUID;
  int ID;
  int quantity;
  float amount;

  bool groupRunning = true;
  bool running = true;
  do{
    if(KeyHit()){
      cout << string(100,'\n');
      char menue = GetChar();
      menue = tolower(menue);
      memset(groupOptions,0,sizeof(groupOptions));
      memset(subOptions,0,sizeof(subOptions));
      switch (menue) { //giant switch of doom

        //quit
        case KEY_ESCAPE: running = false; break;

        // Main group

        case '1':
        GroupTitle = "Main group";
        groupDiscription = "main funcations that do big things.  Newday is what is normally called 12 noon each day while taxes are processed, processItemTransaction is whats called when a player buys or sells anything, printCash is called when a player exchanged emralds for cash, exchangeChash is called when a player exchanges cash for creddit";
        groupOptions[0] = "1 newDay()                ";
        groupOptions[1] = "2 processItemTransaction()";
        groupOptions[2] = "3 printCash()             ";
        groupOptions[3] = "4 exchangeChash()         ";
        groupRunning = true;
        displayMenue(GroupTitle,groupDiscription,groupOptions,sizeof(groupOptions));
        do{
          if(KeyHit()){
            groupRunning = false;
            cout << string(100,'\n');
            char menue = GetChar();
            menue = tolower(menue);
            switch (menue) {

              //quit
              case KEY_ESCAPE: running = false; break;

              case '1': newDay(); break;
              case '2':
              LeftString("processItemTransaction(UUID,ID,quantity)");
              UUID = retrievePlayerUUID();
              if(UUID == 0){
                CenterString("Null player detected... who did this? generate a player");
                break;
              }
              getInput("ID",ID);
              getInput("quantity",quantity);
              processItemTransaction(UUID,ID,quantity);
              break;

              case '3': ;
              LeftString("printCash(UUID,quantity)");
              UUID = retrievePlayerUUID();
              if(UUID == 0){
                CenterString("Null player detected... who did this? generate a player");
                break;
              }
              getInput("quantity",quantity);
              printCash(UUID,quantity);
              break;

              case '4': ;
              LeftString("exchangeChash(UUID,amount)");
              UUID = retrievePlayerUUID();
              if(UUID == 0){
                CenterString("Null player detected... who did this? generate a player");
                break;
              }
              getInput("Amount",amount);
              exchangeChash(UUID,amount);
              break;
            }
          }
        }while(groupRunning);
        break;

        // Generate group

        case '2':
        GroupTitle = "Generate group";
        groupDiscription = "generate Functions that generate data for you so you dont waste time creating random data :), but if you want you can use the create functions to create empty objects";
        groupOptions[0] = "1 generatePlayer() ";
        groupOptions[1] = "2 generateMarket() ";
        groupOptions[2] = "3 createPlayer()   ";
        groupOptions[3] = "4 createMarket()   ";
        groupRunning = true;
        displayMenue(GroupTitle,groupDiscription,groupOptions,sizeof(groupOptions));
        do{
          if(KeyHit()){
            groupRunning = false;
            cout << string(100,'\n');
            char menue = GetChar();
            menue = tolower(menue);
            switch (menue) {
              case '1':
              subtitle = "generatePlayer()";
              subdiscription = "enter parameter for generatePlayer()";
              subOptions[0] = "-1 for new player            ";
              subOptions[1] = "some number for specific UUID";
              displayMenue(subtitle,subdiscription,subOptions,sizeof(subOptions));
              getInput(">",parameter);
              playerData = generatePlayer(parameter);
              break;
              case '2': marketData = generateMarket(); break;
              case '3': playerData = createPlayer();   break;
              case '4': marketData = createMarket();   break;
            }
          }
        }while(groupRunning);
        break;

        // Edit groupOptions

        case '3':
        GroupTitle = "Edit group";
        groupDiscription = "Functions that edit data in memory";
        groupOptions[0] = "1 editPlayer() ";
        groupOptions[1] = "2 EditMarket() ";
        groupRunning = true;
        displayMenue(GroupTitle,groupDiscription,groupOptions,sizeof(groupOptions));
        do{
          if(KeyHit()){
            groupRunning = false;
            cout << string(100,'\n');
            char menue = GetChar();
            menue = tolower(menue);
            switch (menue) {
              case '1': EditPlayer(playerData);
              case '2': EditMarket(marketData);
            }
          }
        }while(groupRunning);
        break;

        // Retrieve group

        case '4':
        GroupTitle = "Retrieve group";
        groupDiscription = "Functions that retrieve data from storage into memory";
        groupOptions[0] = "1 retrievePlayer()  ";
        groupOptions[1] = "2 retrieveMarketToday()     ";
        groupRunning = true;
        displayMenue(GroupTitle,groupDiscription,groupOptions,sizeof(groupOptions));
        do{
          if(KeyHit()){
            groupRunning = false;
            cout << string(100,'\n');
            char menue = GetChar();
            menue = tolower(menue);
            switch (menue) {
              case '1': retrievePlayer(retrievePlayerUUID()); break;
              case '2': marketData = retrieveMarketToday(); break;
            }
          }
        }while(groupRunning);
        break;

        // Display Log group

        case '5':
        GroupTitle = "Display log group";
        groupDiscription = "Functions that display the data in memory";
        groupOptions[0] = "1 displayMarketLog()";
        groupOptions[1] = "2 displayPlayerLog()";
        groupRunning = true;
        displayMenue(GroupTitle,groupDiscription,groupOptions,sizeof(groupOptions));
        do{
          if(KeyHit()){
            groupRunning = false;
            cout << string(100,'\n');
            char menue = GetChar();
            menue = tolower(menue);
            switch (menue) {
              case '1':
              subtitle = "displayMarketLog()";
              subdiscription = "enter parameter for displayMarketLog()";
              subOptions[0] = "enter -2 to display full market logs         ";
              subOptions[1] = "enter -1 to display just the market ballence ";
              subOptions[2] = "some ID for specific item                    ";
              displayMenue(subtitle,subdiscription,subOptions,sizeof(subOptions));
              getInput(">",parameter); //i should write in som input verifacation here... mabe later
              displayMarketLog(parameter); break;
              case '2':
              subtitle = "displayPlayerLog()";
              subdiscription = "enter parameter for displayMarketData()";
              subOptions[0] = "enter 1 to display basic info               ";
              subOptions[1] = "enter 2 to display basic info and inventory ";
              displayMenue(subtitle,subdiscription,subOptions,sizeof(subOptions));
              getInput(">",parameter); //i should write in som input verifacation here... mabe later
              displayPlayerLog(parameter); break;
            }
          }
        }while(groupRunning);
        break;

        // Display group
        case '6':
        GroupTitle = "Display group";
        groupDiscription = "Functions that display the data in memory";
        groupOptions[0] = "1 displayMarketData()";
        groupOptions[1] = "2 returnPlayerData() ";
        groupOptions[2] = "3 displayInventory() ";
        groupOptions[3] = "4 displayHash()      ";
        groupRunning = true;
        displayMenue(GroupTitle,groupDiscription,groupOptions,sizeof(groupOptions));
        do{
          if(KeyHit()){
            groupRunning = false;
            cout << string(100,'\n');
            char menue = GetChar();
            menue = tolower(menue);
            switch (menue) {
              case '1': displayMarketData(marketData,-1,-1); break;
              case '2': CenterString(returnPlayerData(playerData)); break;
              case '3': displayInventory(unhashInv(playerData.inventoryHash),-1,-1); break;
              case '4': displayHash(playerData.inventoryHash); break;
            }
          }
        }while(groupRunning);
        break;

        // Update group

        case '7':
        GroupTitle = "Update group";
        groupDiscription = "Functions that generate data for you so you dont waste time creating random data :)";
        groupOptions[0] = "1 updatePlayer()     ";
        groupOptions[1] = "2 updateMarketToday()";
        groupRunning = true;
        displayMenue(GroupTitle,groupDiscription,groupOptions,sizeof(groupOptions));
        do{
          if(KeyHit()){
            groupRunning = false;
            cout << string(100,'\n');
            char menue = GetChar();
            menue = tolower(menue);
            switch (menue) {
              case '1': updatePlayer(playerData);      break;
              case '2':
              subtitle = "updateMarketToday()";
              subdiscription = "enter parameter for updateMarketToday()";
              subOptions[0] = "enter -1 to overwrite entire market     ";
              subOptions[1] = "some ID for specific item               ";
              displayMenue(subtitle,subdiscription,subOptions,sizeof(subOptions));
              getInput(">",parameter); //i should write in som input verifacation here... mabe later
              updateMarketToday(marketData);
              break;
            }
          }
        }while(groupRunning);
        break;

        default:
        printLine('#');
        printLine(' ');
        CenterString("invalid input");
        string key;
        key.push_back(menue);
        CenterString(key); // << shows incorrect key pressed
      }
      displayMemory(marketData,playerData);
      displayMenue(title,discription,options,sizeof(options)); //display menue all in one line :D
    }
  }while(running);
  cout << string(100,'\n');
}


//text gui for a selection based player retriever
int retrievePlayerUUID(){
  // Create our objects.
  fstream countStream;

  //get count
  int count = 0;
  countStream.open ("player.bin", ios::binary | ios::in);
  if(countStream.is_open()){
    countStream.seekg(0, ios::end);
    count = countStream.tellg()/sizeof(player); //retuns number of players in file
  }
  countStream.close();

  if(count == 0){
    return 0;
  }

  player playerTable[count];

  // Create our objects.
  player temp;
  fstream readStream;
  int index = 0;
  readStream.open ("player.bin", ios::binary | ios::in);
  readStream.read(reinterpret_cast <char *> (&temp), sizeof(temp));
  while(index < count){ //i alreaddy know how manny players there are, Just build the table
    playerTable[index] = temp;
    //display
    index++; //increment index
    readStream.seekg(index * sizeof(temp), ios::beg); //GO TO NEXT PLAYER
    readStream.read(reinterpret_cast <char *> (&temp), sizeof(temp));
  }
  readStream.close();

  bool running = true; // boolian that tells
  int selY = 1; //position of selector

  //menue settup
  string title = "Player Selector";
  string discription = "select the player you want to retrieve into memory, move the selection up and down with W and S";
  string block;
  //dynamic update
  block = "Return to fetch (";
  block.append(to_string(playerTable[selY].UUID));
  block.append(") to tester");
  string options[]= {block,"Esp Quit"};
  displayMenue(title,discription,options,sizeof(options)); //display menue all in one line :D

  //display the table
  printLine('=');
  LeftString(returnPlayerHeadder());
  printLine('-');
  for(int i = 0; i < count; i++){
    LeftString(select(returnPlayerData(playerTable[i]),i,-1,selY,-1));
  }

  do{
    if(KeyHit()){
      cout << string(40,'\n');
      char menue = GetChar();
      menue = tolower(menue);
      switch (menue) {

        //quit
        case KEY_ESCAPE: running = false; break;

        case KEY_RETURN: //if user enters to return player in selection
        // playerData = playerTable[selY]; //take the player from the table, index the selection and copy its data to the reference value. changing it to what the player the user selected
        //and you thought i was actually going to take the UUID from the playerdata i alreaddy read from disc and then use it to index a return value for the function to return the same playerdata again
        // common who do you think i am! "playerData = retrievePlayer(playerTable[selY].UUID)" pffft... althought this would have worked this is verry redundant
        // i could have returned an in here, passing the UUID to the parrent function where it can run the retrieve player function but... thats also redundant, i really do think this is the most eficient way to do it
        //i alreaddy have the player data, just return it
        return playerTable[selY].UUID; // but then again i want to use this as a look up table for UUID's and i think it should actually return the uuid even tho it migh be a bit redundant

        case 'w':
        selY--;//chage selection
        //validate selection
        selY %= count;
        if(selY < 0){
          selY = count - 1;
        }
        break;

        case 's':
        selY++;//chage selection
        //validate selection
        selY %= count;
        break;

        default:
        printLine('#');
        printLine(' ');
        CenterString("invalid input");
        string key;
        key.push_back(menue);
        CenterString(key); // << shows incorrect key pressed
      }

      // since options is now dynamic, im going to have to update it every time before i display it
      block = "Return to fetch (";
      block.append(to_string(playerTable[selY].UUID));
      block.append(") to tester");
      string options[]= {block,"Esc Quit"};
      displayMenue(title,discription,options,sizeof(options)); //display menue all in one line :D

      //display the table
      printLine('=');
      LeftString(returnPlayerHeadder());
      printLine('-');
      for(int i = 0; i < count; i++){
        LeftString(select(returnPlayerData(playerTable[i]),i,-1,selY,-1));
      }
    }
  }while(running);
  return 0;
}

// oh boy is this going to be fun :D
// handles manually editing player data
void EditPlayer(player &playerData){
  //menue settup
  string title = "Edit player";
  string discription = "manually edit the ballance or contents of the players inventory, give or take away item or money at your discression move around with WASD";
  string options[]= {
    "1 add to slot","2 increment slot","3 decrement slot","4 edit slot","5 purge slot",
    "6 edit cash","7 edit credit","8 Save and Quit","Esc Just Quit"
  };

  player tempData = playerData; //save a copy to edit
  // unpack once to reduce the amount or unpacking and repacking i would have to do otherwise...
  //also reduce the amount of mental gymnastics needed to understand and follow what im doing here
  Inventory inventoryData = unhashInv(tempData.inventoryHash);

  int selX = 0,selY = 0; //selector

  CenterString(returnPlayerData(playerData));
  displayInventory(inventoryData,selY,selX); //display menue all in one line :D this is what the selector was for, all this time
  displayMenue(title,discription,options,sizeof(options));

  bool running = true; // boolian that tells
  int quantity = 0, ID = 0, selectionID = 0, selectionAMT = 0;
  float amount = 0;
  //for reference ... i do this a lot
  //int item[y][x][z]; //Inventory [slot y][slot x][id,quantity]
  //int hotbar[x][z]; //hotbar [slot x][id][quantity] (why did i include this? for continuity i guess)

  do{
    if(KeyHit()){
      //depending on where the selection the selection ID and quantity need to be based on the right data set
      if(y > -1){
        selectionAMT = inventoryData.item[y][x][1];
        selectionID = inventoryData.item[y][x][0];
      }
      else{
        selectionAMT = inventoryData.hotbar[x][0];
        selectionID = inventoryData.hotbar[x][0];
      }
      cout << string(100,'\n');
      char menue = GetChar();
      // this, this right here is cancer.
      // im using the selector to select what item to affect but the affect wont nesisarally (most likley wont happen in that slot)
      // for the affect to be bounded by the slot i would have to send the selection information to the edit inventory function and then
      // forgoing the forloops and processing the slot coordinates given to it... now that i say it that way it dosnet seam all that bad,
      // I could pas -1,-1 for it to use any slot as before and pass it a slot coordinate if i want it to attemtp to edit a specific slot
      // i am also givving the user full controll ofi the edit inventory function.. luckly the functi- wait why am i even using the edit inventory
      // function when i could be using the process slot function, it usses the unhashed data, i could unpack the slot at the selection, edit it, then repack it into
      // the playerdata for it to be updated at the end. now where talking :3
      // I really need to stop writting these long winded --dissertations-- rambelings in the comments... not like annyones reading
      switch (menue) {
        //inventory section
        case '1':
        case '2':
        case '3':
        //all three cases will descide the ID the same way
        if(selectionID == 0){
          printLong("Enter the ID add to the slot");
          getInput(">",ID);
        }
        else ID = selectionID; //no if needed here because the selection slot ID was alreaddy determined :D
        case '4': //do nothing here just enter the next swich and put it in the end
        switch (menue) { //switchseption to cover each different quantity after for each case
          case '1':
          printLong("Enter the quantity to add to the slot, positive or negative");
          getInput(">",quantity);
          break;
          case '2': quantity = 1;  break;
          case '3': quantity = -1; break;
          case '4':
          ID = selectionID; //always the selectionID
          quantity = -selectionAMT; // negitiv the quantity the slot contains so it becomes 0
        }

        //for reference
        //inline void addToSlot(int slotID, int slotAmt, float quantity, int ID)
        //in any case <.< i want the same action to take place with these same variables so unless the user exits, this would happen every time
        addToSlot(selectionID,selectionAMT,quantity,ID); //passing both ID and selectionID because the selection might be air, and then the id
        quantity = 0; //reset it to 0 reguardless of the outcome, since if it did not go into the inventory there is nowhere else for it to go

        break; // finally break out of the inventory edit section

        //for reference
        // void editSlot(int &slotID, int &slotAmt, int &quantity, int ID)
        case '5':
        printLong("Enter the ID for slot");
        getInput(">",ID);
        printLong("Enter the quantity to add to the slot, positive or negative");
        getInput(">",quantity);
        editSlot(selectionID,selectionAMT,quantity,ID);

        //money section
        case '6':
        printLong("Enter new value for cash");
        getInput(">",amount);
        tempData.cashBallance = amount;
        break;
        case '7':
        printLong("Enter new value for credit");
        getInput(">",amount);
        tempData.creditBallance = amount;
        break;

        case '8': running = false; break;  //exit the switch and the do loop, repack, save then exit the function
        case '9': running = false; return; //just exit the entire function without making any changes
        default:
        printLine('#');
        printLine(' ');
        CenterString("invalid input");
        string key;
        key.push_back(menue);
        CenterString(key); // << shows incorrect key pressed
      }

      // amagin if i nneded to repack here, oh that would have been awefull
      if(y > -1){
        inventoryData.item[y][x][1] = selectionAMT;
        inventoryData.item[y][x][0] = selectionID;
      }
      else{
        inventoryData.hotbar[x][0] = selectionAMT;
        inventoryData.hotbar[x][0] = selectionID;
      }

      CenterString(returnPlayerData(tempData)); //display
      displayInventory(inventoryData,selY,selX); //im sure glad i figured out i could unpack it once....
      displayMenue(title,discription,options,sizeof(options));
    }
  }while(running);

  //repack it at the end so i only need to do this once
  tempData.inventoryHash = hashInv(inventoryData); //overwrite new inventory
  playerData = tempData; //overwrite with new data
  updatePlayer(playerData);
  //only do this at the end to reduce read/write cycles on disc storage.
  //its better to keep this all in memory... mabe i should also make a way to exit without updating, sorta like exiting without saving x3
}

// handles manually editing market data
void EditMarket(market &marketData){

  market tempMarketData = marketData;

  //menue settup
  string title = "Edit market";
  string discription = "manually edit the ballance, transaction totals, or prices at your discression move selection around with WASD";
  string options[]= {
    "1 edit ballance","Enter to edit selection","Eec Just Quit"
  };

  bool running = true; // boolian that tells
  int selY = 1, selX = 0; //position of selector
  float amount = 0;
  int quantity = 0;

  displayMarketData(tempMarketData,selY,selX);
  displayMenue(title,discription,options,sizeof(options));

  do{
    if(KeyHit()){
      char menue = GetChar();
      menue = tolower(menue);
      switch (menue) {
        case 'w':
        selY--;//chage position
        //validate position
        selY %= numberOfItems;
        if(selY < 0){
          selY = numberOfItems - 1;
        }
        //make sure item is not black listed in validated position
        while(configTable.blackList[selY]){
          //constantly change and validate new position untill item is not black listed
          selY--;
          selY %= numberOfItems;
          if(selY < 0){
            selY = numberOfItems - 1;
          }
        }
        break;

        case 'a':
        selX--;
        selX %= 6;
        if(selX < 0){
          selX = marketItems - 1;
        }
        break;

        case 's':
        selY++;
        selY %= numberOfItems;
        while(configTable.blackList[selY]){
          selY++;
          selY %= numberOfItems;
        }
        break;

        case 'd':
        selX++;
        selX %= marketItems;
        break;

        case '1':
        do{
          cin.clear();
          printLong("Enter new value for ballance");
          getInput(">",amount);
          tempMarketData.ballance = amount;
        }while(amount < 0);

        break;
        case KEY_RETURN:
        switch(selX){
          case 0:
          do{
            cin.clear();
            printLong("Enter price for item in market");
            getInput(">",amount);
          }while(amount < 0);
          editPrice(tempMarketData.thing[selY].salePrice,amount);
          break;
          case 1:
          do{
            cin.clear();
            printLong("Enter stock for item");
            getInput(">",quantity);
          }while(quantity < 0);
          tempMarketData.thing[selY].stock = quantity;
          break;
          case 2:
          printLong("Enter quantity change for item in market");
          getInput(">",quantity);
          tempMarketData.thing[selY].quantityChange = quantity;
          break;
        }
        break;
        case '8': running = false; break;  //exit the switch and the do loop, repack, save then exit the function
        case '9': running = false; return; //just exit the entire function without making any changes
        default:
        printLine('#');
        printLine(' ');
        CenterString("invalid input");
        string key;
        key.push_back(menue);
        CenterString(key); // << shows incorrect key pressed
      }
      cout << string(40,'\n');
      displayMarketData(tempMarketData,selY,selX);
      displayMenue(title,discription,options,sizeof(options));
    }
  }while(running);
  //overwite with new data
  marketData = tempMarketData;
}


// void Simulation(){
//
//   market marketData = generateMarket();
//   updateMarketToday(marketData);
//   player playerBase[10];
//   for(int i = 0; i < 10; i++){
//     playerBase[i] = generatePlayer(-1);
//   }
//   bool running = true;
//   do{
//
//
//
//
//     CenterString("hit enter to advance to next day");
//     CenterString("hold enter if you want to fast forward");
//     getchar();
//     newDay();
//   }while(running);
// }



//main FUNCTIONS actual functions that would exist
void newDay(){
  market newMarket = retrieveMarketToday();
  for(int i = 0; i < numberOfItems; i++){
    if(!configTable.blackList[i]){
      int change = newMarket.thing[i].quantityChange;
      float noise;
      if(change != 0)
      noise = (((rand() % 100) * .01) - 0.5) * 0.05;
      float salePrice = newMarket.thing[i].salePrice;
      if(change < 0)
      salePrice += salePrice * (.10 + (noise)); //increase average 10%
      else if(change > 0)
      salePrice -= salePrice * (.10 + (noise)); //decrease average 10%
      else
      CenterString("price remained the same");
      if(change != 0){
        newMarket.thing[i].noise = noise;
        newMarket.thing[i].quantityChange = change;
        newMarket.thing[i].salePrice = salePrice;
      }
    }
    else{
      CenterString("blacklisted item!");
    }
  }
  purgeMining(); //purge player exchange limmit
  updateMarketToday(newMarket); //overwrites entre entery
  logMarket(newMarket);
}

//for every time something is bought or sold for cash
bool processItemTransaction(int UUID, int ID, int quantity){
  market MarketToday = retrieveMarketToday();
  player playerData = retrievePlayer(UUID);
  //if market is buying
  float price = 0;
  float sourceBallance = 0;
  int sourceItems = 0;
  bool buying;
  if(quantity > 0){ //if buying
    // quantity is positive
    // price is negitive ... because a positive times a negitive gives you negitive!
    buying = true;
    sourceItems = getQuantityFromInv(unhashInv(playerData.inventoryHash),ID);
    sourceBallance = MarketToday.ballance;
    float salePrice = MarketToday.thing[ID].salePrice;
    price = -(salePrice - (salePrice * .10)); //buy price is 10% less than sale price
  }
  else if(quantity < 0){ //if selling
    // quantity is negitive
    // price is negitive ... because a negitive times a negitive give you positive!
    buying = false;
    sourceItems = MarketToday.thing[ID].stock;
    sourceBallance = MarketToday.ballance;
    price = -MarketToday.thing[ID].salePrice;
  }
  if(sourceItems < abs(quantity)){
    CenterString("not enough itmes");
    CenterString("processing all remaining items");
    quantity = sourceItems;
  }
  if(sourceBallance < price * abs(quantity)){
    CenterString("not enough Cash");
    CenterString("processing what can be afforded");
    quantity = floor(sourceBallance / price);
  }
  if(quantity == 0){ //if quantity reaches 0 Transaction iss nullified and failed technically i dont have to return but i do to increace effiecency
    CenterString("Transaction Failed returning");
    return false;
  }

  if(!buying){ //flip sign flip sign to negitive even if its negitive alreaddy
    quantity = -abs(quantity);
  }

  //works for both selling and buying

  //market values
  MarketToday.ballance += quantity * price; //if selling this goes up, if buying this goes down
  MarketToday.thing[ID].stock += quantity;  //if selling this goes down, if buying this goes up

  //player value
  playerData.cashBallance -= quantity * price;  //if selling this goes down, if buying this goes up
  quantity = -quantity; // the sign is reverced from the markets perspective to correct the effect it should have on the player
  addToInventory(playerData.inventoryHash,ID,quantity); //if selling this goes up, if buying this goes down
  // ** what if player can not hold what he buys **
  //if by now, quantity is not 0, that means there is left over from when the edit inventory (quantity)
  //not being able to fill in compleatly all of the items the player tried to purchase.
  // so the quantity now being negitive needs to be reprocessed through
  // the transaction equations to reverse the values back to their origical state to match the diffference
  // between the intended inventory of the player and what the inventory of the player became... wow that was a mouthfull
  if(quantity != 0){ // basically just.. do these again and everything should be fine (i could have just swapped the signs here but then i would have also had to swap the sign of quantity again, and that quantity of sign swapping would get me verry dizzy)
    //market values
    MarketToday.ballance += quantity * price; //if selling this now goes down, if buying this now goes up
    MarketToday.thing[ID].stock += quantity;  //if selling this now goes up, if buying this now goes down
    //player value
    playerData.cashBallance -= quantity * price;  //if selling this now goes up, if buying this now goes down
  }

  //update player log with new player data, update the market with new market data
  MarketToday.thing[ID].quantityChange += quantity; //yep thats it, the ID is encoded in the index, and the quantity is the value.
  // this is just added to the total, and weather of not it is positive or negitive would tetermin if the market is a bull or a bear...either way better get the guns-
  if((updatePlayer(playerData)) && (updateMarketToday(MarketToday))){
    return true;
  }
  CenterString("ERROR! : something went wrong look up^..."); //specific error messages should have been printed above
  //insert something here that can revert it back to its previous state somehow mabe save a temporary copy of everything and put it here
  CenterString("market is corrupted :'c"); //specific error messages should have been printed above
  return false;
  //  **MESSAGE ABOUT PONENTIAL FLAW**
  //returns weather the transaction was sucsessfull or not. although if it is not sucsessfull
  //here, i have a much greater issue that just the player or the market not having the right values.
  //if anny of these functions fails to update the information in the file that means there potential incontinuity
  //whithin the market/player data. leading to a potential vector of an explit to gain free money or material, or loose it with no cause...
  //this really should return more than just false, mabe some sort of exeption or something that would automatically disable the plugin...another mouthfull, great
}

//print money for emralds
bool printCash(int UUID, int quantity){
  player playerData = retrievePlayer(UUID);
  market marketToday = retrieveMarketToday();
  float priceOfDiamonds = marketToday.thing[20].salePrice; // = the current price of diamonds
  int sourceItems = getQuantityFromInv(unhashInv(playerData.inventoryHash),21); //number of emralds player has
  int minnedToday = playerData.quantityMined; //retrieve from player

  if(sourceItems < quantity){
    CenterString("not enough Emralds");
    CenterString("processing all remaining items");
    quantity = sourceItems;
  }
  if(1000 < minnedToday + quantity){ //limmit player to exchanging 1000 per day
    CenterString("Reached limmit for printing for today");
    CenterString("processing all remaining items");
    quantity = 1000 - minnedToday;
  }
  if(quantity <= 0){ //if quantity reaches 0 Transaction iss nullified and failed technically i dont have to return but i do to increace effiecency (i almost forgot quantity can go negitive here)
    CenterString("Transaction Failed returning");
    return false;
    //
  }

  //Proceed to printing!
  marketToday.ballance += quantity * priceOfDiamonds * .8; //market gets 80% of the cut
  playerData.cashBallance += quantity * priceOfDiamonds * .2; //player gets 20% of the cut
  marketToday.thing[IDofEmranlds].stock += quantity; //market -hords- aquires emralds x3...
  playerData.quantityMined += quantity;

  //void addToInventory(player &playerData, int ID, int &quantity) for reference
  quantity = -quantity; //change to negitive because the quantity is being taken away from player
  addToInventory(playerData.inventoryHash,IDofEmranlds,quantity); //value should go down also no need to make sure inventory runs out because alreaddy checked for that

  //save to logs
  if((updatePlayer(playerData)) && (updateMarketToday(marketToday)))
  return true;
  else {
    CenterString("ERROR! : something went wrong look up^..."); //specific error messages should have been printed above
    //insert something here that can revert it back to its previous state somehow mabe save a temporary copy of everything and put it here but then how would i revert it...the filesystem is whats failing /shrug
    CenterString("market is corrupted :'c"); //specific error messages should have been printed above
    return false;
    // PLEASE READ **MESSAGE ABOUT PONENTIAL FLAW** FROM processItemTransaction(), IT IS ALSO RELIVANT HERE
  }
  return true;
}

//exchange cash for credit not the other way around
bool exchangeChash(int UUID, float amount){
  player playerData = retrievePlayer(UUID);
  market marketToday = retrieveMarketToday();

  float priceOfDiamonds = marketToday.thing[20].salePrice; // = the current price of diamonds
  float sourceBallance = playerData.cashBallance;

  if(sourceBallance < amount){
    CenterString("not enough Cash");
    CenterString("processing what can be afforded");
    amount = sourceBallance;
  }
  if(amount == 0){ //if amount reaches 0 Transaction iss nullified and failed technically i dont have to return but i do to increace effiecency
    CenterString("Transaction Failed returning");
    return false;
  }

  //proceed with exchange
  marketToday.ballance +=amount;
  playerData.cashBallance -= amount * (200 / priceOfDiamonds);

  //save to logs
  if((updatePlayer(playerData)) && (updateMarketToday(marketToday))){
    return true;
  }
  else {
    CenterString("ERROR! : something went wrong look up^..."); //specific error messages should have been printed above
    //insert something here that can revert it back to its previous state somehow mabe save a temporary copy of everything and put it here but then how would i revert it...the filesystem is whats failing /shrug
    CenterString("market is corrupted :'c"); //specific error messages should have been printed above
    return false;
    // PLEASE READ **MESSAGE ABOUT PONENTIAL FLAW** FROM processItemTransaction(), IT IS ALSO RELIVANT HERE
  }
  //sucsess
  return true;
}

//exchange credit for amo (idk about this one, i might leave amo out compleatly. this is too complicated for me)
bool makeAmo(int UUID, int quantity){
  player playerData = retrievePlayer(UUID);
  market marketToday = retrieveMarketToday();

  float price = 0; // price of amunition....??? change this
  float sourceBallance = playerData.creditBallance;
  float priceOfDiamonds = marketToday.thing[20].salePrice; //the current price of diamonds in case i need it

  if(sourceBallance < quantity * price){
    CenterString("not enough Credit");
    CenterString("processing what can be afforded");
    quantity = sourceBallance / price; // dont worry this would neer execute if price was 0 because of the prier condition 0 is not less than 0, and the source ballance could never go negitive
  }
  if(quantity == 0){ //if quantity reaches 0 Transaction is nullified and failed technically i dont have to return but i do to increace effiecency
    CenterString("Transaction Failed returning");
    return false;
  }

  //proceed with exchange
  playerData.creditBallance -= quantity * price;

  int idofammo = 0; // ID of amunition....????.. change this (air)

  //void addToInventory(player &playerData, int ID, int &quantity) for reference
  addToInventory(playerData.inventoryHash,idofammo,quantity); //value should go up

  //save to logs
  if(updatePlayer(playerData)){
    return true;
  }
  else {
    CenterString("ERROR! : something went wrong look up^..."); //specific error messages should have been printed above
    //no chance of corruption here it either happens of it does not
    return false;
    // since the only thing being affected here is player data, if it fails to change, then nothing changed and there is no risk of incontinuity
  }
  //sucsess
  return true;
}

//returns stack limmit for item (probably should be in a config)
int stackLimmit(int ID){
  int table[] = {64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64};
  return table[ID];
}

//directly enter data to slot
void editSlot(int &slotID, int &slotAmt, int &quantity, int ID){
  //initial assignment
  slotID = ID;
  slotAmt = quantity;
  if(slotAmt > stackLimmit(slotID)) //prevent overflow
  slotAmt = stackLimmit(slotID);
  if(slotID == 0) //if its air, you can not have more air
  slotAmt = 0;
}

//adds the quantity to the slot that that fits
bool addToSlot(int &slotID, int &slotAmt, int &quantity, int ID){
  // (((quantity > 0) && !(slotAmt > stackLimmit(ID)) && (slotID == ID)) || ((quantity < 0) && !(slotAmt < 0) && (slotID == ID)))
  // this might be a tough one to explain ughhh hmmm on mabe if i can break this down it will be easyer...
  bool removing = (quantity < 0) , adding = (quantity > 0), empty = (slotAmt <= 0), full = (slotAmt >= stackLimmit(ID)), matches = (slotID == ID);
  //mabe this is a bit easyer to read
  if((adding && !full && matches) || (removing && !empty && matches)){ //thats better :D
    slotAmt += quantity;
    quantity = 0;
    //need to reastablish the full and empty variables after adding initial difference
    // keeping the slot within its bounds, it can not overflow or be in a defisit however you spell that
    full = (slotAmt >= stackLimmit(ID));
    empty = (slotAmt <= 0);
    if(full){                             //if the slot overflows...
      quantity = slotAmt - stackLimmit(ID); // take back the difference
      slotAmt = stackLimmit(ID);          // set it back to the limmit
    }
    else if(empty){     // if the slot is not in overflow it might be in defisit...
      quantity = slotAmt; // because quantity is 0 i can assign quantity the difference
      slotAmt = 0;      // then reset the slot back to 0
      slotID = 0;       // and clear its item ID tag
    }
    return true;
  }
  return false;
}

//edits the player inventory to add or remove items from it
void addToInventory(InvHash &inventoryHash, int ID, int &quantity){
  if(quantity != 0){ //skip everything if the quantity is 0 (if the quantity is 0 why would i even call this function in the first place)
    //unpack inventory from player
    Inventory invData = unhashInv(inventoryHash);
    //for reference (whats inside Inventory)
    // int item[y][x][z]; //item [slot y][slot x][id,quantity]
    // int hotbar[x][z]; //hotbar [slot x][id,quantity] (why did i include this? for continuity i guess)
    for(int j = 0; j < 9; j++){
      for(int i = 0; i < 3; i++){
        //unpack the slot
        int slotID = invData.item[i][j][0];
        int slotAmt = invData.item[i][j][1];
        addToSlot(slotID,slotAmt,quantity,ID);
        displayInventory(invData,i,j); //im so happy i was smart eough to pass invdata to this function imagin unpacking, repacking and then unpacking again, that would he horrible
        //repack the slot
        invData.item[i][j][0] = slotID;
        invData.item[i][j][1] = slotAmt;
        if(quantity == 0){
          return;
        }
      }
      //Hotbar
      //unpack the slot
      int slotID = invData.hotbar[j][0];
      int slotAmt = invData.hotbar[j][1];
      addToSlot(slotID,slotAmt,quantity,ID);
      displayInventory(invData,-1,j);
      //repack the slot
      invData.hotbar[j][0] = slotID;
      invData.hotbar[j][1] = slotAmt;
      if(quantity == 0){
        return;
      }
    }
    //repack inventory back into player
    inventoryHash = hashInv(invData);
  }
}

//overwrite price for item
void editPrice(float &price, float arg){
  price = arg;
  if(price < 0)
  price = 0; //no negitive numbers dept is hard to deal with although numerically it would probably still work, i just dont want to
}

//generates random market information for specific item or random item if passed -1
item generateItem(int id){
  //for reference
  // float noise;
  // float salePrice; //only store the sale price because the buy price is based on sell salePrice
  // int stock;
  // int quantityChange;
  //if no id given make one up
  if(id == -1){
    do{
      id = rand() % numberOfItems;
    }while(configTable.blackList[id]); //make sure item is not black listed
  }
  item item;
  if(!configTable.blackList[id]){
    float noise = (((rand() % 100) * .01) - 0.5) * 0.05;
    item.noise = noise;
    float basePrice = configTable.priceBase[id];
    item.salePrice = basePrice + (basePrice * noise); //only store the sale price because the buy price is based on sell salePrice
    item.stock = rand() % 400;
    item.quantityChange = 0;
  }
  else{
    item.salePrice = -1;
    item.noise = 0;
    item.stock = -1;
    item.quantityChange = 0;
  }
  return item;
}

//generates random market information for specific item or random item if passed -1
item createItem(int id){
  item item;
  if(!configTable.blackList[id]){
    float noise = 0;
    item.noise = noise;
    float basePrice = configTable.priceBase[id];
    item.salePrice = basePrice + (basePrice * noise); //only store the sale price because the buy price is based on sell salePrice
    item.stock = 0;
  }
  else{
    item.salePrice = -1;
    item.noise = 0;
    item.stock = -1;
  }
  return item;
}

//generate FUNCTIONS
//creates random market data
market generateMarket(){
  market marketData;
  marketData.ballance = rand() % 20000 + 1;
  for(int i = 0; i < numberOfItems; i++){
    marketData.thing[i] = generateItem(i);
  }
  return marketData;
}

//returns the name of the item
string lookupItem(int id){
  string table[] = {"Air","Stone","Grass","Dirt","Cobblestone","Planks","Sapling","Bedrock","flowing_water","still_water","flowing_lava","still_lava","sand","gravel","gold_ore","iron_ore","coal_ore","wood","leaves","sponge","glass","diamond","emrald"};
  return table[id];
}

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

//generates new player with givven UUID or random one if givver -1
player generatePlayer(int UUID){
  player playerData;
  if(UUID == -1) playerData.UUID = generateUUID();
  else playerData.UUID = UUID;
  string name = lookupItem(rand() % numberOfItems);
  bool reachedEnd = false;
  for(int i = 0; i < nameLength; i++){
    if(!reachedEnd){
      if(name[i] != '\0')
      playerData.name[i] = name[i];
      else{
        playerData.name[i] = '\0';
        reachedEnd = true;
      }
    }
    else{
      playerData.name[i] = '-';
    }
  }
  Inventory inventoryData = generateInventory();
  InvHash hashTag = hashInv(inventoryData);
  playerData.inventoryHash = hashTag;
  playerData.cashBallance = rand() % 1000;
  playerData.creditBallance = rand() % 4000;
  playerData.quantityMined = rand() % 200;
  updatePlayer(playerData);
  return playerData;
}

//generates random inventory for simulation purposes
Inventory generateInventory(){
  Inventory inventoryData;
  memset(inventoryData.item,0,sizeof(inventoryData.item));
  memset(inventoryData.hotbar,0,sizeof(inventoryData.hotbar));
  //for reference
  // int item[3][9][2]; //Inventory [slot y][slot x][id,quantity]
  // int hotbar[9][2]; //hotbar (why did i include this?) [slot x][id,quantity]
  for(int j = 0; j < 9; j++){
    for(int i = 0; i < 3; i++){
      if(rand() % 2 == 0){
        do{
          inventoryData.item[i][j][0] = rand() % numberOfItems; //id
        }while(configTable.blackList[inventoryData.item[i][j][0]]); //make sure item is not black listed
        inventoryData.item[i][j][1] = rand() % 50; //quantity
        cout << string(100,'\n');
        displayInventory(inventoryData,i,j);
        sleep(100);
      }
      else{
        inventoryData.item[i][j][0] = 0;
        inventoryData.item[i][j][1] = 0;
      }
    }
    if(rand() % 2 == 0){
      do{
        inventoryData.hotbar[j][0] = rand() % numberOfItems; //id
      }while(configTable.blackList[inventoryData.hotbar[j][0]]); //make sure item is not black listed
      inventoryData.hotbar[j][1] = rand() % 50; //quantity
      cout << string(100,'\n');
      displayInventory(inventoryData,-1,j);
      sleep(100);
    }
    else{
      inventoryData.hotbar[j][0] = 0;
      inventoryData.hotbar[j][1] = 0;
    }
  }
  return inventoryData;
}

//generates random UUID for simulation purposes
int generateUUID(){
  player player;
  int index = 0;
  int UUID = rand() % 1000 + 1; //reserve 0 for null player
  //CREATE OBJECTS AND READ FIRST PLAYER
  fstream readStream;
  readStream.open ("player.bin", ios::binary | ios::in);
  readStream.read(reinterpret_cast <char *> (&player), sizeof(player));
  do{
    index++; //increment index
    readStream.seekg(index * sizeof(player), ios::beg); //GO TO NEXT PLAYER
    readStream.read(reinterpret_cast <char *> (&player), sizeof(player)); //READ NEXT PLAYER
    if(player.UUID == UUID){ //CHECK PLAYER
      UUID = generateUUID(); //hmmmm RECURSION TO THE RESCUE!
    }
  }while(!readStream.fail()); //while not at end of file
  readStream.close(); //close file
  //print results
  return UUID;
}

//retrieve player information out of player database and player ballance log
player retrievePlayer(int UUID){
  // Create our objects.
  player player;
  int index = 0;
  fstream filestream;
  filestream.open ("player.bin", ios::binary | ios::in); //attempt to open file
  filestream.read(reinterpret_cast <char *> (&player), sizeof(player)); //read first struct
  if(filestream.fail()){ //check if empty
    filestream.clear();
    //create file if there is no file
    fstream create;
    create.open("player.bin", ios::binary | ios::in);
  }
  else{
    do{
      if(player.UUID == UUID){
        filestream.close();  //close file
        return player;
      }
      index++; //set index for next struct
      filestream.seekg(index * sizeof(player), ios::beg); //go to next struct
      filestream.read(reinterpret_cast <char *> (&player), sizeof(player)); //read struct
    }while(!filestream.fail());
    if(UUID == -1){
      filestream.seekg((rand() % index) * sizeof(player), ios::beg);
      filestream.read(reinterpret_cast <char *> (&player), sizeof(player));
      filestream.close();  //close file
      return player;
    }
  }

  //if file could not be opened make one
  filestream.close();  //close file
  return generatePlayer(UUID);
}

//updates player information in the player database
bool updatePlayer(player data){

  // Create our objects.
  player tempPlayer;
  fstream readStream;

  readStream.open ("player.bin", ios::binary | ios::in); //open in input mode
  int index = 0; //set counter
  while(!readStream.fail()){ //check if read failed
    readStream.read(reinterpret_cast <char *> (&tempPlayer), sizeof(tempPlayer)); //read first player

    if(tempPlayer.UUID == data.UUID){ //check if match
      CenterString("yes");
      fstream WriteStream;
      WriteStream.open ("player.bin", ios::binary | ios::out); //open in output mode
      WriteStream.seekg(index * sizeof(player), ios::beg); //set position for write to next position
      WriteStream.write(reinterpret_cast <char *> (&data), sizeof(player)); //overwrite data
      WriteStream.close();
      if(WriteStream.fail()){ //check if write failed
        return false;
      }
      else{
        return true;
      }
    }
    index++; //increment counter
    readStream.seekg(index * sizeof(player), ios::beg);  //set position for read to next position
    readStream.read(reinterpret_cast <char *> (&tempPlayer), sizeof(player)); //read nex player
  }

  fstream appendStream;
  appendStream.open ("player.bin", ios::binary | ios::out | ios::app); //open in input mode
  appendStream.write(reinterpret_cast <char *> (&data), sizeof(player));
  if(appendStream.fail()){ //check if write failed
    return false;
  }
  else{
    return true;
  }
}

void displayPlayerLog(int verbosity){
  // Create our objects.
  player temp;
  fstream readStream;
  int index = 0;
  readStream.open ("player.bin", ios::binary | ios::in);
  readStream.read(reinterpret_cast <char *> (&temp), sizeof(temp));
  printLine('-');
  LeftString(returnPlayerHeadder());
  while(!readStream.fail()){
    printLine('-');
    if(verbosity >= 1){
      LeftString(returnPlayerData(temp));
    }
    if(verbosity >= 2){
      displayInventory(unhashInv(temp.inventoryHash),-1,-1);
    }
    //display
    index++; //increment index
    readStream.seekg(index * sizeof(temp), ios::beg); //GO TO NEXT PLAYER
    readStream.read(reinterpret_cast <char *> (&temp), sizeof(temp));
  }
  readStream.close();
}


//purges mining information in player log
bool purgeMining(){
  // Create our objects.
  player playerData;
  int index = 0;
  fstream filestream;
  //attempt to open file and then read first player
  filestream.open ("player.bin", ios::binary | ios::in | ios::out);
  filestream.read(reinterpret_cast <char *> (&playerData), sizeof(playerData)); //fread first struct
  //if failed to open
  if(filestream.fail()){
    //create file if there is no file
    fstream create;
    create.open("player.bin", ios:: binary | ios::in | ios::out);
  }
  else{
    do{
      playerData.quantityMined = 0;
      filestream.write(reinterpret_cast <char *> (&playerData), sizeof(playerData));
      index++; //set index for next struct
      filestream.seekg(index * sizeof(playerData), ios::beg); //go to next struct
      filestream.read(reinterpret_cast <char *> (&playerData), sizeof(playerData));
    }while(!filestream.fail());
    //if player is not in list append it to end of list (if thread reached here seekg alreaddy pushed the position to end of file so its safe to just write from here)
  }
  filestream.write(reinterpret_cast <char *> (&playerData), sizeof(playerData));
  if(filestream.fail()){
    //create file if there is no file
    filestream.close();
    return false;
  }
  filestream.close();
  return true;
}

//retrieve market object from market log
market retrieveMarketToday(){

  // Create our objects.
  market today;
  fstream marketstream;

  marketstream.open ("Market_Today.bin", ios::binary | ios::in); //open file
  marketstream.read (reinterpret_cast <char *> (&today), sizeof(today) ); // read data as a block:
  // See if we opened the file successfully.
  if (!marketstream.fail()){
    marketstream.close(); //close file
    return today;
  }

  //if file could not be opened make one
  printLong("Merket log could not be found generating new market with random data");
  return generateMarket();
}

//updates market today
bool updateMarketToday(market today){
  // Create our objects.
  fstream daily;
  daily.open ("Market_Today.bin", ios::binary | ios::out | ios::trunc);
  daily.write(reinterpret_cast <char *> (&today), sizeof(today));
  if(daily.fail()){
    daily.close();
    CenterString("daily failed!");
    CenterString("write error");
    return false;
  }
  daily.close();
  return true;
}

//saves market object to market log
bool logMarket(market data){
  // Create our objects.
  fstream perminant;
  perminant.open ("market.bin", ios::binary | ios::app | ios::out);
  perminant.write(reinterpret_cast <char *> (&data), sizeof(data));
  if(perminant.fail()){
    perminant.close();
    return false;
  }
  perminant.close();
  return true;
}

// -2 for just the ballance, -1 for full log, and some number for that item
void displayMarketLog(int ID){
  // Create our objects.
  market temp;
  fstream readStream;
  int index = 0;
  readStream.open ("market.bin", ios::binary | ios::in);
  if(ID >= 0) displayMarketHeadder();
  readStream.read(reinterpret_cast <char *> (&temp), sizeof(temp));
  while(!readStream.fail()){

    if(ID >= 0) LeftString(getItemData(temp.thing[ID],-1,-1,-1));
    else if(ID == -2){
      LeftString(build("Day :",index + 1,build(" market ballance :",temp.ballance," cash"))); //probably one of the coolest lines ive ever written i didnt know build was so awesome
    }
    else{
      displayMarketHeadder();
      displayMarketData(temp,-1,-1);
    }
    //display
    index++; //increment index
    readStream.seekg(index * sizeof(temp), ios::beg); //GO TO NEXT Market log entery
    readStream.read(reinterpret_cast <char *> (&temp), sizeof(temp));
  }
  readStream.close();
}

//fetching data from retrieved file data functions
int getQuantityFromInv(Inventory inventoryData, int ID){
  int sum = 0;
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 9; j++){
      if(inventoryData.item[i][j][0] == ID){
        sum += inventoryData.item[i][j][1];
      }
    }
    if(inventoryData.hotbar[i][0] == ID){
      sum += inventoryData.hotbar[i][1];
    }
  }
  return sum;
}

//Display FUNCTIONS



void displayMarketHeadder(){
  printLine('-');
  CenterString("Market Table");
  string outstring;
  outstring.append(bindCenter(" ID ",4));
  outstring.append(bindCenter(" Name ",20));
  outstring.append(bindCenter(" Sale price ",18));
  outstring.append(bindCenter(" Noise ",10));
  outstring.append(bindCenter(" Stock ",10));
  outstring.append(bindCenter(" Change ",6));
  CenterString(outstring);
  printLine('-');
}

string getItemData(item itemData, int current, int selY, int selX){
  string returnstring;
  //ID
  returnstring.append(bindRight(to_string(current),4));
  //NAME
  returnstring.append(bindCenter(lookupItem(current),20));
  //PRICE
  string block;
  if(itemData.salePrice != -1)
  block.append(String_2Decimals(to_string(itemData.salePrice)));
  else
  block.append("Inf");
  block.append(" $");
  returnstring.append(bindRight(select(block,current,0,selY,selX),18));
  block.clear();
  //NOISE
  block.append(String_2Decimals(to_string(itemData.noise * 100)));
  block.append(" %");
  returnstring.append(bindRight(block,10));
  block.clear();
  //STOCK
  returnstring.append(bindRight(select(to_string(itemData.stock),current,1,selY,selX),10));
  //CHANGE
  returnstring.append(bindRight(select(to_string(itemData.quantityChange),current,2,selY,selX),6));
  return returnstring;
}

void displayMarketData(market marketData, int selY, int selX){
  printLine('=');
  CenterString(build("market ballance :",marketData.ballance," cash"));
  displayMarketHeadder();
  for(int i = 0; i < numberOfItems; i++){
    string outstring;
    CenterString(getItemData(marketData.thing[i],i,selY,selX));
  }
}

InvHash hashInv(Inventory dataTable){
  InvHash hashTag;
  int counter = 0;
  int counter2 = 0;
  for(int k = 0; k < z; k++){
    for(int j = 0; j < x; j++){
      for(int i = 0; i < y; i++){
        hashTag.InvTag[counter] = dataTable.item[i][j][k];
        counter++;
      }
      hashTag.HotTag[counter2] = dataTable.hotbar[j][k];
      counter2++;
    }
  }
  return hashTag;
}

Inventory unhashInv(InvHash hashTag){
  Inventory dataTable;
  memset(dataTable.item,0,sizeof(dataTable.item));
  memset(dataTable.hotbar,0,sizeof(dataTable.hotbar));
  int counter = 0;
  int counter2 = 0;
  for(int k = 0; k < z; k++){
    for(int j = 0; j < x; j++){
      for(int i = 0; i < y; i++){
        dataTable.item[i][j][k] = (int)hashTag.InvTag[counter];
        counter++; //because doing this with figureing this out with i j k and x y z is a dumb idea
      }
      dataTable.hotbar[j][k] = (int)hashTag.HotTag[counter2];
      counter2++;
    }
  }
  return dataTable;
}

void displayHash(InvHash hashtag){
  int counter = 0;
  int counter2 = 0;
  for(int k = 0; k < z; k++){
    //invnetory
    string hotstring;
    for(int i = 0; i < y; i++){
      string outstring;
      for(int j = 0; j < x; j++){
        string block;
        block.push_back('[');
        block.append(bindRight(to_string(counter),2));
        block.push_back(',');
        string block2;
        block2.push_back(hashtag.InvTag[counter]);
        block.append(bindLeft(block2,2));
        block.push_back(']');
        outstring.append(block);
        counter++; //because doing this with figureing this out with i j k and x y z is a dumb idea
      }
      CenterString(bindCenter(outstring,6));
    }
    //hotbar1
    for(int j = 0; j < x; j++){
      string block;
      block.push_back('[');
      block.append(bindRight(to_string(counter2),2));
      block.push_back(',');
      string block2;
      block2.push_back(hashtag.HotTag[counter2]);
      block.append(bindLeft(block2,2));
      block.push_back(']');
      hotstring.append(block);
      counter2++;
    }
    printLine('-');
    CenterString(bindCenter(hotstring,6));
    printLine('-');
  }
}

void displayInventory(Inventory invData, int selY, int selX){
  printLine('-');
  string hotstring;
  for(int i = 0; i < 3; i++){
    string outstring;
    for(int j = 0; j < 9; j++){
      string block;
      block.append(bindRight(to_string(invData.item[i][j][0]),2));
      block.push_back(',');
      block.append(bindLeft(to_string(invData.item[i][j][1]),2));
      outstring.append(select(block,selY,selX,i,j));
    }
    CenterString(outstring);
  }
  for(int j = 0; j < 9; j++){
    string block;
    block.append(bindRight(to_string(invData.hotbar[j][0]),2));
    block.push_back(',');
    block.append(bindLeft(to_string(invData.hotbar[j][1]),2));
    hotstring.append(select(block,selY,selX,-1,j));
  }
  printLine('+');
  CenterString(hotstring);
}


string returnPlayerHeadder(){
  string outstring;
  outstring.append(bindCenter((string)" UUID  ",7));
  outstring.append(bindCenter((string)" Name  ",20));
  outstring.append(bindCenter((string)" Cash Bal ",13));
  outstring.append(bindCenter((string)" Credit Bal ",13));
  outstring.append(bindCenter((string)" QTY mined ",11));
  return(outstring);
}

string returnPlayerData(player playerData){
  string returnstring;
  returnstring.append(bindCenter(to_string(playerData.UUID),7));
  returnstring.append(bindCenter(shorten(playerData.name),20));
  returnstring.append(bindRight(String_2Decimals(to_string(playerData.cashBallance)),11));
  returnstring.append(" $");
  returnstring.append(bindRight(String_2Decimals(to_string(playerData.creditBallance)),11));
  returnstring.append(" #");
  returnstring.append(bindCenter(to_string(playerData.quantityMined),11));
  return(returnstring);
}


//GRAPHICAL FUNCTIONS THEY DO NOOOTHINNGGGGG

//takes 2 pairs of coordinates and returns a string with or without a selection marker if the coordinates mach
string select(string arg,int selY, int selX, int posY, int posX){
  bool selected = ((posX == selX) && (posY == selY));
  string returnstring;
  if(selected)
  returnstring.push_back('[');
  else
  returnstring.push_back(' ');
  returnstring.append(arg);
  if(selected)
  returnstring.push_back(']');
  else
  returnstring.push_back(' ');
  return returnstring;
}

//converts char array name to string
string shorten(char name[nameLength]){
  string returnstring = name;
  return returnstring;
}

/*  centers the string within a length  */
string bindCenter(string arg, int length){
  string returnstring;
  if(arg.length() < (unsigned)length){
    int str_length = arg.length() / 2;
    if(length % 2 != 0)
    returnstring.push_back(' ');
    if(arg.length() % 2)
    returnstring.append(string(length / 2 - str_length - 1,' '));
    else
    returnstring.append(string(length / 2 - str_length,' '));
    returnstring.append(arg);
    returnstring.append(string(length / 2 - str_length,' '));
  }
  else{
    returnstring.append(arg);
  }
  return returnstring;
}

/*  left justifies the string within a length  */
string bindLeft(string arg, int length){
  string returnstring;
  if(arg.length() <= (unsigned)length){
    int str_length = arg.length();
    returnstring.append(arg);
    returnstring.append(string(length - str_length,' '));
  }
  else{
    returnstring.append(arg);
  }
  return returnstring;
}

/*  right justifies the string within a length  */
string bindRight(string arg, int length){
  string returnstring;
  if(arg.length() <= (unsigned)length){
    int str_length = arg.length();
    returnstring.append(string(length - str_length,' '));
    returnstring.append(arg);
  }
  else{
    returnstring.append(arg);
  }
  return returnstring;
}

//combine string and float and string while reducing the float to 2 decimals
inline string build(string sA, float sB, string sC) {
  sA.append(String_2Decimals(to_string(sB)));
  sA.append(sC);
  return sA;
}

//shortens a floting pooint number string to 2 decimals
string String_2Decimals(string arg){
  int i = 0;
  while(true){
    if(arg[i] == '.' && arg[i + 1] != '\0' && arg[i + 2] != '\0')
    arg.erase(i+3,arg.length());
    i++;
    if(arg[i] == '\0') break;
  }
  return arg;
}

/* displays a menue with options and controlls */
void displayMenue(string title, string discription, string options[],int sizeofOptions){
  printLine('=');
  CenterString(title);
  printLine('-');
  printLong(discription);
  printLine('-');
  printMultiString(options,sizeofOptions);
}

/* Takes a long string and prints it to multiple lines while word wrapping */
void printLong(string longString){
  unsigned int pos = 0;
  bool writing = true;
  do{
    string outString;
    int wordlength = 0;
    for(int i = 0; i < display_Width - 4; i++){
      if(pos >= longString.length()){
        writing = false;
      }
      else{
        outString.push_back(longString[pos]);
        if(longString[pos] == ' '){
          wordlength = 0;
        }
        else{
          wordlength++;
        }
        pos++;
      }
    }
    if((writing) && (wordlength > 0)){
      pos -= wordlength;
      for(int j = 0; j < wordlength; j++){
        outString.pop_back();
      }
    }
    LeftString(outString);
  }while(writing);
}

void printMultiString(string strArray[],int sizeOfStrArray){
  unsigned int max = 0;
  for(unsigned int i = 0; i < sizeOfStrArray/sizeof(string); i++){
    if(strArray[i].length() > max){
      max = strArray[i].length();
    }
  }
  //if the array is too big
  if((max + 5) * sizeOfStrArray/sizeof(string) > display_Width){
    int quantityThatFits = 0;
    for(int i = (sizeOfStrArray/sizeof(string)) - 1; i > 0; i--){
      if((max + 5) * i > display_Width){
        continue;
      }
      quantityThatFits = i;
      break;
    }
    //print what fits
    string outString;
    for(int j = 0; j < quantityThatFits; j++){
      outString.append("  ");
      outString.append(strArray[j]);
      outString.append(string(max - strArray[j].length(),' '));
      outString.append("  ");
    }
    LeftString(outString);
    //truncate array for recurcive call
    int SizeOfNewStrArray = (sizeOfStrArray/sizeof(string)) - quantityThatFits;
    string newStrArray[SizeOfNewStrArray];
    for(int j = 0; j < SizeOfNewStrArray; j++){
      newStrArray[j] = strArray[j + quantityThatFits];
    }
    //recurcive call
    printMultiString(newStrArray,SizeOfNewStrArray * sizeof(string));
  }
  //print array of strings
  else{
    string outString;
    for(unsigned int j = 0; j < sizeOfStrArray/sizeof(string); j++){
      outString.append("  ");
      outString.append(strArray[j]);
      outString.append(string(max - strArray[j].length(),' '));
      outString.append("  ");
    }
    LeftString(outString);
  }
}

/* prints a chatacter string across the display */
void printLine(char weight){
  cout << "|";
  cout << string(display_Width + 1, weight);
  cout << "|" << endl;
}

/* prints a title then a character string across the display */
void titleLine(string str, char weight){
  cout << "| ";
  int str_length = str.length();
  cout << str;
  cout << string(display_Width - str_length, weight);
  cout << "|" << endl;
}

/* contains a message within the display boundries centered */
void LeftString(string str){
  cout << "| ";
  cout << bindLeft  (str,display_Width);
  cout << "|" << endl;
}
/* contains a message within the display boundries centered */
void CenterString(string str){
  cout << "| ";
  cout << bindCenter(str,display_Width);
  cout << "|" << endl;
}
/* contains a message within the display boundries centered */
void RightString(string str){
  cout << "| ";
  cout << bindRight (str,display_Width);
  cout << "|" << endl;
}
