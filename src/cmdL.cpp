#include <iostream>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <string>
#include <sstream>
#include "cmdL.hpp"
using namespace std;
ostringstream oss;

string displayHelp()
{
    oss << "Command Line Tool Help Information\n"
        << "Usage: bash test.sh [options] [filename]\n"
        << "Options:\n"
        << "  -h        Display this help information\n"
        << "  -c        Count bytes in the specified file\n"
        << "  -l        Count lines in the specified file\n"
        << "  -w        Count words in the specified file\n";
    cout << oss.str(); // display help
    return oss.str();
}

int countBytes(ifstream &inFile)
{
    // clear any eof flags
    inFile.clear();

    // skip to end
    inFile.seekg(0, ios::end);
    
    // return position as byte count
    return inFile.tellg();
}

int countLines(ifstream &inFile)
{
    // line variable
    string line;

    // counter
    int lineCount = 0;

    // clear any eof flags
    inFile.clear();

    // seek to beginning
    inFile.seekg(0, ios::beg);

    // while line exists, increment count
    while (getline(inFile, line))
    {
        lineCount++;
    }

    // return count
    return lineCount;
}

int countWords(ifstream &inFile)
{
    // word variable to hold each word
    // this will be delimited by whitespace by default.
    // really we're just taking advantage of the stream extraction operator here and its default behavior.
    string word;

    // counter
    int wordCount = 0;

    // clear any eof flags
    inFile.clear();

    // seek to beginning
    inFile.seekg(0, ios::beg);

    // while word exists, increment count
    while (inFile >> word)
    {
        wordCount++;
    }

    // return count
    return wordCount;
}

string parseJsonFiles(ifstream &file1, ifstream &file2)
{
    // Placeholder function for JSON parsing between two files
    // Actual implementation would depend on the specific requirements and JSON library used
    oss << "JSON parsing between two files is not yet implemented." << endl;
    cout << oss.str();
    return oss.str();
}
void clearOss()
{
    oss.str(""); // clear the stringstream
    oss.clear(); // clear any error flags
}