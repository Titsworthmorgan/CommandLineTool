#include "cmdL.hpp"
#include <ctype.h>
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
// Putting these into classes for better structure and
// seperation of concerns.
// Tokenizer class to break input into tokens
class Tokenizer {
  private:
    // Reference to input file stream
    ifstream &inFile;

  public:
    // Enum to represent different types of tokens
    enum class TokenType {
        LEFT_BRACKET,
        RIGHT_BRACKET,
        LEFT_BRACE,
        RIGHT_BRACE,
        COLON,
        COMMA,
        STRING,
        NUMBER,
        BOOL,
        NULL_VALUE,
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
            return {TokenType::END_OF_FILE, "\n"};
        }
        // check for nums
        if (isdigit(ch) || ch == '-') {
            std::string numStr(1, ch);
            while (inFile.get(ch) && (isdigit(ch) || ch == '.')) {
                numStr += ch;
            }
            inFile.unget(); // put back the last read character
            return {TokenType::NUMBER, numStr};
        }
        // check for true, false, null
        // checks for alphabetic characters (a-zA-Z) only
        if (isalpha(ch)) {
            // build the identifier string
            std::string identStr(1, ch);
            // continue building the identifier string while characters are alphabetic
            while (inFile.get(ch) && isalpha(ch)) {
                identStr += ch;
            }
            inFile.unget(); // put back the last read character
            // check for true, false, null
            if (identStr == "true" || identStr == "false") {
                // return boolean token
                return {TokenType::BOOL, identStr};
            } else if (identStr == "null") {
                // return null token
                return {TokenType::NULL_VALUE, identStr};
            } else {
                // unknown identifier
                return {TokenType::UNKNOWN, identStr};
            }
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
            case '[':
                return {TokenType::LEFT_BRACE, "["};
            case ']':
                return {TokenType::RIGHT_BRACE, "]"};
            case '\n':
                return {TokenType::END_OF_FILE, "\n"};
            default:
                return {TokenType::UNKNOWN, std::string(1, ch)};
        }
    }
};

// Parser class to parse tokens into key-value pairs
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
        string stringValue;
    };

    vector<Tokenizer::Token> tokens;
    Parser(ifstream &file) : tokenizer(file) {}

    vector<Tokenizer::Token> parse() {
        // Get the first token
        Tokenizer::Token token = tokenizer.getNextToken();
        // Expecting a left bracket to start the JSON object
        if (token.type != Tokenizer::TokenType::LEFT_BRACKET) {
            throw runtime_error("Expected '{' at the beginning of JSON object");
        }
        tokens.push_back(token);
        // Continue parsing until end of file
        while (token.type != Tokenizer::TokenType::END_OF_FILE) {
            token = tokenizer.getNextToken();
            tokens.push_back(token);
        }
        return tokens;
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

// This just flat out needs to be fixed to properly parse JSON.
void parseAndWriteJsonFiles(ifstream &file1, ofstream &file2) {
    // Create parser instance
    Parser parser(file1);
    // Vector to hold tokens
    vector<Tokenizer::Token> tokens;
    // indentation level
    int indentLevel = 0;
    // track if indent was applied
    bool indentApplied = false;
    // track if next token needs indent
    bool indentNext = false;
    // Parse tokens
    try {
        tokens = parser.parse();
    } catch (const runtime_error &e) {
        // output error
        cerr << "Error parsing JSON: " << e.what() << endl;
        throw;
        runtime_error("Parsing failed");
    }
    // Write tokens to output file with formatting
    for (size_t i = 0; i < tokens.size(); i++) {
        // get current and next token
        const auto &token = tokens[i];
        const auto &nextToken = (i + 1 < tokens.size()) ? tokens[i + 1] : token;
        // switch on token type
        switch (token.type) {
            // left bracket
            case Tokenizer::TokenType::LEFT_BRACKET:
                // write left bracket if needed
                file2 << "{";
                // if not empty object, add newline
                if (nextToken.type != Tokenizer::TokenType::RIGHT_BRACKET)
                    file2 << endl;
                // set flags for indenting
                indentNext = true;
                indentApplied = false;
                // increase indent level
                indentLevel++;
                break;
            case Tokenizer::TokenType::RIGHT_BRACKET:
                // write with indent and newline if not empty object
                file2 << endl;
                // decrease indent level
                indentLevel--;
                file2 << string(indentLevel * 2, ' ') << "}";
                break;
            case Tokenizer::TokenType::RIGHT_BRACE:
                // same thing for right brace
                file2 << endl;
                indentLevel--;
                file2 << string(indentLevel * 2, ' ') << "]";
                break;
            case Tokenizer::TokenType::LEFT_BRACE:
                // write left brace
                file2 << "[";
                // if not empty array, add newline
                if (nextToken.type != Tokenizer::TokenType::RIGHT_BRACE)
                    file2 << endl;
                // set flags for indenting
                indentNext = true;
                indentApplied = false;
                indentLevel++;
                break;
            case Tokenizer::TokenType::COLON:
                // write colon with space
                file2 << ": ";
                break;
            case Tokenizer::TokenType::COMMA:
                // write comma
                file2 << ",";
                // if next token is not right brace or right bracket, add newline
                if (nextToken.type != Tokenizer::TokenType::RIGHT_BRACE &&
                    nextToken.type != Tokenizer::TokenType::RIGHT_BRACKET)
                    file2 << endl;
                // set flags for indenting
                indentNext = true;
                indentApplied = false;
                break;
            case Tokenizer::TokenType::STRING:
                // if indent needed and not yet applied
                if (indentNext && !indentApplied) {
                    file2 << string(indentLevel * 2, ' ');
                    indentApplied = true;
                    indentNext = false;
                }
                // write string value
                file2 << "\"" << token.value << "\"";
                // if next token is COMMA, do not add newline yet
                if (nextToken.type != Tokenizer::TokenType::COMMA &&
                    nextToken.type != Tokenizer::TokenType::COLON && nextToken.type != Tokenizer::TokenType::RIGHT_BRACE &&
                    nextToken.type != Tokenizer::TokenType::RIGHT_BRACKET) {
                    file2 << endl;
                }
                break;
            case Tokenizer::TokenType::END_OF_FILE:
                // probably don't need this
                file2 << endl;
                break;
            case Tokenizer::TokenType::NUMBER:
                // indent if needed
                if (indentNext && !indentApplied) {
                    file2 << string(indentLevel * 2, ' ');
                    indentApplied = true;
                    indentNext = false;
                }
                // write number value
                file2 << token.value;
                break;
            case Tokenizer::TokenType::BOOL:
                if (indentNext && !indentApplied) {
                    file2 << string(indentLevel * 2, ' ');
                    indentApplied = true;
                    indentNext = false;
                }
                file2 << token.value;
                break;
            case Tokenizer::TokenType::NULL_VALUE:
                // indent if needed
                if (indentNext && !indentApplied) {
                    file2 << string(indentLevel * 2, ' ');
                    indentApplied = true;
                    indentNext = false;
                }
                // write null value
                file2 << "null";
                break;
            default:
                // ignore unknown tokens for now
                break;
        }
    }
    // old
    // Simple output of tokens to file2
    // for (const auto &token : tokens) {
    //     switch (token.type) {
    //         case Tokenizer::TokenType::LEFT_BRACKET:
    //             file2 << "{" << endl;
    //             break;
    //         case Tokenizer::TokenType::RIGHT_BRACKET:
    //             file2 << "\n}" << endl;
    //             break;
    //         case Tokenizer::TokenType::RIGHT_BRACE:
    //             file2 << "]" << endl;
    //             break;
    //         case Tokenizer::TokenType::LEFT_BRACE:
    //             file2 << "[" << endl;
    //             break;
    //         case Tokenizer::TokenType::COLON:
    //             file2 << ": ";
    //             break;
    //         case Tokenizer::TokenType::COMMA:
    //             file2 << "," << endl;
    //             break;
    //         case Tokenizer::TokenType::STRING:
    //             file2 << "\"" << token.value << "\"";
    //             break;
    //         case Tokenizer::TokenType::END_OF_FILE:
    //             file2 << endl;
    //             break;
    //         case Tokenizer::TokenType::NUMBER:
    //             file2 << token.value;
    //             break;
    //         case Tokenizer::TokenType::BOOL:
    //             file2 << token.value;
    //             break;
    //         case Tokenizer::TokenType::NULL_VALUE:
    //             file2 << "null";
    //             break;
    //         default:
    //             // ignore unknown tokens for now
    //             break;
    //     }
    // }
    return;
}

void clearOss() {
    oss.str(""); // clear the stringstream
    oss.clear(); // clear any error flags
}