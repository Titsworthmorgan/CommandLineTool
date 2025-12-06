#ifndef JSONFORMATTING_HPP
#define JSONFORMATTING_HPP
#include <fstream>
#include <string>
#include <vector>
/*
Tokenizer Class!
This is used to split everything into tokens.
I kept this one pretty simple:
1. Data structure:
  - Enum for type of "token" or piece.
  - Struct for my own type, I wanted the enumerated type and a string of whatever it was
2. Process:
  All this really does is read through the file, stop at given points, and determine if it's valid or not.
  All of the validation was added afterwards. This was made with the sole purpose of just being used to generate something along the lines of:
  "{", ":", "value", "}", etc.
3. Functions:
  - getNextToken() - reads through the file and returns the next token it finds.
4. Error Handling:
  - Throws runtime errors for invalid number formats (leading zeros, multiple decimal points).
  - Throws errors for strings that are not properly closed.
*/
class Tokenizer {
  private:
    // input file stream
    std::ifstream &inFile;

  public:
    // token types
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
        UNDEFINED_,
        NaN,
        Inf,
        UNKNOWN
    };

    // token structure
    struct Token {
        TokenType type;
        std::string value;
    };

    // constructor - initialize with input file stream
    Tokenizer(std::ifstream &file) : inFile(file) {}
    // get next token from input file
    Token getNextToken();
};

/*
Parser Class!
This is used to read/translate and build a vector of "tokens" from what I dubbed the Tokenizer. Menacing!
1. Data structure:
  - Uses the Tokenizer class to get tokens from the file.
  - Keeps track of the current token being processed.
  - Stores all tokens in a vector.
2. Process:
  - The parser reads tokens from the Tokenizer and builds a structured representation of the JSON data.
  - It handles different JSON constructs like primitive values, objects, and arrays.
  - It includes error handling for malformed JSON and duplicate keys in objects.
3. Functions:
  - advance() - moves to the next token.
  - match() - checks if the current token matches a specific type.
  - expect() - ensures the current token is of a specific type, otherwise throws an error.
  - parseValue() - parses a JSON value based on its type.
  - parseObject() - parses a JSON object.
  - parseArray() - parses a JSON array.
  - validateObjectKeys() - checks for duplicate keys in JSON objects.
*/
class Parser {
  private:
    // call the tokenizer
    Tokenizer tokenizer;
    // current token being processed
    Tokenizer::Token currentToken;
    // store all tokens
    std::vector<Tokenizer::Token> tokens;
    // current depth level
    int depth = 0;
    // advance to the next token
    void advance();
    // check if current token matches type given
    bool match(Tokenizer::TokenType type) const;
    // expect a token of a specific type, else throw error
    void expect(Tokenizer::TokenType type, const std::string &message);
    // parse a value (object, array, string, number, bool, null)
    void parseValue(int depth);
    // parse an object
    void parseObject(int depth);
    // parse an array
    void parseArray(int depth);
    // validate object keys for duplicates
    // size_t startIndex, size_t endIndex
    bool validateObjectKeys();

  public:
    // constructor - initialize with input file stream
    Parser(std::ifstream &file) : tokenizer(file) {
        // initialize by advancing to the first token
        advance();
    }
    // main parse function - called immediately to get all tokens
    std::vector<Tokenizer::Token> parse();
};

// function to parse JSON file and return tokens
std::vector<Tokenizer::Token> parseJsonFile(std::ifstream &file);

// function to format tokens and write to output file
void formatJsonToFile(const std::vector<Tokenizer::Token> &tokens, std::ofstream &outFile);

// function to parse input JSON file and write formatted output to another file
void parseAndWriteJsonFiles(std::ifstream &file1, std::ofstream &file2);

// convenience test function
void testJsonParsing();

#endif // JSONFORMATTING_HPP