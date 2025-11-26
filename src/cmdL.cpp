#include <iostream>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <string>
#include <sstream>
#include <vector>
#include <map>
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

enum class TokenType {
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COLON, COMMA,
    STRING, NUMBER,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
};

enum class valueType {
    STRING,
    NUMBER,
    OBJECT,
    ARRAY,
    NONE
};

struct JSONValue {
    valueType type;
    std::string stringValue;
    std::vector<JSONValue> arrayValue;
    std::map<std::string, JSONValue> objectValue;
};

class Tokenizer
{
    private: 
    public:
};

class Parser
{
    private:
    public:
};

string parseJsonFiles(ifstream &file1, ofstream &file2)
{
    // Placeholder implementation
    oss << "JSON parsing not yet implemented.\n";
    cout << oss.str();
    return oss.str();
    // maybe use classes for tokenizer and parser?
    // or could I use a series of functions?
    // Note for later
    // What might work best is to have the tokenizer class produce a vector of tokens
    // Then have the parser class take that vector and produce a JSONValue object
    // Finally have a function that takes a JSONValue object and serializes it to the output file
    // This would separate concerns nicely
    // This should work.
}

void clearOss()
{
    oss.str(""); // clear the stringstream
    oss.clear(); // clear any error flags
}