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

// yes I want these lowercase
enum CommandLineOptions
{
    h, // help
    c, // count bytes
    l, // count lines
    w, // count words
    j, // json parsing between two files
};

int main(int argc, char *argv[])
{
    ifstream inFile;
    // post increment, we are potentially skipping an element by using an offset.
    for (int i = 1; i < argc; i++)
    {
        if (*argv[i] == '-') // Capture option flags
        {
            char option = tolower(argv[i][1]); // force to lowercase
            switch (option)
            {
            case 'h':
            {
                displayHelp();
                clearOss();
                break;
            }
            case 'c':
            {
                // 1. open file at given path
                inFile.open(argv[i + 1]);

                // 2. check if it opened correctly
                if (!inFile)
                {
                    // output error message from errno
                    // Thank god this exists.
                    // I did NOT want to make my own error table with function calls for each one lol
                    cerr << strerror(errno) << endl;

                    // output error
                    cerr << "Invalid file path: " << argv[i + 1] << endl;

                    cerr.clear(); // clear error state
                    return 1;     // exit with error code
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
            }
            case 'l':
            {
                // same steps as above, we are outputting and int.
                inFile.open(argv[i + 1]);
                if (!inFile)
                {
                    cerr << strerror(errno) << endl;
                    // output error
                    cerr << "Invalid file path: " << argv[i + 1] << endl;
                    cerr.clear(); // clear error state
                    return 1;     // exit with error code
                }
                int lineCount;
                lineCount = countLines(inFile);
                cout << "Line count: " << lineCount << endl;
                inFile.close();
                break;
            }
            case 'w':
            {
                // same steps
                inFile.open(argv[i + 1]);
                if (!inFile)
                {
                    cerr << strerror(errno) << endl;
                    // output error
                    cerr << "Invalid file path: " << argv[i + 1] << endl;
                    cerr.clear(); // clear error state
                    return 1;     // exit with error code
                }
                int wordCount;
                wordCount = countWords(inFile);
                cout << "Word count: " << wordCount << endl;
                inFile.close();
                break;
            }
            // json parsing
            case 'j':
            {
                // open both files this time
                ifstream file1;
                ifstream file2;
                file1.open(argv[i + 1]);
                file2.open(argv[i + 2]);
                if (!file1)
                {
                    cerr << strerror(errno) << endl;
                    cerr << "Invalid file path: " << argv[i + 1] << endl;
                    cerr.clear();
                    return 1;
                }
                if (!file2)
                {
                    cerr << strerror(errno) << endl;
                    cerr << "Invalid file path: " << argv[i + 2] << endl;
                    cerr.clear();
                    return 1;
                }
                // call json parsing function with both refs
                parseJsonFiles(file1, file2);
                // close files
                file1.close();
                file2.close();
                i += 1; // increment an additional time as we used two file paths
                // I want to
                break;
            }
            default:
            {
                cout << "Unknown option: " << option << endl;
                break;
            }
            } // end switch
            i += 1; // skip next argument as it's a value
        }
        else
        {
            cout << "Non-option argument: " << argv[i] << endl;
            // oss << string(argv[i]) << endl;
        }
    }
    return 0;
}