#include "cmdL.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

string displayHelp() {
    // build help string
    ostringstream oss;
    // help information
    oss << "Command Line Tool Help Information\n"
        << "Usage: bash test.sh [options] [filename]\n"
        << "Options:\n"
        << "  -h        Display this help information\n"
        << "  -c        Count bytes in the specified file\n"
        << "  -f        Count characters in the specified file\n"
        << "  -l        Count lines in the specified file\n"
        << "  -w        Count words in the specified file\n"
        << "  -j        Format JSON from input file to output file\n"
        << "  -t        Run JSON parsing tests on files in /testing folder\n"
        << "Example:\n"
        << "  bash test.sh -c input.txt\n"
        << "  bash test.sh -j ./testing/testFinal.json ./output/outputJSON.json\n";
    // output help string to console
    cout << oss.str();
    // return help string
    return oss.str();
}

int countBytes(ifstream &inFile) {
    // get current position
    inFile.clear();
    // seek to end
    inFile.seekg(0, ios::end);
    // return position as byte count
    return inFile.tellg();
}

int countCharacters(ifstream &inFile) {
    // initialize character count
    char ch;
    // initialize character count variable
    int charCount = 0;
    // reset file stream state and position
    inFile.clear();
    inFile.seekg(0, ios::beg);
    // read through file character by character
    while (inFile.get(ch)) {
        charCount++;
    }
    // return character count
    return charCount;
}

int countLines(ifstream &inFile) {
    // initialize line count variable
    string line;
    // initialize line count variable
    int lineCount = 0;
    // reset file stream state and position
    inFile.clear();
    inFile.seekg(0, ios::beg);
    // read through file line by line
    while (getline(inFile, line)) {
        lineCount++;
    }
    // return line count
    return lineCount;
}

int countWords(ifstream &inFile) {
    // initialize word count variable
    string word;
    // initialize word count variable
    int wordCount = 0;
    // reset file stream state and position
    inFile.clear();
    inFile.seekg(0, ios::beg);
    // read through file word by word
    while (inFile >> word) {
        wordCount++;
    }
    // return word count
    return wordCount;
}

void clearOss() {
    // clear ostringstream
    ostringstream oss;
    // reset contents
    oss.str("");
    // clear flags
    oss.clear();
}