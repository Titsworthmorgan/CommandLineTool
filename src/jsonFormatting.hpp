#ifndef JSONFORMATTING_HPP
#define JSONFORMATTING_HPP
#include <fstream>
#include <string>
#include <vector>

class Tokenizer {
  private:
    std::ifstream &inFile;

  public:
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

    struct Token {
        TokenType type;
        std::string value;
    };

    Tokenizer(std::ifstream &file) : inFile(file) {}
    Token getNextToken();
};

class Parser {
  private:
    Tokenizer tokenizer;
    Tokenizer::Token currentToken;
    void advance();
    bool match(Tokenizer::TokenType type) const;
    void expect(Tokenizer::TokenType type, const std::string &message);

  public:
    Parser(std::ifstream &file) : tokenizer(file) {
        advance();
    }
    std::vector<Tokenizer::Token> parse();
};

std::vector<Tokenizer::Token> parseJsonFile(std::ifstream &file);

void formatJsonToFile(const std::vector<Tokenizer::Token> &tokens, std::ofstream &outFile);

void parseAndWriteJsonFiles(std::ifstream &file1, std::ofstream &file2);

#endif // JSONFORMATTING_HPP