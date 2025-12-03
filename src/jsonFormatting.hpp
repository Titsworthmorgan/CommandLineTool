#ifndef JSONFORMATTING_HPP
#define JSONFORMATTING_HPP
#include <fstream>
#include <string>
#include <vector>

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
    // constructor
    Tokenizer(std::ifstream &file) : inFile(file) {}
    // get next token from input file
    Token getNextToken();
};

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
  public:
    // constructor
    Parser(std::ifstream &file) : tokenizer(file) {
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