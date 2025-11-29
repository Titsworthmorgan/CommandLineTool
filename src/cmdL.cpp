#include "cmdL.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
ostringstream oss;

class Tokenizer {
  private:
    // Reference to input file stream
    ifstream &inFile;

  public:
    // Enum to represent different types of tokens
    enum class TokenType {
        LEFT_BRACKET,
        RIGHT_BRACKET,
        COLON,
        COMMA,
        STRING,
        END_OF_FILE,
        UNKNOWN
    };

    // Struct to represent a token
    struct Token {
        TokenType type;
        std::string value;
    };

    // Constructor - initializes with input file stream
    Tokenizer(ifstream &file) : inFile(file) {}

    // Function to get the next token from the input stream
    Token getNextToken() {
        char ch;
        // Skip whitespace
        while (inFile.get(ch)) {
            if (!isspace(ch)) {
                break;
            }
        }
        // Check for end of file
        if (inFile.eof()) {
            // return empty string token
            return {TokenType::END_OF_FILE, ""};
        }
        // Identify token type
        switch (ch) {
            case '{':
                return {TokenType::LEFT_BRACKET, "{"};
            case '}':
                return {TokenType::RIGHT_BRACKET, "}"};
            case ',':
                return {TokenType::COMMA, ","};
            case ':':
                return {TokenType::COLON, ":"};
            case '"': {
                std::string strValue;
                while (inFile.get(ch) && ch != '"') {
                    strValue += ch;
                }
                return {TokenType::STRING, strValue};
            }
            default:
                return {TokenType::UNKNOWN, std::string(1, ch)};
        }
    }
};

class Parser {
  private:
    // Tokenizer instance
    Tokenizer tokenizer;

  public:
    // Enum to represent the type of JSON value
    enum class valueType {
        STRING,
        UNKNOWN,
    };

    // Struct to represent a JSON value
    struct JSONValue {
        valueType type;
        std::string stringValue;
    };
    // Constructor - initializes with input file stream
    Parser(ifstream &file) : tokenizer(file) {}
    // Function to parse JSON and return a map of key-value pairs
    map<string, JSONValue> parse() {
        // Map to hold key-value pairs
        map<string, JSONValue> jsonMap;
        // Get the first token
        Tokenizer::Token token = tokenizer.getNextToken();
        // Expecting a left bracket to start the JSON object
        if (token.type != Tokenizer::TokenType::LEFT_BRACKET) {
            throw runtime_error("Expected '{' at the beginning of JSON object");
        }
        // Loop to parse key-value pairs
        while (true) {
            // Get key
            token = tokenizer.getNextToken();
            // Check for end of object
            if (token.type == Tokenizer::TokenType::RIGHT_BRACKET) {
                break; // End of JSON object
            }
            // Expecting a string key
            if (token.type != Tokenizer::TokenType::STRING) {
                throw runtime_error("Expected string key in JSON object");
            }
            // Store key
            string key = token.value;
            // Expecting colon
            token = tokenizer.getNextToken();
            if (token.type != Tokenizer::TokenType::COLON) {
                throw runtime_error("Expected ':' after key in JSON object");
            }
            // Get value
            token = tokenizer.getNextToken();
            if (token.type != Tokenizer::TokenType::STRING) {
                throw runtime_error("Expected string value in JSON object");
            }
            // Store value
            JSONValue value;
            // For simplicity, we only handle string values for now
            value.type = valueType::STRING;
            // Assign string value
            value.stringValue = token.value;
            // Insert key-value pair into map
            jsonMap[key] = value;
            // Get next token (either comma or end of object)
            token = tokenizer.getNextToken();
            // Check for comma or end of object
            if (token.type == Tokenizer::TokenType::COMMA) {
                continue; // Next key-value pair
            } else if (token.type == Tokenizer::TokenType::RIGHT_BRACKET) {
                break; // End of JSON object
            } else {
                throw runtime_error("Expected ',' or '}' in JSON object");
            }
        }
        return jsonMap;
    }
};

string displayHelp() {
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

int countBytes(ifstream &inFile) {
    // clear any eof flags
    inFile.clear();

    // skip to end
    inFile.seekg(0, ios::end);

    // return position as byte count
    return inFile.tellg();
}

int countLines(ifstream &inFile) {
    // line variable
    string line;

    // counter
    int lineCount = 0;

    // clear any eof flags
    inFile.clear();

    // seek to beginning
    inFile.seekg(0, ios::beg);

    // while line exists, increment count
    while (getline(inFile, line)) {
        lineCount++;
    }

    // return count
    return lineCount;
}

int countWords(ifstream &inFile) {
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
    while (inFile >> word) {
        wordCount++;
    }

    // return count
    return wordCount;
}

void parseAndWriteJsonFiles(ifstream &file1, ofstream &file2) {
    try {
        // Create a parser instance with the input file stream
        Parser parser(file1);
        // Parse the JSON and get the key-value map
        map<string, Parser::JSONValue> jsonMap = parser.parse();

        // Write parsed key-value pairs to output file
        for (const auto &pair : jsonMap) {
            if (pair.second.type == Parser::valueType::STRING) {
                file2 << pair.first << ": " << pair.second.stringValue << endl;
            } else {
                file2 << pair.first << ": [Unsupported Type]" << endl;
            }
        }
    } catch (const runtime_error &e) {
        cerr << "Error parsing JSON: " << e.what() << endl;
        return;
    }
    clearOss();
    return;
}

void clearOss() {
    oss.str(""); // clear the stringstream
    oss.clear(); // clear any error flags
}