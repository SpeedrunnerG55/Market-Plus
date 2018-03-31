#include <iostream> //for console io
#include <iomanip>
#include <math.h>
#include <cstring>  //for memset
#include <algorithm> //for min and max

const int display_Width = 60;

using namespace std;

void displayGraph(float data[], int length){
  // constants
  const int gridWidth = 5, gridHeight = 5, height = 40; //height of the output graph

  length++; //include 0

  //domain restriction
  if(length > display_Width - 1)
  length = display_Width - 1; //if the length is too long limmit it

  //get the limmits and a copy of the data with the search domain of length
  float cealing = *max_element(data,data+length);
  float base = *min_element(data,data+length);

  //if the limmits are the same the graph is imposible, print the single value and return
  if(base == cealing){
    //therefore cealing = celing2 not that i needed to know that or anything
    cout << base;
    return;
  }

  //allow user to expand window if needed
  cout << "width " << length << endl;
  cout << "height " << height << endl;
  getchar();

  //dsiaplay grid sideways with grid lines and axis markers
  for(int j = height; j >= 0; j--){

    //index
    float index = ((j * ((cealing - base)/(height))) + base);

    //if function crosses 0 on an integer j value
    bool zero = (index == 0);

    //do this here since i dont need i for these
    //falls on an extents
    bool top = (j == height);
    bool bottom = (j == 0);
    //if falls on a gridline
    bool horizontal = (j % gridHeight == 0);

    //y axis
    //conditions where the index should be shown
    if (top || bottom || horizontal || zero){
      cout << setw(10) << index;
      cout << ' ';
    }
    else
    cout << string(11,' ');

    //each line
    for(int i = length - 1; i >= 0; i--){
      if(j == (int)( (data[i] - base) / (cealing - base) * height) ){ //graph plot
        cout << '*';
      }
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
        cout << 'I';
        else if(index == 0) //compare the index because j will always be 0 on the bottom but the index could be 0 anywhere
        cout << '=';

        //grid intersections
        else if((top || horizontal || bottom) && (leftSide || virtical))
        cout << "+";

        //grid lines

        //virtical lines
        else if((leftSide || virtical) && (j % 2 == 0)) //for every other j for dashed lines
        cout << "|";
        //horizontal lines
        else if((top || horizontal || bottom) && (i % 2 == 0)) //for every other i for dashed lines
        cout << "-";

        //nothing
        else
        cout << " ";
      }
    }
    cout << endl;
  }
  //x axis
  cout << string(11,' ');
  for(int i = length -1; i >= 0; i--){
    //terms
    //falls on an extents
    bool leftSide = (i == length -1);
    //if falls on a gridline
    bool virtical = (i % gridWidth == 0);
    string block;
    if(virtical || leftSide)
    block.append(to_string(-i));
    else
    block.push_back(' ');
    cout << block;
    i -= (block.length() - 1); //skip the spaces taken up by block
  }
  cout << endl;
}



int main(){
  srand(time(NULL));

  float data[display_Width];
  memset(data,0,sizeof(data));
  for(int i = display_Width - 1; i >= 0; i--){
    data[i] = -i;
  }

  displayGraph(data,50);

  return 0;
}
