#include "jsonFormatting.hpp"
#include <cctype>
#include <iostream>
#include <stdexcept>
using namespace std;

Tokenizer::Token Tokenizer::getNextToken() {
    char ch;
    while (inFile.get(ch)) {
        if (!isspace(ch)) {
            break;
        }
    }
    if (inFile.eof()) {
        return {TokenType::END_OF_FILE, "\n"};
    }
    if (isdigit(ch) || ch == '-') {
        std::string numStr(1, ch);
        while (inFile.get(ch) && (isdigit(ch) || ch == '.')) {
            numStr += ch;
        }
        inFile.unget();
        return {TokenType::NUMBER, numStr};
    }
    if (isalpha(ch)) {
        std::string identStr(1, ch);
        while (inFile.get(ch) && isalpha(ch)) {
            identStr += ch;
        }
        inFile.unget();
        if (identStr == "true" || identStr == "false") {
            return {TokenType::BOOL, identStr};
        } else if (identStr == "null") {
            return {TokenType::NULL_VALUE, identStr};
        } else {
            return {TokenType::UNKNOWN, identStr};
        }
    }
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

void Parser::advance() {
    currentToken = tokenizer.getNextToken();
}

bool Parser::match(Tokenizer::TokenType type) const {
    return currentToken.type == type;
}

void Parser::expect(Tokenizer::TokenType type, const std::string &message) {
    if (!match(type)) {
        throw runtime_error("Parsing error: " + message);
    }
    advance();
}

std::vector<Tokenizer::Token> Parser::parse() {
    std::vector<Tokenizer::Token> tokens;

    if (!match(Tokenizer::TokenType::LEFT_BRACKET) &&
        !match(Tokenizer::TokenType::LEFT_BRACE)) {
        throw std::runtime_error("JSON must start with '{' or '['");
    }

    while (!match(Tokenizer::TokenType::END_OF_FILE)) {
        tokens.push_back(currentToken);
        advance();
    }

    tokens.push_back(currentToken);

    return tokens;
}
std::vector<Tokenizer::Token> parseJsonFile(std::ifstream &file) {
    Parser parser(file);
    return parser.parse();
}

void formatJsonToFile(const std::vector<Tokenizer::Token> &tokens, std::ofstream &outFile) {
    int indentLevel = 0;
    bool indentNext = false;
    bool indentApplied = false;

    for (size_t i = 0; i < tokens.size(); i++) {
        const auto &token = tokens[i];
        const auto &nextToken = (i + 1 < tokens.size()) ? tokens[i + 1] : token;

        switch (token.type) {
            case Tokenizer::TokenType::LEFT_BRACKET:
                outFile << "{";
                if (nextToken.type != Tokenizer::TokenType::RIGHT_BRACKET)
                    outFile << "\n";
                indentNext = true;
                indentApplied = false;
                indentLevel++;
                break;

            case Tokenizer::TokenType::RIGHT_BRACKET:
                outFile << "\n";
                indentLevel--;
                outFile << std::string(indentLevel * 2, ' ') << "}";
                break;

            case Tokenizer::TokenType::LEFT_BRACE:
                outFile << "[";
                if (nextToken.type != Tokenizer::TokenType::RIGHT_BRACE)
                    outFile << "\n";
                indentNext = true;
                indentApplied = false;
                indentLevel++;
                break;

            case Tokenizer::TokenType::RIGHT_BRACE:
                outFile << "\n";
                indentLevel--;
                outFile << std::string(indentLevel * 2, ' ') << "]";
                break;

            case Tokenizer::TokenType::COLON:
                outFile << ": ";
                break;

            case Tokenizer::TokenType::COMMA:
                outFile << ",";
                if (nextToken.type != Tokenizer::TokenType::RIGHT_BRACE &&
                    nextToken.type != Tokenizer::TokenType::RIGHT_BRACKET)
                    outFile << "\n";
                indentNext = true;
                indentApplied = false;
                break;

            case Tokenizer::TokenType::STRING:
                if (indentNext && !indentApplied) {
                    outFile << std::string(indentLevel * 2, ' ');
                    indentApplied = true;
                    indentNext = false;
                }
                outFile << "\"" << token.value << "\"";
                break;

            case Tokenizer::TokenType::NUMBER:
            case Tokenizer::TokenType::BOOL:
                if (indentNext && !indentApplied) {
                    outFile << std::string(indentLevel * 2, ' ');
                    indentApplied = true;
                    indentNext = false;
                }
                outFile << token.value;
                break;

            case Tokenizer::TokenType::NULL_VALUE:
                if (indentNext && !indentApplied) {
                    outFile << std::string(indentLevel * 2, ' ');
                    indentApplied = true;
                    indentNext = false;
                }
                outFile << "null";
                break;

            case Tokenizer::TokenType::END_OF_FILE:
                outFile << "\n";
                break;

            default:
                break;
        }
    }
}

void parseAndWriteJsonFiles(std::ifstream &inFile, std::ofstream &outFile) {
    try {
        auto tokens = parseJsonFile(inFile);
        formatJsonToFile(tokens, outFile);
    } catch (const std::runtime_error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}