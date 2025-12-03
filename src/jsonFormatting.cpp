#include "jsonFormatting.hpp"
#include <cctype>
#include <iostream>
#include <stdexcept>
using namespace std;

Tokenizer::Token Tokenizer::getNextToken() {
    // define a char to read
    char ch;
    // skip whitespace
    while (inFile.get(ch)) {
        if (!isspace(ch)) {
            break;
        }
    }
    // check for end of file
    if (inFile.eof()) {
        return {TokenType::END_OF_FILE, "\n"};
    }
    // determine token type
    if (isdigit(ch) || ch == '-') {
        // number
        std::string numStr(1, ch);
        // read the rest of the number
        while (inFile.get(ch) && (isdigit(ch) || ch == '.')) {
            // append digit or decimal point to number string
            numStr += ch;
        }
        // unget the last character read that is not part of the number
        inFile.unget();
        // no leading zero's allowed unless the number is zero.something
        if (numStr[0] == '0' && numStr.length() > 1 && numStr[1] != '.') {
            throw runtime_error("Invalid number format with leading zeros: " + numStr);
        }
        // return number token
        return {TokenType::NUMBER, numStr};
    }
    // identifier (true, false, null)
    if (isalpha(ch)) {
        // read identifier
        std::string identStr(1, ch);
        // read the rest of the identifier
        while (inFile.get(ch) && isalpha(ch)) {
            // append character to identifier string
            identStr += ch;
        }
        // unget the last character read that is not part of the identifier
        inFile.unget();
        // determine if identifier is a boolean or null
        if (identStr == "true" || identStr == "false") {
            // boolean value
            return {TokenType::BOOL, identStr};
            // null value
        } else if (identStr == "null") {
            // return null token
            return {TokenType::NULL_VALUE, identStr};
        } else {
            // unknown identifier
            return {TokenType::UNKNOWN, identStr};
        }
    }
    // single character tokens
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
                cout << "Reading char: " << ch << endl;
                strValue += ch;
            }
            cout << "Completed string: " << strValue << endl;
            if (strValue.find('\n') != std::string::npos) {
                throw runtime_error("String literals cannot contain newlines");
            }

            if (strValue.find('\r') != std::string::npos) {
                throw runtime_error("String literals cannot contain carriage returns");
            }

            if (strValue.find('\t') != std::string::npos) {
                throw runtime_error("String literals cannot contain tabs");
            }

            if (strValue.find('\\') != std::string::npos) {
                throw runtime_error("String literals cannot contain backslashes");
            }

            if (strValue.find('"') != std::string::npos) {
                throw runtime_error("String literals cannot contain double quotes");
            }

            if (strValue.find('\0') != std::string::npos) {
                throw runtime_error("String literals cannot contain null characters");
            }

            if (strValue.find('\b') != std::string::npos) {
                throw runtime_error("String literals cannot contain backspace characters");
            }

            if (strValue.find('\f') != std::string::npos) {
                throw runtime_error("String literals cannot contain form feed characters");
            }

            if (strValue.find('\v') != std::string::npos) {
                throw runtime_error("String literals cannot contain vertical tab characters");
            }

            if (strValue.find('\a') != std::string::npos) {
                throw runtime_error("String literals cannot contain alert/bell characters");
            }

            if (strValue.find('\x1B') != std::string::npos) {
                throw runtime_error("String literals cannot contain escape characters");
            }

            if (strValue.find('\x7F') != std::string::npos) {
                throw runtime_error("String literals cannot contain delete characters");
            }
            
            return {TokenType::STRING, strValue};
        }
        case '[':
            return {TokenType::LEFT_BRACE, "["};
        case ']':
            return {TokenType::RIGHT_BRACE, "]"};
        case '\n':
            return {TokenType::END_OF_FILE, "\n"};
        case '\'':
            throw runtime_error("Single quotes are not valid in JSON strings");
        default:
            return {TokenType::UNKNOWN, std::string(1, ch)};
    }
}

// Parser member function implementations
void Parser::advance() {
    // assign next token to currentToken
    currentToken = tokenizer.getNextToken();
}

// check if current token matches type given
bool Parser::match(Tokenizer::TokenType type) const {
    // return true if current token type matches given type
    return currentToken.type == type;
}

// expect a token of a specific type, else throw error
void Parser::expect(Tokenizer::TokenType type, const std::string &message) {
    // if current token type does not match given type, throw error
    if (!match(type)) {
        throw runtime_error("Parsing error: " + message);
    }
    // otherwise, advance to next token
    advance();
}

// parse a value (object, array, string, number, bool, null)
void Parser::parseValue(int depth) {
    // push current token to tokens vector
    tokens.push_back(currentToken);
    if (depth > 20) {
        throw runtime_error("Exceeded maximum parsing depth - possible malformed JSON");
    }
    // determine type of value and parse accordingly
    if (match(Tokenizer::TokenType::LEFT_BRACKET)) {
        // object
        parseObject(depth + 1);
    } else if (match(Tokenizer::TokenType::LEFT_BRACE)) {
        // array
        parseArray(depth + 1);
    } else if (match(Tokenizer::TokenType::STRING) ||
               match(Tokenizer::TokenType::NUMBER) ||
               match(Tokenizer::TokenType::BOOL) ||
               match(Tokenizer::TokenType::NULL_VALUE)) {
        // primitive value
        advance();
    } else {
        if (currentToken.value == "undefined" || currentToken.value == "NaN" || currentToken.value == "Infinity") {
            throw runtime_error("Invalid value in JSON: " + currentToken.value);
        }
        throw runtime_error("Expected a value, got: " + currentToken.value + " instead");
    }
}

void Parser::parseObject(int depth) {
    // expect left bracket '{'
    expect(Tokenizer::TokenType::LEFT_BRACKET, "Expected '{'");

    if (currentToken.value == "/" || currentToken.value == "*") {
        throw runtime_error("Comments are not allowed in JSON");
    }
    // check for empty object
    if (match(Tokenizer::TokenType::RIGHT_BRACKET)) {
        // empty object
        tokens.push_back(currentToken);
        // push right bracket and advance
        advance();
        return;
    }
    // parse key-value pairs
    while (true) {
        // Parse key (must be a string)
        tokens.push_back(currentToken);
        // if not a string
        expect(Tokenizer::TokenType::STRING, "Expected string key in object");
        // Parse colon ':'
        tokens.push_back(currentToken);
        expect(Tokenizer::TokenType::COLON, "Expected ':' after object key");
        // Parse value again, must be a valid JSON value if we got here
        parseValue(depth);

        // if next token is right bracket, end of object
        if (match(Tokenizer::TokenType::RIGHT_BRACKET)) {
            // push right bracket
            tokens.push_back(currentToken);
            // advance and break
            advance();
            break;
            // else if next token is comma, continue to next key-value pair
        } else if (match(Tokenizer::TokenType::COMMA)) {
            // push comma
            tokens.push_back(currentToken);
            // advance to next token
            advance();
            if (currentToken.value == "/" || currentToken.value == "*") {
                throw runtime_error("Comments are not allowed in JSON");
            }
            // After comma, we MUST have another key-value pair, not a closing bracket
            if (match(Tokenizer::TokenType::RIGHT_BRACKET)) {
                throw runtime_error("Trailing comma in object - comma cannot appear before '}'");
            }
        } else {
            throw runtime_error("Expected ',' or '}' in object");
        }
    }
}

void Parser::parseArray(int depth) {
    // expect left brace '['
    expect(Tokenizer::TokenType::LEFT_BRACE, "Expected '['");

    // check for empty array
    if (match(Tokenizer::TokenType::RIGHT_BRACE)) {
        // empty array
        tokens.push_back(currentToken);
        // push right brace and advance
        advance();
        return;
    }
    // if comments are found here, throw error
    if (currentToken.value == "/" || currentToken.value == "*") {
        throw runtime_error("Comments are not allowed in JSON");
    }

    while (true) {
        // Call this thing again to parse the value
        parseValue(depth);

        if (currentToken.value == "/" || currentToken.value == "*") {
            throw runtime_error("Comments are not allowed in JSON");
        }
        // Check what comes next
        if (match(Tokenizer::TokenType::RIGHT_BRACE)) {
            // push right brace - it's the end of the array
            tokens.push_back(currentToken);
            // advance and break
            advance();
            break;
            // more elements
        } else if (match(Tokenizer::TokenType::COMMA)) {
            // push comma
            tokens.push_back(currentToken);
            //  advance to next token
            advance();

            // After comma, we MUST have another value, not a closing bracket
            if (match(Tokenizer::TokenType::RIGHT_BRACE)) {
                throw runtime_error("Trailing comma in array - comma cannot appear before ']'");
            }
            if (currentToken.value == "/" || currentToken.value == "*") {
                throw runtime_error("Comments are not allowed in JSON");
            }
        } else {
            throw runtime_error("Expected ',' or ']' in array");
        }
    }
}

std::vector<Tokenizer::Token> Parser::parse() {
    // clear any existing tokens
    tokens.clear();
    // initial check for valid starting token
    if (!match(Tokenizer::TokenType::LEFT_BRACKET) &&
        !match(Tokenizer::TokenType::LEFT_BRACE)) {
        throw std::runtime_error("JSON must start with '{' or '['");
    }
    depth = 0;
    // parse the value
    parseValue(depth);
    // push end of file token
    tokens.push_back(currentToken);
    // expect end of file
    expect(Tokenizer::TokenType::END_OF_FILE, "Expected end of file after JSON");
    // return all tokens
    return tokens;
}

std::vector<Tokenizer::Token> parseJsonFile(std::ifstream &file) {
    // create parser instance
    Parser parser(file);
    // return parsed tokens
    return parser.parse();
}

void formatJsonToFile(const std::vector<Tokenizer::Token> &tokens, std::ofstream &outFile) {
    // format JSON tokens to file with indentation
    int indentLevel = 0;
    // track if we need to indent next token
    bool indentNext = false;
    // track if indentation has been applied for the current line
    bool indentApplied = false;

    // iterate through tokens and format output
    for (size_t i = 0; i < tokens.size(); i++) {
        // current token and next token (if exists)
        const auto &token = tokens[i];
        const auto &nextToken = (i + 1 < tokens.size()) ? tokens[i + 1] : token;
        if (indentLevel > 10) {
            throw runtime_error("Exceeded maximum indentation level - possible malformed JSON");
        }
        /*
            Switch on token type:
            * Handle each token type accordingly
            * Adjust indentation levels and line breaks as needed
            * Write formatted output to outFile
        */
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
                outFile << token.value;
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
        // parse input file
        auto tokens = parseJsonFile(inFile);
        // format and write to output file
        formatJsonToFile(tokens, outFile);
    } catch (const std::runtime_error &e) {
        // handle parsing errors
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void testJsonParsing() {
    // fail 1 - 15, each should trigger a parsing error
    const std::vector<std::string> testFiles = {
        // "./testing/fail1.json",
        // "./testing/fail2.json",
        // "./testing/fail3.json",
        // "./testing/fail4.json",
        // "./testing/fail5.json",
        // "./testing/fail6.json",
        // "./testing/fail7.json",
        // "./testing/fail8.json",
        "./testing/fail9.json",
        // "./testing/fail10.json",
        // "./testing/fail11.json",
        // "./testing/fail12.json",
        // "./testing/fail13.json",
        // "./testing/fail14.json",
        // "./testing/fail15.json",
        // "./testing/pass1.json",
        // "./testing/pass2.json",
        // "./testing/pass3.json",
        // "./testing/pass4.json",
        // "./testing/pass5.json",
        // "./testing/pass6.json",
        // "./testing/pass7.json",
        // "./testing/pass8.json",
        // "./testing/pass9.json",
        // "./testing/pass10.json",
        // "./testing/pass11.json",
        // "./testing/pass12.json",
        // "./testing/pass13.json",
        // "./testing/pass14.json",
        // "./testing/pass15.json"
    };
    // for each file, output an error message if it fails to parse for what it is supposed to fail on
    for (const auto &filePath : testFiles) {
        std::ifstream inFile(filePath);
        if (!inFile) {
            std::cerr << "Could not open file: " << filePath << std::endl;
            continue;
        }

        try {
            auto tokens = parseJsonFile(inFile);
            std::cout << "Parsed successfully: " << filePath << std::endl;
        } catch (const std::runtime_error &e) {
            std::cout << "Failed to parse " << filePath << ": " << e.what() << std::endl;
        }
        inFile.close();
    }
}