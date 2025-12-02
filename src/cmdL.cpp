#include "cmdL.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

string displayHelp() {
    ostringstream oss;
    oss << "Command Line Tool Help Information\n"
        << "Usage: bash test.sh [options] [filename]\n"
        << "Options:\n"
        << "  -h        Display this help information\n"
        << "  -c        Count bytes in the specified file\n"
        << "  -f        Count characters in the specified file\n"
        << "  -l        Count lines in the specified file\n"
        << "  -w        Count words in the specified file\n"
        << "  -j        Format JSON from input file to output file\n"
        << "Example:\n"
        << "  bash test.sh -c input.txt\n"
        << "  bash test.sh -j ./testing/testFinal.json ./output/outputJSON.json\n";
    cout << oss.str();
    return oss.str();
}

int countBytes(ifstream &inFile) {
    inFile.clear();
    inFile.seekg(0, ios::end);
    return inFile.tellg();
}

int countCharacters(ifstream &inFile) {
    char ch;
    int charCount = 0;
    inFile.clear();
    inFile.seekg(0, ios::beg);
    while (inFile.get(ch)) {
        charCount++;
    }
    return charCount;
}

int countLines(ifstream &inFile) {
    string line;
    int lineCount = 0;
    inFile.clear();
    inFile.seekg(0, ios::beg);
    while (getline(inFile, line)) {
        lineCount++;
    }
    return lineCount;
}

int countWords(ifstream &inFile) {
    string word;
    int wordCount = 0;
    inFile.clear();
    inFile.seekg(0, ios::beg);
    while (inFile >> word) {
        wordCount++;
    }
    return wordCount;
}

void clearOss() {
    ostringstream oss;
    oss.str("");
    oss.clear();
}