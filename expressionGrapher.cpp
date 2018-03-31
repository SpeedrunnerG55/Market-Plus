#include <iostream> //for console io
#include <iomanip>
#include <math.h>
#include <cstring>  //for memset
#include <algorithm> //for min and max

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

//for delays
#include <chrono>
#include <thread>


using namespace std;

//custom graphical functions (this could be a class probably)
const int log_Limmit = 1000;
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
inline string build(string sA, float sB, string sC);
string String_2Decimals(string arg);
string bindCenter(string arg, int length);
string bindRight(string arg, int length);
string bindLeft(string arg, int length);
string select(string arg, int selY, int selX, int posY, int posX);


/*  pauses execution for x milliseconds  */
void sleep(int delay){
  this_thread::sleep_for(chrono::milliseconds(delay));
}

//displays list of expressions with selector
void displayExpressionList(string input[], int sets, int selection){
  printLine('-');
  if(sets == 0){
    printLine('*');
    CenterString("NO DATA");
    printLine('*');
  }
  else{
    string outstring[sets];
    for(int i = 0; i < sets; i++){
      outstring[i].append(to_string(i));
      outstring[i].append(select(String_2Decimals(input[i]),i,-1,selection,-1));
      outstring[i] = bindLeft(outstring[i],11);
    }
    printMultiString(outstring,sizeof(outstring));
  }
}

//for debug, displays a string with a selector
void displayExpression(string arg, unsigned int selection){
  string outstring;
  for(unsigned int i = 0; i < arg.length(); i++){
    if(i == selection){
      outstring.push_back('[');
    }
    else{
      outstring.push_back(' ');
    }
    outstring.push_back(arg[i]);
    if(i == selection){
      outstring.push_back(']');
    }
    else{
      outstring.push_back(' ');
    }
  }
  RightString(outstring);
}

void displayGraph(string data[][log_Limmit], int sets, char label[], int x1, int x2){
  for(int k = 0; k < sets; k++){
    //looks throught the first character of the first expression
    //of each set to see if the expression is malformed
    //if any of them are, the function aborts
    if(data[k][0][0] == '#'){
      printLine('*');
      CenterString("MALFORMED EXPRESSION WILLNOT GRAPH");
      CenterString(build("Set",k+1,""));
      printLine('*');
      return;
    }
  }
  printLine('=');
  CenterString("data Grapher");

  // constants
  const int gridWidth = 5, gridHeight = 4, height = 40; //height of the output graph

  //length of domain
  int domainWidth = x2 - x1;

  domainWidth++; //incluse x2

  printLine('-');
  CenterString(build("domain width ",domainWidth,""));

  if(domainWidth < 0){
    CenterString("what are these negitive domain shinangigans? get real");
    return;
  }

  bool undifined[domainWidth]; //marker to tag a point on the graph as undifined
  memset(undifined,false,sizeof(undifined));//set all of domain to defined unill any of the sets define an x as undifined

  bool found = false; // if the first real number was found

  long double cealing, base; //define base and cealing before loop starts to give them the right scope


  long double numericTable[sets][domainWidth];
  for(int k = 0; k < sets; k++){
    // printLine('-');
    // CenterString(build("Data set ",k,""));
    // displayExpressionList(data[k],domainWidth,-1);
    for(int i = 0; i < domainWidth; i++){
      if(!(i >= log_Limmit)){
        string message;
        switch (data[k][i][0]) { //a 2 dimentional array of strings and i want to acsess the first character of each...
          case '@':
          // message.append(data[k][i]);
          // message.append(" is undifined, will not graph");
          // CenterString(message);
          undifined[i] = true;
          break; //i can not define a vlue for data, it is undifined
          default:
          numericTable[k][i] = stod(data[k][i]);
          if(!found){ //make the limmits of the graph initialy be equal to the first real data point
            cealing = numericTable[k][i];
            base = numericTable[k][i];
          }
          else{
            cealing = max(cealing,numericTable[k][i]);
            base = min(base,numericTable[k][i]);
          }
          found = true;
        }
      }
    }
  }

  if(!found){
    CenterString("can not graph, the entire graph is undinined");
    return;
  }

  //if the limmits are the same the graph is imposible, print the single value and return
  if(base == cealing){
    CenterString(build("the function has a constant value of ",base," over current domain"));
    return;
  }

  //dsiaplay data sideways with grid lines and axis markers
  for(int j = height; j >= 0; j--){
    //index
    long double virticalIndex = ((j * ((cealing - base)/(height))) + base);

    //if function crosses the x axis on an integer j value
    bool Yzero = (virticalIndex == 0);

    //do this here since i dont need i for these
    //falls on an extents
    bool top = (j == height);
    bool bottom = (j == 0);
    //if falls on a gridline
    bool horizontal = (j % gridHeight == 0);

    //create new string object for each line
    string outstring;

    //y axis
    //conditions where the index should be shown
    if (top || bottom || horizontal || Yzero)
    outstring.append(bindCenter(String_2Decimals(to_string(virticalIndex)),11));
    else
    outstring.append(string(11,' '));

    //each line
    for(int i = 0 ; i <= domainWidth - 1; i++){
      int range[sets];
      bool anyRange = false;
      for(int k = 0; k < sets; k++){
        if(!undifined[i] && !(i >= log_Limmit)){
          range[k] = (numericTable[k][i] - base) / (cealing - base) * height;
          if((j >= range[k]) && (j < range[k] + 2)){
            anyRange = true;
            outstring.push_back(label[k]);
            break;
          }
        }
        else{
          anyRange = true;
          outstring.push_back('@');
          break;
        }
      }
      //if the value of anny of the sets are within range of the plot ...
      if(!anyRange && !undifined[i]){ //grid lines
        //terms
        int horizontalIndex = (i + x1);

        //if function crosses the y axis on an integer i value
        bool Xzero = (horizontalIndex == 0);

        //falls on an extents
        bool right = (i == domainWidth - 1);
        bool left = (i == 0);

        //if falls on a gridline
        bool virtical = (i % gridWidth == 0);

        //grid lines and magor axis

        //major axis
        if (Xzero)
        outstring.push_back('I');
        else if(Yzero) //compare the index because j will always be 0 on the bottom but the index could be 0 anywhere
        outstring.push_back('=');

        //corners
        else if((bottom && right) || (top && left)) //bottom right border and top left corners
        outstring.push_back('/');

        //grid intersections
        else if((top || horizontal || bottom) && (left || virtical))
        outstring.push_back('+');

        //grid lines

        //virtical lines
        else if((left || virtical) && (j % 2 == 0)) //for every other j for dashed lines
        outstring.push_back('|');
        //horizontal lines
        else if((top || horizontal || bottom) && (i % 2 == 0)) //for every other i for dashed lines
        outstring.push_back('-');

        //nothing
        else
        outstring.push_back(' ');
      }
    }
    CenterString(outstring);
  }
  //new string object for x axis
  string outstring;
  //x axis
  outstring.append(string(11,' '));
  for(int i = 0 ; i <= domainWidth - 1; i++){
    int horizontalIndex = (i + x1);
    //terms
    //falls on an extents
    bool right = (i == domainWidth - 1);
    bool left = (i == 0);
    //if function crosses the y axis on an integer i value
    bool Xzero = (horizontalIndex == 0);
    //if falls on a gridline
    bool virtical = (i % gridWidth == 0);
    string block;
    if(virtical || right || left || Xzero)
    block.append(to_string((int)horizontalIndex));
    // block.push_back('+');
    else
    block.push_back(' ');

    i += (block.length() - 1); //skip the spaces taken up by block
    outstring.append(block);

  }
  CenterString(outstring);
}



//insets a string inside of another string at he position
string insert(string arg, string arg2, int pos){
  string returnstring;
  int index = 0;
  for(int i = 0; i < pos; i++){
    returnstring.push_back(arg[i]);
    // displayExpression(returnstring,index);
    index++;
  }
  for(unsigned int i = 0; i < arg2.length(); i++){
    returnstring.push_back(arg2[i]);
    // displayExpression(returnstring,index);
    index++;
  }
  for(unsigned int i = pos; i < arg.length(); i++){
    returnstring.push_back(arg[i]);
    // displayExpression(returnstring,index);
    index++;
  }
  return returnstring;
}

//removes spaces from string
string shorten(string arg){
  string returnstring;
  // displayExpression(arg,-1);
  int index = 0;
  for(unsigned int i = 0; i < arg.length(); i++){
    if(arg[i] != ' '){
      returnstring.push_back(arg[i]);
      // displayExpression(arg,i);
      arg[i] = ' ';
      // displayExpression(returnstring,index);
      index++;
    }
  }
  return returnstring;
}

//determins the hierarchy of how the expression is to be evaluated in
string determinOrders(string arg){
  int index = 0;
  int length = arg.length();
  string newOrder;
  for (int i = 0; i < length; i ++){
    if(arg[i] == '('){
      index++;
      newOrder.push_back((char)index);
    }
    else if(arg[i] == ')'){
      newOrder.push_back((char)index);
      index--;
    }
    else{
      newOrder.push_back((char)index);
    }
  }
  return newOrder;
}

//replaces instances of variable with its value
string insertVariable(string arg, char name, long double value){
  for(unsigned int i = 0; i < arg.length(); i++){
    if(arg[i] == 'e'){
      arg[i] = ' ';
      string doublestring = to_string(0.5772156649);
      arg = insert(arg,doublestring,i);
      i += doublestring.length();
    }
    if(arg[i] == 'G'){
      arg[i] = ' ';
      string doublestring = to_string(0.0000000000667408);
      arg = insert(arg,doublestring,i);
      i += doublestring.length();
    }
    if(arg[i] == 'p'){
      arg[i] = ' ';
      string doublestring = to_string(3.14159265358979);
      arg = insert(arg,doublestring,i);
      i += doublestring.length();
    }
    if(arg[i] == name){
      arg[i] = ' ';
      string doublestring = to_string(value);
      doublestring.erase ( doublestring.find_last_not_of('0') + 1, std::string::npos );
      arg = insert(arg,doublestring,i);
      i += doublestring.length();
    }
  }
  return arg;
}

//pushes a character tot he fromt of the string becuase STD dosnt have this
string push_front(string arg, char c){
  string returnstring;
  returnstring.push_back(c);
  for(unsigned int i = 0; i < arg.length(); i++){
    returnstring.push_back(arg[i]);
  }
  return returnstring;
}

//scans for error markers in string
bool scan(string &arg){
  for(unsigned int i = 0; i < arg.length(); i++){
    switch (arg[i]) {
      case '#':
      arg = push_front(arg,'#');
      return false;
      case '@':
      arg = push_front(arg,'@');
      return false;
    }
  }
  return true;
}

//does basic math
string arithmatic(string arg){
  //scan for errors
  if(!scan(arg)){
    return arg; //if bad do nothing
  }

  //determin order to do operations
  string order = determinOrders(arg);
  int maxOrder = 0;
  for(unsigned int j = 0; j < order.length(); j++){
    maxOrder = max((int)order[j],maxOrder);
  }

  while(maxOrder > 1){
    string operation;
    for (unsigned int i = 0; i < arg.length(); i++){
      if((i != 0) && (i != arg.length() -1) && (maxOrder != 1))
      if((int)order[i] == maxOrder){
        operation.push_back(arg[i]);
        arg[i] = ' '; //replace with a space
      }
      string returnstring;
      if((i != 0) && (order[i-1] == maxOrder) && (order[i] == maxOrder - 1)){
        returnstring = arithmatic(operation);
        arg = insert(arg,returnstring,i);
        //this is importiant, if the enclosed expression is
        //long enough that when the entire expresion is contracted,
        //i might be beyond the start of the next one and it wont register
        //so set it back to 0
        i = 0;
        arg = shorten(arg);
        order = determinOrders(arg);
        maxOrder = 0;
        for(unsigned int j = 0; j < order.length(); j++){
          maxOrder = max((int)order[j],maxOrder);
        }
        operation.clear();
      }
    }
  }
  if(!scan(arg)){
    return arg; //if bad do nothing
  }
  string answere;
  string left, right;
  char type = '\0'; //in case a type is not determined
  bool first = true;

  for (unsigned int i = 0; i < arg.length(); i ++){
    switch (arg[i]) {
      case '+': type = '+'; first = false; break;
      case '*': type = '*'; first = false; break;
      case '/': type = '/'; first = false; break;
      case '^': type = '^'; first = false; break;
      case '-':
      if(!((arg[i-1] == '(') || (arg[i-1] == '-') || (arg[i-1] == '+') || (arg[i-1] == '/') || (arg[i-1] == '*'))){
        first = false;
        type = '-';
        break;
      }
      default:
      if((arg[i] != ' ') && (arg[i] != '(') && (arg[i] != ')')){
        if(first){
          left.push_back(arg[i]);
        }
        else{
          right.push_back(arg[i]);
        }
      }
    }
  }
  // displayExpression(arg,-1);
  if((type == '/') && (stod(right) == 0)){
    return push_front(arg,'@');
  }
  else{
    switch (type) {
      case '+': answere = to_string(stod(left) + stod(right)); break;
      case '-': answere = to_string(stod(left) - stod(right)); break;
      case '*': answere = to_string(stod(left) * stod(right)); break;
      case '/': answere = to_string(stod(left) / stod(right)); break;
      case '^': answere = to_string(pow(stod(left),stod(right))); break;
      default: answere = left; //the order of operations function should catch all malformed expressions so, if there is only a left operand return it
    }
    if(answere.length() != 1){
      answere.erase ( answere.find_last_not_of('0') + 1, std::string::npos );
    }
    return answere;
  }
}

//surounds a string with parenthasis
string enclose(string arg){
  string returnstring;
  returnstring.push_back('(');
  for(unsigned int i = 0; i < arg.length(); i++){
    returnstring.push_back(arg[i]);
  }
  returnstring.push_back(')');
  return returnstring;
}

//returns if the character is 0-9 or a decimal point
bool isANumber(char arg){
  return ((arg == '.')||(arg == '0')||(arg == '1')||(arg == '2')||(arg == '3')||(arg == '4')||(arg == '5')||(arg == '6')||(arg == '7')||(arg == '8')||(arg == '9'));
}

//performs opder of operations and puts explicit parenthasius around each operation
string orderOfOperations(string arg){
  if(!scan(arg)){
    return arg; //if bad do nothing
  }
  arg = shorten(arg); //remove spaces

  //scan for and enclose negitive numbers to seperate them from
  for(unsigned int i = 0; i < arg.length(); i++){
    //negitive numbers
    if((arg[i] == '-') && (i != arg.length() - 1) && (isANumber(arg[i+1]))){
      string negitive;
      do{
        negitive.push_back(arg[i]);
        arg[i] = ' ';
        i++;
      }while((i != arg.length()) && (isANumber(arg[i])));
      string enclosed = enclose(negitive);
      arg = insert(arg,enclosed,i);
      arg = shorten(arg);
    }
    //parenthasis
    else if((arg[i] == '-') && (i != arg.length() - 1) && (arg[i+1] == '(')){
      string negitive;
      int count = 0; //number of opened parenthasis that needs to be closed
      do{
        negitive.push_back(arg[i]);
        arg[i] = ' ';
        i++;
        if(arg[i] == '(')
        count++;
        else if(arg[i] == ')')
        count--;
        if((count > 0) && (i == arg.length() - 1)){
          cout << "Negitive Number Exeption" << endl;
          arg = push_front(arg,'#'); //left side could not be found, none of the characters on the left side are numbers
          return arg; // leave the function compleatly, there is nothing i can do the data is bad
        }
      }while(count != 0);
      string enclosed = enclose(negitive);
      arg = insert(arg,enclosed,i);
      arg = shorten(arg);
    }
  }
  char order[5] = {'^','*','/','+','-'};
  for(int i = 0; i < 5; i++){
    for(unsigned int j = 0; j < arg.length(); j++){
      if(arg[j] == order[i]){
        string operation;
        // a - symbol with a '(' to the left of it always means its a negitive symbol, and not a subtraction operation
        if(!((i == 4) && (arg[j-1] == '('))){
          //the position characters are read from, in absolute terms
          unsigned int absolute;

          //save and remove first character at j
          operation.push_back(arg[j]);
          arg[j] = ' ';

          bool left = false, right = false; //tells what each side is finished, do right, set right to true, do left set left to true, exit

          //controll variables for each side (right side first)
          bool parenthasis = false;  //weather or not the current side is a parenthasis group
          int parenthasisopened = 0; //number of parenthasis opened
          bool first = true;         //weather or not the first character was read or not (for non parenthasis groups)
          int relitive = 0;          //the distance relitive to the current j position
          unsigned int last = arg.length() - 1; //what the last term is defined as for the right side

          do{
            //if right side is finished, reset variables for left side
            if(right && (relitive > 0)){
              parenthasis = false;     //weather or not the current side is a parenthasis group
              parenthasisopened = 0;   //number of parenthasis opened
              first = true;            //weather or not the first character was read or not (for non parenthasis groups)
              relitive = 0;            //the distance relitive to the current j position
              last = 0;                //what the last term is defined as for the left side
            }

            //shift to next character
            if(!right)
            relitive++;
            else
            relitive--;
            char current;            //the character at the current position
            absolute = j + relitive; //the absolute position relitive from j
            current = arg[absolute]; //the character at position relitive from j

            //if right side has not finished and current character is '(' a parenthasis has ben opened
            // of if right side has been finished and current character is ')' a parenthasis has ben opened
            if((!right && (current == '(')) || (right && (current == ')'))){
              parenthasisopened++;
              parenthasis = true;
            }

            if(!parenthasis){
              bool currentnum = isANumber(current);
              //if the first character of this side is not a number or an opening parenthasis
              if(!currentnum && first){
                cout << "Non Numeric Operandd Exeption" << endl;
                arg = push_front(arg,'#'); //left side could not be found, none of the characters on the left side are numbers
                return arg; // leave the function compleatly, there is nothing i can do the data is bad
              }
              first = false;

              //only append characters if they are numbers
              if(currentnum){
                //append character to correct side before the side is declared finished, if it is
                if(!right)
                operation.push_back(arg[absolute]);
                else
                operation = push_front(operation,arg[absolute]); //there was no push_front in the std library so i made my own
                arg[absolute] = ' ';
              }

              //if the current character is not a number or the position has reached the end, the side is finished
              if(!currentnum || (absolute == last)){
                if(!right)
                right = true; //mark this side as finished
                else
                left = true; //mark this side as finished
              }
            }
            else{
              //reduce count if a closing parenthasis
              if((!right && (current == ')'))||(right && (current == '(')))
              parenthasisopened--;

              //if reached the end but not all parenthasis where closed
              if((parenthasisopened > 0) && (absolute == last)){
                cout << "unclosed parenthasis Exeption - right" << endl;
                arg = push_front(arg,'#'); //left side could not be found, none of the characters on the left side are numbers
                return arg; // leave the function compleatly, there is nothing i can do the data is bad
              }

              //append character to correct side before the side is declared finished, if it is
              if(!right)
              operation.push_back(arg[absolute]);
              else
              operation = push_front(operation,arg[absolute]); //there was no push_front in the std library so i made my own
              arg[absolute] = ' ';

              //if all parenthasis are closed the side is finished
              if(parenthasisopened == 0){
                if(!right)
                right = true;
                else
                left = true;
              }
            }
          }while(!left); //right gets finished before left and left finished last
          string enclosed = enclose(operation);
          arg = insert(arg,enclosed,j);
          arg = shorten(arg);
          j++;
        }
      }
    }
  }
  return arg;
}



int main(){

  //menue settup
  string title = "Main Menue";
  string discription = "An expression grapher and explorer";
  string options[]= {
    "1 Enter new expression   ",
    "< A D > meve left limmit ",
    "< J L > move right limmit",
    "< F H > move both limmits",
    "^ W S v select expression"
  };

  const int maxSets = 50; //maximum number os sets

  bool mask[maxSets];
  memset(mask,true,sizeof(mask)); //maks all sets untill unmasked
  string input[maxSets]; // input expressions (the actual information to be compiled and graphed each loop)
  char label[maxSets]; //labels for each expression

  // expression selection
  int selection = 0;

  //domain
  int x1 = -30, x2 = 30;  //current domain restriction (not fixed)

  //counters
  int domainIndex;  //a counter for each data point in the domain
  int setIndex; //number of unmakes sets, and a counter for the ones unmasked

  // display menue all in one line :D
  //renaming variable to reduce cunfusion, an array that "carries" the minimun amount of data required to satisfy the graph function
  string carryOnce[maxSets][log_Limmit];
  setIndex = 0; //set set counter to 0
  for(int k = 0; k < maxSets; k++){ //run through the full list grbbing only what i want
    domainIndex = 0; // counter to mesure the size of the domain
    if(!mask[k]){ // determins what sets are masked from the display
      for(int i = x1; i <= x2; i++){
        carryOnce[setIndex][domainIndex] = arithmatic(orderOfOperations(insertVariable(input[k],'x',i)));
        domainIndex++;
      }
      setIndex++;
    }
  }
  displayGraph(carryOnce,setIndex,label,x1,x2);
  displayExpressionList(input,maxSets,selection); //the number of sets that are unmasked
  displayMenue(title,discription,options,sizeof(options));

  bool graph = true;
  bool running = true;
  do{
    if(KeyHit()){
      cout << string(40,'\n');
      char menue = GetChar();
      menue = tolower(menue);
      switch (menue) {

        //quit
        case KEY_ESCAPE: running = false; break;

        //Domain Manipulation
        case 'a': x1--; break;
        case 'd': x1++; break;
        case 'j': x2--; break;
        case 'l': x2++; break;
        case 'f':
        x1--;
        x2--;
        break;
        case 'h':
        x1++;
        x2++;
        break;

        //Expression Manipulation
        case 'w':
        selection++;
        selection %= maxSets;
        break;
        case 's':
        selection--;
        if(selection < 0)
        selection = maxSets -1;
        break;

        //add set
        case '1':
        if(setIndex < 50){
          mask[selection] = false;
          CenterString("Graphing Y = f(x) enter definition for f(x)");
          CenterString("you may use x as a variable.  will graph initially from -30 to 30");
          getline(cin,input[selection]);
          CenterString("enter label for expression");
          cin >> label[selection];
        }
        else{
          CenterString("maximum number of data sets reached, you monster");
        }
        break;

        //remove set
        case '2':
        mask[selection] = true;
        input[selection] = "";
        break;

        default:
        printLine('#');
        printLine(' ');
        CenterString("invalid input");
        string key;
        key.push_back(menue);
        CenterString(key); // << shows incorrect key pressed
      }

      if(graph){
        // display menue all in one line :D
        string carry[maxSets][log_Limmit]; //an array that "carries" the minimun amount of data required to satisfy the graph function
        setIndex = 0; //counter for number of sets grabbed
        for(int k = 0; k < maxSets; k++){ //run through the full list grbbing only what i want
          domainIndex = 0; // counter to mesure the size of the domain
          if(!mask[k]){ //only grab the data if it is unmasked
            for(int i = x1; i <= x2; i++){
              carry[setIndex][domainIndex] = arithmatic(orderOfOperations(insertVariable(input[k],'x',i)));
              domainIndex++;
            }
            setIndex++;
          }
        }
        displayGraph(carry,setIndex,label,x1,x2);
      }
      displayExpressionList(input,maxSets,selection);
      displayMenue(title,discription,options,sizeof(options));
    }
  }while(running);

  getchar();
  return 0;
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
    string outstring;
    int wordlength = 0;
    for(int i = 0; i < display_Width - 4; i++){
      if(pos >= longString.length()){
        writing = false;
      }
      else{
        outstring.push_back(longString[pos]);
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
        outstring.pop_back();
      }
    }
    LeftString(outstring);
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
    string outstring;
    for(int j = 0; j < quantityThatFits; j++){
      outstring.append(bindLeft(strArray[j],max));
    }
    LeftString(outstring);
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
    string outstring;
    for(unsigned int j = 0; j < sizeOfStrArray/sizeof(string); j++){
      outstring.append(bindLeft(strArray[j],max));
    }
    LeftString(outstring);
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
