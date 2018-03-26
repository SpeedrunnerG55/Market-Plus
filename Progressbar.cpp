#include <iostream> //for console io
#include <fstream>  //for file io

//for delays
#include <chrono>
#include <thread>

using namespace std;

/*  pauses execution for x milliseconds  */
void sleep(int delay){
  this_thread::sleep_for(chrono::milliseconds(delay));
}

void Progress(int current, int total){
  cout << '[' << string(current + 1,'#') << string(total - (current + 1),' ') << ']' << '\r';
}

int main(){
  int i = 0;
  while(true){
    i++;
    i %= 10;
    Progress(i,10);
  }
  return 0;
}
