#include "cmdL.hpp"
#include <iostream>
#include <fstream>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstring>
using namespace std;
enum CommandLineOptions
{
    h, // help
    c, // count bytes
    l, // count lines
    w, // count words
};

int main(int argc, char* argv[])
{
    int offset = 1; // offset to get to the argument value
    ifstream inFile;
    for (int i = 1; i < argc; ++i)
    {
        if (*argv[i] == '-') // if option flag given
        {
            char option = tolower(argv[i][1]); // force to lowercase
            switch (option)
            {
            case 'h':
                displayHelp();
                clearOss();
                break;
            case 'c':
                // 1. open file at given path
                inFile.open(argv[i + offset]);

                // 2. check if it opened correctly
                if (!inFile) {
                    // output error message from errno
                    // Thank god this exists. 
                    // I did NOT want to make my own error table with function calls for each one lol
                    cerr << strerror(errno) << endl;

                    // output error 
                    cerr << "Invalid file path: " << argv[i + offset] << endl;

                    cerr.clear(); // clear error state
                    return 1; // exit with error code
                    // TODO: Find some sort of test cases for this.
                }

                // 3. variable to hold byte count, I wanted this inline but... I guess you can't
                // and I want to potentially do multiple operations later
                int byteCount;

                // 4. Call relevant function (here it's countBytes for returning int byte count)
                byteCount = countBytes(inFile);

                // 5. output result
                cout << "Byte count: " << byteCount << endl;

                // 6. close file
                inFile.close();

                // return number of bytes as output for potential scripting use?
                // return byteCount;
                break;
            case 'l':
                // same steps as above, we are outputting and int.
                inFile.open(argv[i + offset]);
                if (!inFile) {
                    cerr << strerror(errno) << endl;
                    // output error 
                    cerr << "Invalid file path: " << argv[i + offset] << endl;
                    cerr.clear(); // clear error state
                    return 1; // exit with error code
                }
                int lineCount;
                lineCount = countLines(inFile);
                cout << "Line count: " << lineCount << endl;
                inFile.close();
                break;
            case 'w':
                // count words
                cout << "Count words option selected." << endl;
                // oss << string(argv[i + offset]) << endl;
                // cout << oss.str(); // debug
                break;
            default:
                // unknown option
                cout << "Unknown option: " << option << endl;
                // oss << string(argv[i + offset]) << endl;
                // cout << oss.str(); // debug
                break;
            }
            i += offset; // skip next argument as it's a value
            // oss.str(""); // clear the stringstream for next use
            // oss.clear(); // clear any error flags
        }
        else
        {
            cout << "Non-option argument: " << argv[i] << endl;
            // oss << string(argv[i]) << endl;
        }

    }
    return 0;
}