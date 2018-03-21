#include <iostream>

//for delays
#include <chrono>
#include <thread>

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

using namespace std;

/*  pauses execution for x milliseconds  */
void sleep(int delay){
  this_thread::sleep_for(chrono::milliseconds(delay));
}

int main(){

  //display menue all in one line :D

  bool running = true;
  do{
    if(KeyHit()){
      GetChar();
      for(int i = 0; i < 20; i++){
        cout << i;
        sleep(20);
      }
    }
  }while(running);
  return 0;
}
