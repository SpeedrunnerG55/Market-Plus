#include <iostream> //for console io
#include <iomanip>
#include <math.h>
#include <cstring>  //for memset
#include <algorithm> //for min and max

const int display_Width = 300;

using namespace std;

void displayGraph(float data[][display_Width], int sets, int length){
  // constants
  const int gridWidth = 45, gridHeight = 3, height = 51; //height of the output graph

  //domain restriction
  if(length > display_Width - 12)
  length = display_Width - 12; //if the length is too long limmit it

  //get the minimum and maximum values of each element of each set ...
  // get the max and minimum values of the first set
  float cealing = *max_element(data[0],data[0]+length);
  float base = *min_element(data[0],data[0]+length);
  //get the max and minimum values of the rest of the sets
  for(int i = 1; i < sets; i++){
    //since the max_element wont work on this 2d array,
    //i need to get each max and min value for each on the 1d arrays in the 2d array
    //this would have gotten really messy without min and max
    cealing = max(cealing,*max_element(data[i],data[i]+length));
    base = min(base,*min_element(data[i],data[i]+length));
  }

  //if the limmits are the same the graph is imposible, print the single value and return
  if(base == cealing){
    cout << base;
    return;
  }

  //allow user to expand window if needed
  cout << "width " << display_Width << endl;
  cout << "height " << height << endl;
  getchar();
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
    //conditions where the index should be shown
    if (top || bottom || horizontal || zero){
      cout << setw(10) << index;
      cout << ' ';
    }
    else
    cout << string(11,' ');

    //each line
    for(int i = length - 1; i >= 0; i--){
      int range[sets];
      bool anyRange = false;
      for(int k = 0; k < sets; k++){
        range[k] = (data[k][i] - base) / (cealing - base) * height;
        if(j == range[k]){
          anyRange = true;
          cout << k + 1;
          break;
        }
      }
      //if the value of anny of the sets are within range of the plot ...
      if(!anyRange){ //grid lines
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
        else if(zero) //compare the index because j will always be 0 on the bottom but the index could be 0 anywhere
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
  cout << string(10,' ');
  for(int i = length - 1; i >= 0; i--){
    //terms
    //falls on an extents
    bool leftSide = (i == length - 1);
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
  float data[2][display_Width];
  memset(data,0,sizeof(data));
  for(int i = 0; i < display_Width; i++){
    data[0][i] = - 2 * cos((float)(i * 3.14159265358979 / 180) * 4);
    data[1][i] = -sin((float)(i * 3.14159265358979 / 180) * 4);
  }

  displayGraph(data,2,361);

  return 0;
}
