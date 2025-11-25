#include "cmdL.hpp"
#include <iostream>
#include <fstream>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <iomanip>
using namespace std;
stringstream oss;
enum CommandLineOptions
{
    h, // help
    c, // count bytes
    l, // count lines
    w, // count words
};

int main(int argc, char* argv[])
{
    cout << "Command Line Tool Executed" << endl; // debug
    cout << "Number of arguments: " << argc << endl; // debug
    int offset = 1; // offset to get to the argument value
    ifstream inFile;
    for (int i = 1; i < argc; ++i)
    {
        // cout << "Argument " << i << ": " << *argv[i] << endl; // debug
        // cout << "Full Argument String " << i << ": " << argv[i] << endl; // debug

        if (*argv[i] == '-') // if option flag given
        {
            char option = tolower(argv[i][1]); // force to lowercase
            cout << "Option detected: " << option << endl; // debug
            switch (option)
            {
            case 'h':
                cout << "Help option selected." << endl;
                oss << string(argv[i + offset]) << endl;
                cout << oss.str(); // debug
                break;
            case 'c':
                cout << "Count bytes option selected." << endl;
                oss << string(argv[i + offset]) << endl;
                // assign to variable
                inFile.open(argv[i + offset]);
                if (!inFile) {
                    cerr << "Error opening file: " << argv[i + offset] << endl;
                    return 1;
                }
                inFile.close();
                cout << oss.str(); // debug
                break;
            case 'l':
                cout << "Count lines option selected." << endl;
                oss << string(argv[i + offset]) << endl;
                cout << oss.str(); // debug
                break;
            case 'w':
                cout << "Count words option selected." << endl;
                oss << string(argv[i + offset]) << endl;
                cout << oss.str(); // debug
                break;
            default:
                cout << "Unknown option: " << option << endl;
                oss << string(argv[i + offset]) << endl;
                cout << oss.str(); // debug
                break;
            }
            i += offset; // skip next argument as it's a value
            oss.str(""); // clear the stringstream for next use
            oss.clear(); // clear any error flags
        }
        else
        {
            cout << "Non-option argument: " << argv[i] << endl;
            oss << string(argv[i]) << endl;
        }

    }
    return 0;
}