/*
a simulation/representation of the algorithms involved in an enhanced market plugin concept for minecraft
@file    market.cpp
@author  Speed
*/

#include <iostream> //for console io
#include <fstream>  //for file io
#include <cstring>  //for memset
#include <algorithm> //for min and max

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
  int quantityPrinted;
};

//main FUNCTIONS
//actual functions that would exist
void newDay(market &marketToday); //mid day each day
bool processItemTransaction (market &marketToday, player &playerData, int ID, int quantity); //for every time something is bought or sold for cash
void printCash              (market &marketToday, player &playerData, int quantity);         //print money for emralds
void exchangeChash          (market &marketToday, player &playerData, float amount);         //exchange cash for credit not the other way around
//edit sub group
void addToInventory (InvHash &inventoryHash, int ID, int &quantity);    // edits the player inventory to add or remove items from it
bool addToSlot      (int &slotID, int &slotQTY, int &quantity, int ID); // adds quantity of id to slot
bool verifySlot     (int &slotID, int &slotQTY, int &quantity, int ID); // verifys the slot parameters and ajdusts quantity if needed, returns false if the slot
void editPrice      (float &price, float arg);                          // overwrite price for item
int retrievePlayerUUID();                                               //text gui for a selection based player retriever

void makeAmo        (market &marketToday, player &playerData, int quantity); //exchange credit for amo (am unsure about this one)

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

//for both (potentially)
void displayGraph(float data[], int length); //displays the last length'th data  elements in an array as a graph

// for market
void displayMarketData  (market marketData, int selY, int selX); //displays market information on every item with a selector
void displayMarketHeadder();                      //displays labels for each peice of information of the market data
string getItemData (item itemData, int current, int selY, int selX);   //Gets item data for item and returns it as a formatted string with a selector :o

// for Player
string returnPlayerData  (player playerData); //returns basic player data for cash and creddit ballance and quantity minned as a string
string returnPlayerHeadder();                 //returns labels for each peice of information of the player data as a string
void displayInventory   (Inventory invData, int selY, int selX);
void displayHash        (InvHash hashtag); //probably wont ever get this running correctly

// displaying filesystem functions.....yeah i dont know where these go either
void displayMarketLog(int ID);                // -2 for just the ballance, -1 for full log, and some number for that item
void displayMarketGraph(int ID, int length);  // displays the price of something over time as a graph -1 for market ballance
void displayPlayerLog(int verbosity);         //1 for basic info 2 for basic info and inventory

// reference FUNCTIONS

//probably wont need this one
string lookupItem(int id); //returns the name of the item (will change to config file later...mabe)
//or this
int stackLimmit  (int ID); //returns stack limmit for item (probably should be in a config)

struct config{
  bool blackList[numberOfItems];
  float priceBase[numberOfItems];
};

config getConfig(); //attempt at config file indexer
bool createConfig();//creates template config

bool blackList(int id); //list of items illigal to buy or sell (will change to config file later)
int priceBase (int id); //is base for price (will change to config file later)

//  Filesystem FUNCTIONS

//for market
market retrieveMarketToday();           //retrieve market object from market log
bool updateMarketToday(market today);   //overwrites market today
bool logMarket        (market market);  //saves market object to market log and overwrites market today

// for player
player retrievePlayer(int UUID, int index);  // retrieve player information out of player database and player ballance log
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

string progress(int current, int total){
  string bar;
  bar.append("[");
  bar.append(string(current + 1,'#'));
  bar.append(string(total - (current + 1),' '));
  bar.append("]");
  return bar;
}

int main(){
  srand(time(NULL));
  //menue settup
  string title = "Main Menue";
  string discription = "Market Plus, a simulation/representation of the algorithms involved in an enhanced market plugin concept for minecraft. By Speed";
  string options[]= {"1 Function-tester","2 Simulation","Esc Quit"};

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
        case '2': Simulation();      break; // this will probably break a lot
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
  fstream writestream;
  writestream.open ("file.cfg", ios::binary | ios::out); //attempt to open file
  if(!writestream.is_open()){ //check if empty
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
    writestream.write(reinterpret_cast <char *> (&temp), sizeof(config));
    writestream.close();  //close file
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

player createPlayer(int UUID){
  player playerData;
  memset(playerData.inventoryHash.HotTag,0,sizeof(playerData.inventoryHash.HotTag));
  memset(playerData.inventoryHash.InvTag,0,sizeof(playerData.inventoryHash.InvTag));
  playerData.UUID = UUID; //i could pass 0 here if i wanted it to be a null player
  memset (playerData.name,' ',sizeof(playerData.name));
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
  playerData.cashBallance = 0;
  playerData.creditBallance = 0;
  playerData.quantityPrinted = 0;
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
  player playerData = createPlayer(0);

  //menue settup
  string title = "Function-tester";
  string discription = "test each function manually, and see their effects, generate functions generate data and stores it in memory, retrieve functions retrieve data stored on disc (database), update functions store and/or updates information stored on disc (database)";
  string options[]= {
    "1 Main group","2 generate group","3 Edit group",
    "4 retrieve group","5 Display Log","6 Display group",
    "7 Update group","Esc Quit"
  };

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

              case '1': newDay(marketData); break;
              case '2':
              LeftString("processItemTransaction(UUID,ID,quantity)");
              getInput("ID",ID);
              getInput("quantity",quantity);
              if(playerData.UUID == 0){
                UUID = retrievePlayerUUID();
                playerData = retrievePlayer(UUID,-1);
              }
              if(UUID == 0){
                CenterString("Null player detected... who did this? generate a player");
                break;
              }
              processItemTransaction(marketData,playerData,ID,quantity);
              break;

              case '3': ;
              LeftString("printCash(UUID,quantity)");
              if(playerData.UUID == 0){
                UUID = retrievePlayerUUID();
                playerData = retrievePlayer(UUID,-1);
              }
              if(UUID == 0){
                CenterString("Null player detected... who did this? generate a player");
                break;
              }
              getInput("quantity",quantity);
              printCash(marketData,playerData,quantity);
              break;

              case '4': ;
              LeftString("exchangeChash(UUID,amount)");
              if(playerData.UUID == 0){
                UUID = retrievePlayerUUID();
                playerData = retrievePlayer(UUID,-1);
              }
              if(UUID == 0){
                CenterString("Null player detected... who did this? generate a player");
                break;
              }
              getInput("Amount",amount);
              exchangeChash(marketData,playerData,amount);
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
              case '3': playerData = createPlayer(0);   break;
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
        groupOptions[0] = "1 retrievePlayer()     ";
        groupOptions[1] = "2 retrieveMarketToday()";
        groupRunning = true;
        displayMenue(GroupTitle,groupDiscription,groupOptions,sizeof(groupOptions));
        do{
          if(KeyHit()){
            groupRunning = false;
            cout << string(100,'\n');
            char menue = GetChar();
            menue = tolower(menue);
            switch (menue) {
              case '1': playerData = retrievePlayer(retrievePlayerUUID(),-1); break;
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
        groupOptions[2] = "3 displayMarketGraph()";
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
              subOptions[0] = "-2 full market logs";
              subOptions[1] = "-1 market ballence ";
              subOptions[2] = "item ID sale price ";
              displayMenue(subtitle,subdiscription,subOptions,sizeof(subOptions));
              getInput(">",parameter); //i should write in som input verifacation here... mabe later
              displayMarketLog(parameter);
              break;
              case '2':
              subtitle = "displayPlayerLog()";
              subdiscription = "enter parameter for displayMarketData()";
              subOptions[0] = "1 basic info               ";
              subOptions[1] = "2 basic info and inventory ";
              displayMenue(subtitle,subdiscription,subOptions,sizeof(subOptions));
              getInput(">",UUID); //i should write in som input verifacation here... mabe later
              displayPlayerLog(UUID);
              break;
              case '3':
              subtitle = "displayMarketGraph()";
              subdiscription = "enter parameter for displayMarketGraph()";
              subOptions[0] = "-1 market ballence";
              subOptions[1] = "item ID sale price";
              displayMenue(subtitle,subdiscription,subOptions,sizeof(subOptions));
              getInput(">",ID); //i should write in som input verifacation here... mabe later
              LeftString("number of days to go back (limmit about 60)");
              getInput(">",parameter); //i should write in som input verifacation here... mabe later
              displayMarketGraph(ID,parameter);
              break;
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
              case '1': updatePlayer(playerData); break;
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
  fstream readstream;
  int index = 0;
  readstream.open ("player.bin", ios::binary | ios::in);
  readstream.read(reinterpret_cast <char *> (&temp), sizeof(temp));
  while(index < count){ //i alreaddy know how manny players there are, Just build the table
    playerTable[index] = temp;
    //display
    index++; //increment index
    readstream.seekg(index * sizeof(temp), ios::beg); //GO TO NEXT PLAYER
    readstream.read(reinterpret_cast <char *> (&temp), sizeof(temp));
  }
  readstream.close();

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
  int quantity = 0, ID = 0, selectionID = 0, selectionQTY = 0;
  float amount = 0;
  //for reference ... i do this a lot
  //int item[y][x][z]; //Inventory [slot y][slot x][id,quantity]
  //int hotbar[x][z]; //hotbar [slot x][id][quantity] (why did i include this? for continuity i guess)

  do{
    if(KeyHit()){
      char menue = GetChar();
      //dont pack or unpack if user is just moving the selection around
      //or els silleh things will happen
      if(!(menue == 'w' || menue == 'a' || menue == 's' || menue == 'd')){ //friggen demorgan
        //depending on where the selection the selection ID and quantity need to be based on the right data set
        if(selY >= 0){
          selectionQTY = inventoryData.item[selY][selX][1];
          selectionID = inventoryData.item[selY][selX][0];
        }
        else{
          selectionQTY = inventoryData.hotbar[selX][1];
          selectionID = inventoryData.hotbar[selX][0];
        }
      }
      cout << string(100,'\n');
      switch (menue) {
        //selection section
        //selection movement
        case 'w':
        selY--;//chage position
        //validate position
        if(selY < -1){
          selY = 2;
        }
        break;

        case 'a':
        selX--;
        selX %= 9;
        if(selX < 0){
          selX = 8;
        }
        break;

        case 's':
        selY++;
        if(selY > 2){
          selY = -1;
        }
        break;

        case 'd':
        selX++;
        selX %= 9;
        break;

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

        switch (menue) { //switchseption to cover each different quantity after for each case
          case '1':
          printLong("Enter the quantity to add to the slot, positive or negative");
          getInput(">",quantity);
          break;
          case '2': quantity = 1;  break;
          case '3': quantity = -1; break;
        }

        //for reference
        //inline void addToSlot(int slotID, int slotQTY, float quantity, int ID)
        //in any case <.< i want the same action to take place with these same variables so unless the user exits, this would happen every time
        addToSlot(selectionID,selectionQTY,quantity,ID); //passing both ID and selectionID because the selection might be air, and then the id
        quantity = 0; //reset it to 0 reguardless of the outcome, since if it did not go into the inventory there is nowhere else for it to go
        break;

        //edit slot
        case '4':
        printLong("Enter ID");
        getInput(">",ID);
        printLong("Enter Quantity");
        getInput(">",quantity);
        //set the values
        selectionID = ID;
        selectionQTY = quantity;
        //for reference
        //bool verifySlot(int &slotID, int &slotQTY, int &quantity, int ID){
        verifySlot(selectionID,selectionQTY,quantity,ID); //even though they are equal, this is the proper syntax for this function....OCD much?
        quantity = 0; //reset it to 0 reguardless of the outcome, since if it did not go into the inventory there is nowhere else for it to go
        break;

        // purge slot
        // void editSlot(int &slotID, int &slotQTY, int &quantity, int ID)
        case '5':
        selectionQTY = 0;
        selectionID = 0;
        // i almost used editSlot() for this... seesh
        break;

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

        //exit section
        case '8': running = false; break;  //exit the switch and the do loop, repack, save then exit the function
        case '9': running = false; return; //just exit the entire function without making any changes

        //invalid input section
        default:
        printLine('#');
        printLine(' ');
        CenterString("invalid input");
        string key;
        key.push_back(menue);
        CenterString(key); // << shows incorrect key pressed
      }

      //dont pack or unpack if user is just moving the selection around
      //or els silleh things will happen
      if(!(menue == 'w' || menue == 'a' || menue == 's' || menue == 'd')){
        // amagin if i nneded to repack here, oh that would have been awefull
        if(selY >= 0){
          inventoryData.item[selY][selX][1] = selectionQTY;
          inventoryData.item[selY][selX][0] = selectionID;
        }
        else{
          inventoryData.hotbar[selX][1] = selectionQTY;
          inventoryData.hotbar[selX][0] = selectionID;
        }
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


void dailyActions(player playerBase[10], market &marketToday){
  printLine('=');
  CenterString("daily actions!");
  printLine('=');
  // LeftString(returnPlayerHeadder());
  for(int i = 0; i < 10; i++){ //for each player
    // player playerData = retrievePlayer(0,i); <- more represens what would happen, keeping the most of the data out of memory,
    // so if/when the server shuts down sudenly noone loses anything

    int quantity;
    quantity = rand() % 2; //add a random quantity to players inventory to simulate material gained by normal means
    addToInventory(playerBase[i].inventoryHash,IDofEmranlds,quantity); //add the amount

    do{
      quantity = -rand() % 5; //remove a random quantity to players inventory to simulate material lost by normal means
    }while(getQuantityFromInv(unhashInv(playerBase[i].inventoryHash),IDofEmranlds) < -quantity); // make sure amount taken away is not greater than the amount the player has
    addToInventory(playerBase[i].inventoryHash,IDofEmranlds,quantity); //remove the amount

    if(getQuantityFromInv(unhashInv(playerBase[i].inventoryHash),IDofEmranlds) > 20){
      printCash(marketToday,playerBase[i],10);
    }

    playerBase[i].creditBallance += rand() % 100;
    playerBase[i].creditBallance -= rand() % 4000;

    if(playerBase[i].creditBallance < 30){
      exchangeChash(marketToday,playerBase[i],-playerBase[i].creditBallance);
    }

    for(int j = 0; j < numberOfItems; j++){ //for each item
      if(!configTable.blackList[j]){ //only update items not on black list
        // by normal means
        quantity = rand() % 30; //add a random quantity to players inventory to simulate material gained by normal means
        addToInventory(playerBase[i].inventoryHash,j,quantity); //add the quantity
        do{
          quantity = -rand() % 20; //remove a random quantity to players inventory to simulate material lost by normal means
        }while(getQuantityFromInv(unhashInv(playerBase[i].inventoryHash),j) < -quantity); // make sure quantity taken away is not greater than the amount the player has
        addToInventory(playerBase[i].inventoryHash,j,quantity); //remove the quantity

        // by market
        if(getQuantityFromInv(unhashInv(playerBase[i].inventoryHash),j) < 10 ){ //if the player had less than 10 of items attempt buy 10 items
          quantity = rand() % 20; //atempt to buy a random quantity
          processItemTransaction(marketToday,playerBase[i],j,-quantity); //negitive because this is from the markets perspective
        }
        else if(getQuantityFromInv(unhashInv(playerBase[i].inventoryHash),j) > 40){ //if the player had more than 20 of items attempt sell 10 items
          quantity = rand() % 20; //atempt to sell a random quantity
          processItemTransaction(marketToday,playerBase[i],j,quantity); //positive because this is from the markets perspective
        }
      }
    }
    // printLine('-');
    // CenterString(build("this player has ",getQuantityFromInv(unhashInv(playerBase[i].inventoryHash),IDofEmranlds),"emralds"));
    // printLine('-');
    LeftString(returnPlayerData(playerBase[i]));
    // updatePlayer(playerData); //save actions to player to database <- more represens what would happen, keeping the most of the data out of memory,
    // so if/when the server shuts down sudenly noone loses anything
  }
}

void shiftback(float array[40], float entery){
  for(int i = 39; i > 0; i--){
    array[i] = array[i - 1];
  }
  array[0] = entery;
}

void Simulation(){
  if(!remove("player.bin"))
  CenterString("deleted playerFile");
  if(!remove("market.bin"))
  CenterString("deleted marketFile");
  if(!remove("Market_Today.bin"))
  CenterString("deleted marketlog");
  CenterString("Generating new marketFile");
  market marketToday = createMarket();
  float marketLog[40]; //for super fast logging, keeping it in memory.
  memset(marketLog,3,sizeof(marketLog));
  CenterString("Generating new players");
  player playerBase[10];
  for(int i = 0; i < 10; i++){
    //due to me wanting to improve preformance, im going to use the filesystem less and keep as much as i can in MEMORY
    //this reflecs less on the actual operation of the plugin but improves the simulation Speed
    playerBase[i] = createPlayer(generateUUID());
    updatePlayer(playerBase[i]);
  }
  bool running = true;
  do{
    cout << string(100,'\n');
    CenterString(build("ballance",marketToday.ballance,"$"));
    // displayMarketGraph(-1,100);
    dailyActions(playerBase,marketToday);
    shiftback(marketLog,marketToday.ballance);
    displayGraph(marketLog,40);
    newDay(marketToday);
  }while(running);
}

//main FUNCTIONS actual functions that would exist
void newDay(market &newMarket){
  // market newMarket = retrieveMarketToday();
  for(int i = 0; i < numberOfItems; i++){
    if(!configTable.blackList[i]){
      int change = newMarket.thing[i].quantityChange;
      float noise;
      if(change != 0)
      noise = (((rand() % 100) * .01) - 0.5) * 0.05;
      float salePrice = newMarket.thing[i].salePrice;
      if(change < 0) //selling a lot
      salePrice += salePrice * (.10 + (noise)); //increase average 10%
      else if(change > 0) // buying a lot
      salePrice -= salePrice * (.10 + (noise)); //decrease average 10%
      else
      if(change != 0){
        newMarket.thing[i].noise = noise;
        newMarket.thing[i].quantityChange = change;
        newMarket.thing[i].salePrice = salePrice;
      }
    }
  }
  purgeMining(); //purge player exchange limmit
  // updateMarketToday(newMarket); //overwrites entre entery
  // logMarket(newMarket); <- Disabled for superSpeed
}

//for every time something is bought or sold for cash
bool processItemTransaction(market &marketToday, player &playerData, int ID, int quantity){
  if(quantity > 0){ //buying
    //make sure the player has enough things
    int playerStock = getQuantityFromInv(unhashInv(playerData.inventoryHash),ID);
    if(playerStock < quantity){
      if(playerStock != 0){
        quantity = playerStock;
      }
      else{
        return false;
      }
    }
    //market is buying
    //make sure the market has enough money
    float salePrice = marketToday.thing[ID].salePrice * .8; //market buys for 90% of the normal price
    if(marketToday.ballance < (quantity * salePrice)){
      if(marketToday.ballance != 0){//check if the market is crashed. lol
        quantity = floor(marketToday.ballance / salePrice); //adjust the quantity so the market does have enough
      }
      else{
        return false; //quick fail
      }
    }
    //if quantity has not reached 0
    if(quantity != 0){ //buying, quantity is positive
      // Transaction
      //market
      marketToday.ballance -= quantity * salePrice; //positive value
      marketToday.thing[ID].stock += quantity; //positive value
      marketToday.thing[ID].quantityChange += quantity; //positive value
      //player
      playerData.cashBallance += quantity * salePrice; //positive value
      quantity = -quantity;
      addToInventory(playerData.inventoryHash,ID,quantity); //negitive value
      if(quantity != 0){ //feed the negitive quantity back through and put the items back
        //player
        playerData.cashBallance += quantity * salePrice; //negitive value
        //market
        marketToday.ballance -= quantity * salePrice; //negitive value
        marketToday.thing[ID].stock += quantity; //negitive value
        marketToday.thing[ID].quantityChange += quantity; //negitive value
      }
    }
    else{
      return false; //fail a bit later
    }
  }
  else if(quantity < 0){ //selling
    //make sure the market has enough things
    if(marketToday.thing[ID].stock < -(quantity)){ //negitinve since the quantity is negitive
      if(marketToday.thing[ID].stock != 0){ //check if the market has none of the items. lol
        quantity = -marketToday.thing[ID].stock; //negitinve since the quantity is negitive
      }
      else{
        return false; //quick fail
      }
    }
    //make sure the player has enough money
    float salePrice = marketToday.thing[ID].salePrice;
    if(playerData.cashBallance < -(quantity) * salePrice){ //negitinve since the quantity is negitive, and negitve time a negitve equals a positive
      if(playerData.cashBallance != 0){//check if the player is broke. lol
        quantity = -floor(playerData.cashBallance / salePrice); //adjust the quantity so the player does have enough, also negitive
      }
      else{
        return false; //quick fail
      }
    }
    //market is selling
    if(quantity != 0){ //selling, quantity is negitive
      //market
      marketToday.ballance -= quantity * salePrice; // <negitve value
      marketToday.thing[ID].stock += quantity; // <negitve value
      marketToday.thing[ID].quantityChange += quantity; // <negitve value
      //player
      playerData.cashBallance += quantity * salePrice; // <negitve value
      quantity = -quantity;
      addToInventory(playerData.inventoryHash,ID,quantity); // positive value
      if(quantity != 0){ //feed the negitive quantity back through and put the items back
        //player
        playerData.cashBallance += quantity * salePrice; // positive value
        //market
        marketToday.ballance -= quantity * salePrice; // positive value
        marketToday.thing[ID].stock += quantity; // positive value
        marketToday.thing[ID].quantityChange += quantity; // <negitve value
      }
    }
    else{
      return false; //fail a bit later
    }
  }
  else if(quantity == 0){
    return false; //fail really fast
  }
  return false; // i guess if quantity isnt numeric...im just doing this because the compiler yelled at me
}

//print money for emralds
void printCash(market &marketToday, player &playerData, int quantity){
  float priceOfDiamonds = marketToday.thing[20].salePrice; // = the current price of diamonds
  int sourceItems = getQuantityFromInv(unhashInv(playerData.inventoryHash),IDofEmranlds); //number of emralds player has
  int minnedToday = playerData.quantityPrinted; //retrieve from player

  if(sourceItems < quantity){
    quantity = sourceItems;
  }
  if(1000 < minnedToday + quantity){ //limmit player to exchanging 1000 per day
    quantity = 1000 - minnedToday;
  }
  if(quantity <= 0){ //if quantity reaches 0 Transaction iss nullified and failed technically i dont have to return but i do to increace effiecency (i almost forgot quantity can go negitive here)
    return;
  }

  //Proceed to printing!
  marketToday.ballance += quantity * (priceOfDiamonds * .8); //market gets 80% of the cut
  playerData.cashBallance += quantity * (priceOfDiamonds * .2); //player gets 20% of the cut
  marketToday.thing[IDofEmranlds].stock += quantity; //market -hords- aquires emralds x3...
  playerData.quantityPrinted += quantity;

  //void addToInventory(player &playerData, int ID, int &quantity) for reference
  quantity = -quantity; //change to negitive because the quantity is being taken away from player
  addToInventory(playerData.inventoryHash,IDofEmranlds,quantity); //value should go down also no need to make sure inventory runs out because alreaddy checked for that

  //save to logs
  // updateMarketToday(marketToday);
}

//exchange cash for credit not the other way around
void exchangeChash(market &marketToday, player &playerData, float amount){

  float priceOfDiamonds = marketToday.thing[20].salePrice; // = the current price of diamonds
  float sourceBallance = playerData.cashBallance;

  if(sourceBallance < amount){
    amount = sourceBallance;
  }
  if(amount == 0){ //if amount reaches 0 Transaction iss nullified and failed technically i dont have to return but i do to increace effiecency
    return;
  }

  //proceed with exchange
  marketToday.ballance += amount;
  playerData.cashBallance -= amount;
  playerData.creditBallance += amount * (200 / priceOfDiamonds);

  //save to logs
  // updateMarketToday(marketToday);
}

//exchange credit for amo (idk about this one, i might leave amo out compleatly. this is too complicated for me)
void makeAmo(market &marketToday, player &playerData, int quantity){

  float price = 0; // price of amunition....??? change this
  float sourceBallance = playerData.creditBallance;
  float priceOfDiamonds = marketToday.thing[20].salePrice; //the current price of diamonds in case i need it

  if(sourceBallance < quantity * price){
    quantity = sourceBallance / price; // dont worry this would neer execute if price was 0 because of the prier condition 0 is not less than 0, and the source ballance could never go negitive
  }
  if(quantity == 0){ //if quantity reaches 0 Transaction is nullified and failed technically i dont have to return but i do to increace effiecency
    return;
  }

  //proceed with exchange
  playerData.creditBallance -= quantity * price;

  int idofammo = 0; // ID of amunition....????.. change this (air)

  //void addToInventory(player &playerData, int ID, int &quantity) for reference
  addToInventory(playerData.inventoryHash,idofammo,quantity); //value should go up
}

//returns stack limmit for item (probably should be in a config)
int stackLimmit(int ID){
  int table[] = {64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64};
  return table[ID];
}

//verifys the slot parameters and ajdusts quantity if needed, returns false if the slot
bool verifySlot(int &slotID, int &slotQTY, int &quantity, int ID){
  if(slotQTY > stackLimmit(slotID)){ //prevent overflow
    quantity = slotQTY - stackLimmit(slotID); //take back the overflow
    slotQTY = stackLimmit(slotID);
    return false;
  }
  //if slot becomes empty, make it empty
  //and it its air, you can not have more air
  if(slotQTY <= 0 || slotID == 0){
    quantity = slotQTY; //take back the difference if the slot goes negitive
    slotID = 0;
    slotQTY = 0;
  }
  return true;
}

//adds the quantity to the slot that that fits
bool addToSlot(int &slotID, int &slotQTY, int &quantity, int ID){
  // (((quantity > 0) && !(slotQTY > stackLimmit(ID)) && ((slotID == ID) || (slotQTY <= 0))) || ((quantity < 0) && !(slotQTY < 0) && (slotID == ID)))
  // this might be a tough one to explain ughhh hmmm on mabe if i can break this down it will be easyer...
  bool removing = (quantity < 0) , adding = (quantity > 0), empty = (slotQTY <= 0), full = (slotQTY >= stackLimmit(ID)), matches = (slotID == ID);
  //mabe this is a bit easyer to read
  if((adding && !full && (empty || matches)) || (removing && !empty && matches)){ //thats better :D
    //initial assignment
    slotID = ID;
    slotQTY += quantity;
    quantity = 0;
    return verifySlot(slotID,slotQTY,quantity,ID);
  }
  return false;
}

//edits the player inventory to add or remove items from it
void addToInventory(InvHash &inventoryHash, int ID, int &quantity){
  if(quantity != 0){ //skip everything if the quantity is 0 (if the quantity is 0 why would i even call this function in the first place)
    //unpack inventory from hash
    Inventory invData = unhashInv(inventoryHash);
    //for reference (whats inside Inventory)
    // int item[y][x][z]; //item [slot y][slot x][id,quantity]
    // int hotbar[x][z]; //hotbar [slot x][id,quantity] (why did i include this? for continuity i guess)
    for(int j = 0; j < 9; j++){
      for(int i = 0; i < 3; i++){
        //unpack the slot
        int slotID = invData.item[i][j][0];
        int slotQTY = invData.item[i][j][1];
        addToSlot(slotID,slotQTY,quantity,ID);
        // displayInventory(invData,i,j); //im so happy i was smart eough to pass invdata to this function imagin unpacking, repacking and then unpacking again, that would he horrible
        //repack the slot
        invData.item[i][j][0] = slotID;
        invData.item[i][j][1] = slotQTY;
        if(quantity == 0){
          //repack inventory back into hash
          inventoryHash = hashInv(invData);
          return;
        }
      }
      //Hotbar
      //unpack the slot
      int slotID = invData.hotbar[j][0];
      int slotQTY = invData.hotbar[j][1];
      addToSlot(slotID,slotQTY,quantity,ID);
      // displayInventory(invData,-1,j);
      //repack the slot
      invData.hotbar[j][0] = slotID;
      invData.hotbar[j][1] = slotQTY;
      if(quantity == 0){
        //repack inventory back into hash
        inventoryHash = hashInv(invData);
        return;
      }
    }
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
  bool table[numberOfItems] = {1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1};
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
  if(UUID == 0) playerData.UUID = generateUUID();
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
  playerData.quantityPrinted = 0;
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
  fstream readstream;
  readstream.open ("player.bin", ios::binary | ios::in);
  readstream.read(reinterpret_cast <char *> (&player), sizeof(player));
  do{
    index++; //increment index
    readstream.seekg(index * sizeof(player), ios::beg); //GO TO NEXT PLAYER
    readstream.read(reinterpret_cast <char *> (&player), sizeof(player)); //READ NEXT PLAYER
    if(player.UUID == UUID){ //CHECK PLAYER
      UUID = generateUUID(); //hmmmm RECURSION TO THE RESCUE!
    }
  }while(!readstream.fail()); //while not at end of file
  readstream.close(); //close file
  //print results
  return UUID;
}

//retrieve player information out of player database and player ballance log
player retrievePlayer(int UUID, int index){
  // Create our objects.
  player player;
  int count;

  //get number of players
  fstream countstream;
  count = 0;
  countstream.open ("player.bin", ios::binary | ios::in);
  if(countstream.is_open()){
    countstream.seekg(0, ios::end);
    count = countstream.tellg()/sizeof(player); //retuns number of players in file
  }
  countstream.close();

  if(UUID == 0){ // if null player is passed get a specifically indexed player (for loading arrays of players for mass operations)
    if(index < count){ //only try to read an indexed player if the index is valid, meaning not outside the file limmits
      //open readstream
      fstream readstream;
      readstream.open ("player.bin", ios::binary | ios::in); //attempt to open file
      if(readstream.is_open()){ //if the file opened
        readstream.seekg(index * sizeof(player), ios::beg); //go directly to chosen player like a boss
        readstream.read(reinterpret_cast <char *> (&player), sizeof(player)); //read struct
        if(!readstream.fail()){ //only return if read did not fail
          readstream.close();  //close file stream
          return player; //return the player object
        }
      }
    }
    // UUID = generateUUID();  <- not sure to do this or not here
    // if controll reaches this point that means the uuid was 0,
    // and an attempt was made to index a specific player but failed
    // so not to return a null player generate a random UUID and create a new players
    // with the random UUID, but ill be honest, the only time this would execute
    // is when debugging, so not like this would mess with anythinhg by creating random players
    // at least it shoudnt... i hope, there is no reason to index players like this unless im batch grabbing
    // a lot of persistant players to do group operations like simulations... mabe i should return null players, idk
  }
  else{ //get the specific player
    fstream readstream;
    readstream.open ("player.bin", ios::binary | ios::in); //attempt to open file
    if(readstream.is_open()){ //if the file opened
      for (int i = 0; i < count; i++){ //for loop because i know the count
        readstream.seekg(i * sizeof(player), ios::beg); //go to next struct
        readstream.read(reinterpret_cast <char *> (&player), sizeof(player)); //read struct
        if(player.UUID == UUID){ //if the player uuid matches
          readstream.close();  //close file
          return player; //return the player object
        }
      }
      readstream.close();  //close file stream if its open
    }
  }

  // im not sure if this should should be generate or create... ill do create and pass the UUID to it. i guess that seams right
  //if file could not be opened make one
  //basically if annything failed, weather because the index was too high, player was not there
  return createPlayer(UUID);
}

//updates player information in the player database
bool updatePlayer(player playerData){
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
      readstream.seekg(i * sizeof(player), ios::beg); //set position to end
      readstream.read(reinterpret_cast <char *> (&playerTable[i]),sizeof(player));
      if(playerTable[i].UUID == playerData.UUID){
        found = true;
        playerTable[i] = playerData;
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

void displayPlayerLog(int verbosity){
  // Create our objects.
  player temp;
  fstream readstream;
  int index = 0;
  readstream.open ("player.bin", ios::binary | ios::in);
  readstream.read(reinterpret_cast <char *> (&temp), sizeof(temp));
  printLine('-');
  LeftString(returnPlayerHeadder());
  while(!readstream.fail()){
    printLine('-');
    if(verbosity >= 1){
      LeftString(returnPlayerData(temp));
    }
    if(verbosity >= 2){
      displayInventory(unhashInv(temp.inventoryHash),-1,-1);
    }
    //display
    index++; //increment index
    readstream.seekg(index * sizeof(temp), ios::beg); //GO TO NEXT PLAYER
    readstream.read(reinterpret_cast <char *> (&temp), sizeof(temp));
  }
  readstream.close();
}

//purges mining information in player log hopfully this wont create huge 500MB player files for no reason like the last version did
bool purgeMining(){
  // Create our objects.
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

  fstream readstream;
  readstream.open ("player.bin", ios::binary | ios::in | ios::app); //attempt to open file, and in app mode to make it if its not there
  if(readstream.is_open()){ //if the file opened
    player playerTable[count];
    fstream readstream;
    readstream.open ("player.bin", ios::binary | ios::in);
    //build table and check table then modify if found
    for(int i = 0; i < count; i++){
      readstream.seekg(i * sizeof(player), ios::beg); //set position to end
      readstream.read(reinterpret_cast <char *> (&playerTable[i]),sizeof(player));
      playerTable[i].quantityPrinted = 0; //all of this just for this one line
    }
    readstream.close();  //close file stream if its open no more reads
    // Create our objects.
    fstream writestream;
    writestream.open ("player.bin", ios::binary | ios::out); // << this line deletes everything in the file
    if(writestream.is_open()){
      for(int i = 0; i < count; i++){
        writestream.seekg(i * sizeof(player), ios::beg); //set position to index
        writestream.write(reinterpret_cast <char *> (&playerTable[i]),sizeof(player));
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
  return false;
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
  printLong("Merket log could not be found returning blank market");
  return createMarket();
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

// ill probably make this into its own function later then
void displayMarketGraph(int ID, int length){
  // Create our objects.
  fstream countstream;
  int count = 0;
  countstream.open ("market.bin", ios::binary | ios::in);
  if(countstream.is_open()){
    countstream.seekg(0, ios::end);
    count = countstream.tellg()/sizeof(market); //retuns number of players in file
  }
  countstream.close();

  if(count == 0){
    CenterString("there in no market log");
    return;
  }

  //if there is not enough log enteies to satisfy the argument, restrict the graph to the amount there is
  if(length > count){
    length = count;
  }

  //declare table object
  float valueTable[length];

  //build table of the last length market items
  fstream readstream;
  readstream.open ("market.bin", ios::binary | ios::in);
  if(readstream.is_open()){
    int ArrayIndex = 0;
    for(int i = count - 1; i >=  count - length; i--){
      market temp;
      readstream.seekg(i * sizeof(market), ios::beg);
      readstream.read(reinterpret_cast <char *> (&temp), sizeof(market));
      if(!readstream.fail()){
        if(ID != -1){
          valueTable[ArrayIndex] = temp.thing[ID].salePrice;
        }
        else{
          valueTable[ArrayIndex] = temp.ballance;
        }
      }
      else{
        valueTable[ArrayIndex] = 0; //if for whatever reaon there is read error, 0
      }
      ArrayIndex++;
    }
  }
  readstream.close();
  //display the graph :D
  displayGraph(valueTable,length);
}

void displayGraph(float data[], int length){
  // constants
  const int gridWidth = 5, gridHeight = 5, height = 21; //height of the output graph

  //domain restriction
  if(length > display_Width - 12)
  length = display_Width - 12; //if the length is too long limmit it

  //get the limmits and a copy of the data with the search domain of length
  float cealing = *max_element(data,data+length);
  float base = *min_element(data,data+length);

  //if the limmits are the same the graph is imposible, print the single value and return
  if(base == cealing){
    //therefore cealing = celing2 not that i needed to know that or anything
    cout << base;
    return;
  }
  //if the limmits are the same the graph is imposible, print the single value and return
  if(base == cealing){
    // CenterString(String_2Decimals(to_string(base)));
    return;
  }
  //dsiaplay grid sideways with grid lines and axis markers
  for(int j = height - 1; j >= 0; j--){
    //index
    float index = ((j * ((cealing - base)/(height - 1))) + base);

    //if function crosses 0 on an integer j value
    bool zero = (index == 0);

    //do this here since i dont need i for these
    //falls on an extents
    bool top = (j == height - 1);
    bool bottom = (j == 0);
    //if falls on a gridline
    bool horizontal = (j % gridHeight == 0);

    //y axis
    string outstring;
    //conditions where the index should be shown
    if (top || bottom || horizontal || zero)
    outstring.append(bindCenter(String_2Decimals(to_string(index)),10));
    else
    outstring.append(string(10,' '));

    //each line
    for(int i = length - 1; i >= 0; i--){
      int range = (data[i] - base) / (cealing - base) * height; // -1 because the for loop runs through the index numbers of 0 - height -1
      if(j == range) //graph plot
      outstring.push_back('*');
      else{ //grid lines
        //terms

        //falls on an extents
        bool leftSide = (i == length - 1);
        bool right = (i == 0);

        //if falls on a gridline
        bool virtical = (i % gridWidth == 0);

        //grid lines and magor axis
        //major axis
        if (right)
        outstring.push_back('I');
        else if(zero) //compare the index because j will always be 0 on the bottom but the index could be 0 anywhere
        outstring.push_back('=');

        //grid intersections
        else if((top || horizontal || bottom) && (leftSide || virtical))
        outstring.push_back('+');

        //grid lines

        //virtical lines
        else if((leftSide || virtical) && (j % 2 == 0)) //for every other j for dashed lines
        outstring.push_back('|');

        //horizontal lines
        else if((top || horizontal || bottom) && (i % 2 == 0)) //for every other i for dashed lines
        outstring.push_back('-');

        //nothing
        else
        outstring.push_back(' ');
      }

    }
    RightString(outstring);
  }
  //x axis
  string outstring;
  outstring.append(string(10,' '));
  for(int i = length - 1; i >= 0; i--){
    //terms
    //falls on an extents
    bool leftSide = (i == length - 1);
    //if falls on a gridline
    bool virtical = (i % gridWidth == 0);
    //output block
    string block;
    if(virtical || leftSide)
    block.append(to_string(-i));
    else
    block.push_back(' ');
    //append block to outstring
    outstring.append(block);
    i -= (block.length() - 1); //skip the spaces taken up by block
  }
  RightString(outstring);
}

// -2 for just the ballance, -1 for full log, and some number for that item
void displayMarketLog(int ID){
  // Create our objects.
  market temp;
  fstream readstream;
  int index = 0;
  readstream.open ("market.bin", ios::binary | ios::in);
  if(ID >= 0) displayMarketHeadder();
  readstream.read(reinterpret_cast <char *> (&temp), sizeof(market));
  while(!readstream.fail()){
    if(ID >= 0) LeftString(getItemData(temp.thing[ID],-1,-1,-1));
    else if(ID == -1){
      LeftString(build("Day :",index + 1,build(" market ballance :",temp.ballance," cash"))); //probably one of the coolest lines ive ever written i didnt know build was so awesome
    }
    else{
      displayMarketHeadder();
      displayMarketData(temp,-1,-1);
    }
    //display
    index++; //increment index
    readstream.seekg(index * sizeof(market), ios::beg); //GO TO NEXT Market log entery
    readstream.read(reinterpret_cast <char *> (&temp), sizeof(market));
  }
  readstream.close();
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
  returnstring.append(bindCenter(to_string(playerData.quantityPrinted),11));
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
    if(arg[i] == '.' && arg[i + 1] == '0' && arg[i + 2] == '0')
    arg.erase(i,arg.length());
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
      max = strArray[i].length() + 2; //binding area with extera space
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
      outString.append(bindLeft(strArray[j],max));
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
      outString.append(bindLeft(strArray[j],max));
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
