#include "cmdL.hpp"
#include "jsonFormatting.hpp"
#include <cctype>
#include <cstring>
#include <iostream>
#include <stdexcept>
using namespace std;

enum CommandLineOptions {
    h,
    c,
    f,
    l,
    w,
    j,
};

int main(int argc, char *argv[]) {
    ifstream inFile;
    for (int i = 1; i < argc; i++) {
        if (*argv[i] == '-') {
            char option = tolower(argv[i][1]);
            switch (option) {
                case 'h': {
                    displayHelp();
                    clearOss();
                    break;
                }
                case 'c': {
                    ifstream inFile;
                    inFile.open(argv[i + 1]);

                    if (!inFile) {
                        cerr << strerror(errno) << endl;

                        cerr << "Invalid file path: " << argv[i + 1] << endl;

                        cerr.clear();
                        return 1;
                    }

                    int byteCount;

                    byteCount = countBytes(inFile);

                    cout << "Byte count: " << byteCount << endl;

                    inFile.close();

                    break;
                }
                case 'f': {
                    ifstream inFile;
                    inFile.open(argv[i + 1]);
                    if (!inFile) {
                        cerr << strerror(errno) << endl;
                        cerr << "Invalid file path: " << argv[i + 1] << endl;
                        cerr.clear();
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
                        cerr << strerror(errno) << endl;
                        cerr << "Invalid file path: " << argv[i + 1] << endl;
                        cerr.clear();
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
                        cerr << strerror(errno) << endl;
                        cerr << "Invalid file path: " << argv[i + 1] << endl;
                        cerr.clear();
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
                        cerr << strerror(errno) << endl;
                        cerr << "Invalid file path: " << argv[i + 1] << endl;
                        cerr.clear();
                        return 1;
                    }
                    if (!file2) {
                        cerr << strerror(errno) << endl;
                        cerr << "Invalid file path: " << argv[i + 2] << endl;
                        cerr.clear();
                        return 1;
                    }
                    try {
                        parseAndWriteJsonFiles(file1, file2);
                    } catch (const runtime_error &e) {
                        file1.close();
                        file2.close();
                        return 1;
                    }
                    file1.close();
                    file2.close();
                    i += 1;
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