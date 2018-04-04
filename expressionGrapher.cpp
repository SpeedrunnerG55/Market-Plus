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
int display_Width = 77;
int graph_width = 80;
int graph_height = 40;

void printMultiString(string strArray[],int sizeOfStrArray);
void printLong       (string longString);
void displayMenue    (string title, string discription, string options[],int sizeofOptions);
void printLine       (char weight);
void titleLine       (string str, char weight);
void LeftString      (string str);
void CenterString    (string str);
void RightString     (string str);

//formatting
inline string build    (string sA, float sB, string sC);
string String_2Decimals(string arg);
string bindCenter      (string arg, int length);
string bindRight       (string arg, int length);
string bindLeft        (string arg, int length);
string select          (string arg, int selY, int selX, int posY, int posX);

string orderOfOperations(string arg);
string arithmatic(string arg);

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
      outstring[i].append(to_string(i+1));
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

//insets a string inside of another string at he position
string insert(string arg, string arg2, int pos){
  string returnstring;
  int index = 0;
  for(int i = 0; i < pos; i++){
    returnstring.push_back(arg[i]);
    index++;
  }
  for(unsigned int i = 0; i < arg2.length(); i++){
    returnstring.push_back(arg2[i]);
    index++;
  }
  for(unsigned int i = pos; i < arg.length(); i++){
    returnstring.push_back(arg[i]);
    index++;
  }
  return returnstring;
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

//removes empty space from a data set kinda like compresing a file in memory
void condenser(string arg[], int count){
  string condenced[count];
  int index = 0;
  for(int i = 0; i < count; i ++){
    if(arg[i][0] != '\0'){
      condenced[index] = arg[i];
      index++;
    }
  }
  for(int i = 0; i < count; i ++){
    arg[i] = condenced[i];
  }
}

//displays an expression as a graph
void displayGraph(string input[], int sets, char label[], int x1, int x2, int curserIndex, int selection, int y1, int y2){

  printLine('=');
  CenterString("data Grapher");

  // display menue all in one line :D
  string data[sets][graph_width + 1]; //an array that "carries" the minimun amount of data required to satisfy the graph function

  // constants
  const int gridWidth = 5, gridHeight = 5; //height of the output graph

  if(x2 < x1){
    CenterString("what are these negitive domain shinangigans? get real");
    return;
  }

  bool undifined[graph_width + 1]; //marker to tag a point on the graph as undifined
  memset(undifined,false,sizeof(undifined));//set all of domain to defined unill any of the sets define an x as undifined

  bool found = false; // if the first real number was found

  long double cealing, base; //define base and cealing before loop starts to give them the right scope floor and cealing is global to all sets so the graph is universally scalled

  long double numericTable[sets][graph_width + 1]; //numeric values for each evaluation string

  //build table of undifined x values and get floor and cealing
  for(int k = 0; k < sets; k++){
    for(int i = 0; i <= graph_width; i++){
      long double xValue = (i * ((long double)(x2 - x1)/(graph_width))) + x1;
      data[k][i] = arithmatic(orderOfOperations(insertVariable(input[k],'x',xValue)));
      string message;
      if(data[k][i][0] == '@')
      undifined[i] = true;
      else{
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

  if(!found){
    CenterString("can not graph, the entire graph is undinined");
    return;
  }

  //if the limmits are the same the graph is imposible, print the single value and return
  if(base == cealing){
    CenterString(build("the function has a constant value of ",base," over current domain"));
    return;
  }

  //allow the user to adjust the base and cealing to zoom in on data
  base += y1;
  cealing += y2;

  printLine('-');

  // basic information about the graph
  CenterString("Basic Info");
  CenterString(build("the domain width is ",x2 - x1,build(" from ",x1,build(" to ",x2,""))));
  CenterString(build("the range height is ",cealing - base,build(" from ",base,build(" to ",cealing,""))));
  CenterString(build("for set ",selection + 1,build(" the value at ",(curserIndex * ((long double)(x2 - x1)/(graph_width))) + x1,build(" is ",numericTable[selection][curserIndex],""))));

  printLine('-');

  //dsiaplay data sideways with grid lines and axis markers
  for(int j = graph_height; j >= 0; j--){
    //index
    long double previousyValue = ((long double)((j+1) * ((cealing - base)/(graph_height))) + base);
    long double yValue = ((long double)(j * ((cealing - base)/(graph_height))) + base);
    long double nextyValue = ((long double)((j-1) * ((cealing - base)/(graph_height))) + base);

    //if function crosses the x axis on an integer j value or between it and the next
    bool Yzero = false;
    //test if the value before was greater than 0 and the current value is below 0
    if(j != graph_height)
    Yzero = ((previousyValue > 0) && (yValue <= 0));
    else //prevent going out of bounds, do a different test at the top
    Yzero = ((yValue >= 0) && (nextyValue < 0));

    //weather the function intersetcts the curser line at this line or not
    bool crossing = (j == (int)((numericTable[selection][curserIndex] - base) / (cealing - base) * graph_height));

    //do this here since i dont need i for these
    //falls on an extents
    bool top = (j == graph_height);
    bool bottom = (j == 0);
    //if falls on a gridline
    bool horizontal = (j % gridHeight == 0);

    //create new string object for each line
    string outstring;

    //y axis
    //conditions where the index should be shown
    if(crossing)
    outstring.append(bindCenter(String_2Decimals(to_string(numericTable[selection][curserIndex])),11));

    else if(Yzero)
    outstring.append(bindCenter("0",11));

    else if (top || bottom || horizontal) //acurate for gridlines but not for indexing x values...
    outstring.append(bindCenter(String_2Decimals(to_string(yValue)),11));

    else
    outstring.append(string(11,' '));

    //each line
    for(int i = 0; i <= graph_width; i++){
      int range[sets];
      bool anyRange = false;
      for(int k = 0; k < sets; k++){
        if(!undifined[i] && !(i >= graph_width + 1)){
          range[k] = (numericTable[k][i] - base) / (cealing - base) * graph_height;
          if((j >= range[k]) && (j < range[k] + 1)){
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
        long double previousxValue = ((i-1) * ((long double)(x2 - x1)/(graph_width))) + x1;
        long double xValue = (i * ((long double)(x2 - x1)/(graph_width))) + x1;
        long double nextxValue = ((i+1) * ((long double)(x2 - x1)/(graph_width))) + x1;

        //curser line
        bool curserLine = (curserIndex == i);

        //if function crosses the y axis on an integer i value or between it and the next
        bool Xzero = false;
        //test if the value before was greater than 0 and the current value is below 0
        if(i != graph_width)
        Xzero = ((previousxValue < 0) && (xValue >= 0));
        else //prevent going out of bounds, do a different test at the top
        Xzero = ((xValue <= 0) && (nextxValue > 0));

        //falls on an extents
        bool right = (i == graph_width);
        bool left = (i == 0);

        //if falls on a gridline
        bool virtical = (i % gridWidth == 0);

        //grid lines and magor axis

        //corners
        if((bottom && right) || (top && left)) //bottom right border and top left corners
        outstring.push_back('/');

        else if((bottom && left) || (top && right)) //bottom right border and top left corners
        outstring.push_back((char)92); // the \ character

        //left and right borders
        else if(left || right)
        outstring.push_back('|');

        //left and right borders
        else if(top || bottom)
        outstring.push_back('-');

        //curser lines

        //virtical
        else if(curserLine)
        outstring.push_back('*');
        //horizontal
        else if(crossing)
        outstring.push_back('*');

        //major axis
        else if (Xzero)
        outstring.push_back('I'); //a vertical equals sign here would have been perfect but oh well
        else if(Yzero) //compare the index because j will always be 0 on the bottom but the index could be 0 anywhere
        outstring.push_back('=');

        //grid intersections
        else if(horizontal && virtical)
        outstring.push_back('+');

        //grid lines

        //virtical lines
        else if(virtical && (j % 2 == 0)) //for every other j for dashed lines
        outstring.push_back('|');

        //horizontal lines
        else if(horizontal && (i % 2 == 0)) //for every other i for dashed lines
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
  for(int i = 0; i <= graph_width; i++){
    long double previousxValue = ((i-1) * ((long double)(x2 - x1)/(graph_width))) + x1;
    long double xValue = (i * ((long double)(x2 - x1)/(graph_width))) + x1;
    long double nextxValue = ((i+1) * ((long double)(x2 - x1)/(graph_width))) + x1;
    //terms
    //falls on an extents
    bool right = (i == graph_width);
    bool left = (i == 0);
    //curser line
    bool curserLine = (curserIndex == i);
    //if function crosses the y axis on an integer i value
    // bool Xzero = (xValue == 0);

    bool Xzero = false;
    //test if the value before was greater than 0 and the current value is below 0
    if(i != graph_width)
    Xzero = ((previousxValue < 0) && (xValue >= 0));
    else //prevent going out of bounds, do a different test at the top
    Xzero = ((xValue <= 0) && (nextxValue > 0));

    //if falls on a gridline
    bool virtical = (i % gridWidth == 0);
    string block;
    if( Xzero )
    block.push_back('0');
    else if( virtical || right || left || curserLine )
    block.append(to_string((int)xValue));
    // block.push_back('+');
    else
    block.push_back(' ');

    i += (block.length() - 1); //skip the spaces taken up by block
    outstring.append(block);
  }
  CenterString(outstring);
}

//removes spaces from string
string shorten(string arg){
  string returnstring;
  int index = 0;
  for(unsigned int i = 0; i < arg.length(); i++){
    if(arg[i] != ' '){
      returnstring.push_back(arg[i]);
      arg[i] = ' ';
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

//returns if the character is 0-9 or a decimal point
bool isANumber(char arg){
  return ((arg == 'S')||(arg == 'C')||(arg == 'T')||(arg == '.')||(arg == '0')||(arg == '1')||(arg == '2')||(arg == '3')||(arg == '4')||(arg == '5')||(arg == '6')||(arg == '7')||(arg == '8')||(arg == '9'));
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
      case '^': type = '^'; first = false; break;
      case '*': type = '*'; first = false; break;
      case '/': type = '/'; first = false; break;
      case '%': type = '%'; first = false; break;
      case '+': type = '+'; first = false; break;

      //trig functions
      case 'S': type = 'S'; break;
      case 'C': type = 'C'; break;
      case 'T': type = 'T'; break;

      case '-':
      if( (arg[i-1] != '(') && !(isANumber(arg[i-1]))){
        first = false;
        type = '-';
        break;
      } //last is this so if its not a subtraction operation its a negative *Number* no NO BREAKS!
      default:
      if((arg[i] != '(') && (arg[i] != ')')){
        if(first){
          left.push_back(arg[i]);
        }
        else{
          right.push_back(arg[i]);
        }
      }
    }
  }
  if(((type == '/') || (type == '%')) && (stod(right) == 0)){
    return push_front(arg,'@');
  }
  else{
    switch (type) {
      //arithmatic operations
      case '^': answere = to_string(pow(stod(left),stod(right))); break;
      case '*': answere = to_string(stod(left) * stod(right)); break;
      case '/': answere = to_string(stod(left) / stod(right)); break;
      case '%': answere = to_string(stoi(left) % stoi(right)); break;
      case '+': answere = to_string(stod(left) + stod(right)); break;
      case '-': answere = to_string(stod(left) - stod(right)); break;
      //trig operations (also unary type operations)
      case 'S': answere = to_string(sin(stod(left))); break;
      case 'C': answere = to_string(cos(stod(left))); break;
      case 'T': answere = to_string(tan(stod(left))); break;
      //if there is no operation the value is just a number
      default: answere = left; //the order of operations function should catch all malformed expressions so, if there is only a left operand return it
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

//scan for and enclose negitive numbers to seperate them from subtraction operations
string scanForNegatives(string arg){
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
  return arg;
}

string scanForTrigFunctions(string arg){
  for(unsigned int i = 0; i < arg.length(); i++){
    bool trigPrefix = ((arg[i] == 'S')||(arg[i] == 'C')||(arg[i] == 'T')); //sin cos tan
    //parenthasis
    if(trigPrefix && (i != arg.length() - 1) && (arg[i+1] == '(')){
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
          cout << "trig Exeption" << endl;
          arg = push_front(arg,'#'); //left side could not be found, none of the characters on the left side are numbers
          return arg; // leave the function compleatly, there is nothing i can do the data is bad
        }
      }while(count != 0);
      string enclosed = enclose(negitive);
      arg = insert(arg,enclosed,i);
      arg = shorten(arg);
    }
  }
  return arg;
}

//performs opder of operations and puts explicit parenthasius around each operation
string orderOfOperations(string arg){
  if(!scan(arg)){
    return arg; //if bad do nothing
  }
  arg = shorten(arg); //remove spaces

  //scan for and enclose negitive numbers
  arg = scanForNegatives(arg);

  //scan for trif functions
  arg = scanForTrigFunctions(arg);

  char order[6] = {'^','*','/','%','+','-'};
  for(int i = 0; i < 5; i++){
    for(unsigned int j = 0; j < arg.length(); j++){
      if(arg[j] == order[i]){
        string operation;
        // a - symbol with a '(' to the left of it always means its a negitive symbol, and not a subtraction operation
        if(!((i == 5) && (arg[j-1] == '('))){
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
    " 1 Enter new expression        ",
    " 2 remove selected expression  ",
    " 3 Reset range ",
    " 4 normalize domain ",
    " a d move left domain limmit   ",
    " j l move right domain limmit  ",
    " A D move both domain limmit   ",
    " w s move lower range limit    ",
    " i k move upper rannge limmit  ",
    " W S move both range limmit    ",
    " t g move selection            ",
    " f h move curser left and right"
  };

  const int maxSets = 50; //maximum number os sets

  string input[maxSets]; // input expressions (the actual information to be compiled and graphed each loop)
  char label[maxSets]; //labels for each expression

  // expression selection
  int selection = 0;

  //domain
  int x1 = -30, x2 = 30;  //current domain restriction (not fixed)

  //range
  int y1 = 0, y2 = 0;

  //curser
  int curser = 0;

  //counters
  int sets = 0; //current number of sets

  //display the menue
  displayMenue(title,discription,options,sizeof(options));

  bool graph = false;
  bool running = true;
  do{
    if(KeyHit()){
      cout << string(40,'\n');
      char menue = GetChar();
      switch (menue) {

        //quit
        case KEY_ESCAPE: running = false; break;

        //Expression Manipulation
        case 't':
        if(sets > 0){
          selection++;
          selection %= sets;
        }
        break;
        case 'g':
        if(sets > 0){
          selection--;
          if(selection < 0)
          selection = sets -1;
        }
        break;

        //Domain Manipulation
        case 'd': x1++; break;
        case 'l': x2++; break;
        case 'a': x1--; break;
        case 'j': x2--; break;
        case 'D':
        x1++;
        x2++;
        break;
        case 'A':
        x1--;
        x2--;
        break;

        //curser
        //mmove curser left
        case 'f':
        curser--;
        break;
        //move curser right
        case 'h':
        curser++;
        break;

        //dimain limmit adjustment
        case 'w': y1++; break;
        case 'i': y2++; break;
        case 's': y1--; break;
        case 'k': y2--; break;
        case 'W':
        y1++;
        y2++;
        break;
        case 'S':
        y1--;
        y2--;
        break;

        //add set
        case '1':
        if(sets < 50){
          CenterString("Graphing Y = f(x) enter definition for f(x)");
          CenterString("you may use x as a variable.  will graph initially from -30 to 30");
          getline(cin,input[sets]);
          if(input[sets][0] == '\0'){
            CenterString("Empty expression detected, aborting");
            break;
          }
          else if(orderOfOperations(insertVariable(input[sets],'x',1))[0] == '#'){
            CenterString("Malformed expression");
            break;
          }

          CenterString("enter label for expression");
          cin >> label[sets];
          graph = true;
          sets++;
        }
        else{
          CenterString("maximum number of data sets reached, you monster");
        }
        break;

        //remove set
        case '2':
        if(sets > 0){
          input[selection] = "";
          sets--;
          if(sets == 0){
            graph = false;
          }
        }
        break;

        //reset anyRange
        case '3':
        y1 = 0;
        y2 = 0;
        break;

        //normalise domain
        case '4':
        x1 = -30;
        x2 = 30;
        break;

        default:
        printLine('#');
        printLine(' ');
        CenterString("invalid input");
        string key;
        key.push_back(menue);
        CenterString(key); // << shows incorrect key pressed
      }

      if(curser > graph_width)
      curser = 0;
      if(curser < 0)
      curser = graph_width;

      if(graph){

        condenser(input,maxSets); //condense the data to the bigining of the object space

        //for reference
        // void displayGraph(string data[][log_Limmit], int sets, char label[], int x1, int x2, int curser, int selection, int lim1, int lim2){
        displayGraph(input,sets,label,x1,x2,curser,selection,y1,y2);
      }
      CenterString(build("current numer of expressions :",sets,""));
      displayExpressionList(input,sets,selection);
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
    returnstring.append(string(length / 2 - str_length,' '));
    returnstring.append(arg);
    if(arg.length() % 2)
    returnstring.append(string(length / 2 - str_length - 1,' '));
    else
    returnstring.append(string(length / 2 - str_length,' '));
    if(length % 2 != 0)
    returnstring.push_back(' ');
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
  if((max + 5) * sizeOfStrArray/sizeof(string) > (unsigned) display_Width){
    int quantityThatFits = 0;
    for(int i = (sizeOfStrArray/sizeof(string)) - 1; i > 0; i--){
      if((max + 5) * i > (unsigned) display_Width){
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
