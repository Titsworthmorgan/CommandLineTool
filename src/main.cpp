#include "cmdL.hpp"
#include "jsonFormatting.hpp"
#include <cctype>
#include <cstring>
#include <iostream>
#include <stdexcept>
using namespace std;

// take arg count and arg vector
int main(int argc, char *argv[]) {
    // create input file stream
    ifstream inFile;
    // process each argument
    for (int i = 1; i < argc; i++) {
        // check for option flag
        if (*argv[i] == '-') {
            // get option character
            char option = tolower(argv[i][1]);
            /*
                Switch on option character:
                    - get file path from next argument
                    - open file
                    - check for errors
                    - call appropriate function
                    - output result
                    - close file
            */
            switch (option) {
                case 'h': {
                    // self explanatory here
                    displayHelp();
                    // probably not needed but oh well
                    clearOss();
                    break;
                }
                case 'c': {
                    // grab file path from next argument
                    ifstream inFile;
                    // open the file
                    inFile.open(argv[i + 1]);
                    // check for file open errors
                    if (!inFile) {
                        throw runtime_error("Invalid file path: " + std::string(argv[i + 1]));
                        return 1;
                    }
                    // byte count variable
                    int byteCount;
                    // call byte count function
                    byteCount = countBytes(inFile);
                    // output byte count
                    cout << "Byte count: " << byteCount << endl;
                    // close the file
                    inFile.close();

                    break;
                }
                case 'f': {
                    // same thing
                    ifstream inFile;
                    inFile.open(argv[i + 1]);
                    if (!inFile) {
                        throw runtime_error("Invalid file path: " + std::string(argv[i + 1]));
                        return 1;
                    }
                    int charCount;
                    charCount = countCharacters(inFile);
                    cout << "Character count: " << charCount << endl;
                    inFile.close();
                    break;
                }
                case 'l': {
                    ifstream inFile;
                    inFile.open(argv[i + 1]);
                    if (!inFile) {
                        throw runtime_error("Invalid file path: " + std::string(argv[i + 1]));
                        return 1;
                    }
                    int lineCount;
                    lineCount = countLines(inFile);
                    cout << "Line count: " << lineCount << endl;
                    inFile.close();
                    break;
                }
                case 'w': {
                    inFile.open(argv[i + 1]);
                    if (!inFile) {
                        throw runtime_error("Invalid file path: " + std::string(argv[i + 1]));
                        return 1;
                    }
                    int wordCount;
                    wordCount = countWords(inFile);
                    cout << "Word count: " << wordCount << endl;
                    inFile.close();
                    break;
                }
                case 'j': {
                    ifstream file1;
                    ofstream file2;
                    file1.open(argv[i + 1]);
                    file2.open(argv[i + 2]);
                    if (!file1) {
                        throw runtime_error("Invalid file path: " + std::string(argv[i + 1]));
                        return 1;
                    }
                    if (!file2) {
                        throw runtime_error("Invalid file path: " + std::string(argv[i + 2]));
                        cerr.clear();
                        return 1;
                    }
                    try {
                        parseAndWriteJsonFiles(file1, file2);
                    } catch (const runtime_error &e) {
                        throw runtime_error("JSON Parsing/Formatting Error: " + std::string(e.what()));
                        file1.close();
                        file2.close();
                        return 1;
                    }
                    file1.close();
                    file2.close();
                    i += 1;
                    break;
                }
                case 't': {
                    // test function for JSON parsing
                    /*
                        Some things that do NOT work, and I couldnt figure out:
                        - e numbers (1e10, -2.5e-3) or exponential notation
                        - array objects with duplicate keys (e.g., [{"id":1,"name":"A"},{"id":1,"name":"B"}])
                        These should be valid if in an array. But my duplicate key check flags them.
                    */
                    testJsonParsing();
                    break;
                }
                case 'a': {
                    // all of the above counts in a single run
                    ifstream inFile;
                    inFile.open(argv[i + 1]);
                    if (!inFile) {
                        throw runtime_error("Invalid file path: " + std::string(argv[i + 1]));
                        return 1;
                    }
                    int byteCount = countBytes(inFile);
                    inFile.clear();
                    inFile.seekg(0, ios::beg);
                    int charCount = countCharacters(inFile);
                    inFile.clear();
                    inFile.seekg(0, ios::beg);
                    int lineCount = countLines(inFile);
                    inFile.clear();
                    inFile.seekg(0, ios::beg);
                    int wordCount = countWords(inFile);
                    cout << "Byte count: " << byteCount << endl;
                    cout << "Character count: " << charCount << endl;
                    cout << "Word count: " << wordCount << endl;
                    cout << "Line count: " << lineCount << endl;
                    inFile.close();
                    break;
                }
                default: {
                    cout << "Unknown option: " << option << endl;
                    break;
                }
            }
            i += 1;
        } else {
            cout << "Non-option argument: " << argv[i] << endl;
        }
    }
    return 0;
}