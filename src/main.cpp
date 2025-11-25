#include "cmdL.hpp"
#include <iostream>
#include <fstream>
#include <limits>
#include <numeric>
#include <sstream>
using namespace std;

int main(int argc, char* argv[])
{
    cout << "Command Line Tool Executed" << endl;
    cout << "Number of arguments: " << argc << endl;
    for (int i = 1; i < argc; ++i)
    {
        cout << "Argument " << i << ": " << *argv[i] << endl;
    }
    return 0;
}