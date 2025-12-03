#include "jsonFormatting.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
using namespace std;

// token acquisition from input file
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
        // check for multiple decimal points
        if (numStr.find("..") != std::string::npos) {
            throw runtime_error("Invalid number format with multiple decimal points: " + numStr);
        }
        // if multiple decimal points
        size_t decimalCount = std::count(numStr.begin(), numStr.end(), '.');
        if (decimalCount > 1) {
            // throw error
            throw runtime_error("Invalid number format with multiple decimal points: " + numStr);
        }
        // return number token
        return {TokenType::NUMBER, numStr};
    }
    // primitive types: true, false, null
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
        // here we're selectively returning tokens based on the identifier string
        // I just wanted to limit it to: "it can only be..."
        if (identStr == "true" || identStr == "false") {
            // boolean value returned
            return {TokenType::BOOL, identStr};
            // null value returned
        } else if (identStr == "null") {
            // return null token
            return {TokenType::NULL_VALUE, identStr};
        } else {
            // unknown identifier returned
            return {TokenType::UNKNOWN, identStr};
        }
    }
    // single character tokens
    // again we want to be select valid tokens only, everything else is for all intents and purposes unknown or invalid
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
            // strings we handle differently, need to account for escape sequences
            // this was a nightmare to figure out because I keep forgetting that \\ == \ in a string literal in C++ for example
            // However a quick google search I was able to find a table of valid escape sequences in JSON strings
            // and some articles on what they are read as.
            std::string strValue;
            // read until closing quote, handling escape sequences
            while (inFile.get(ch)) {
                // check for escape character
                if (ch == '\\') {
                    // append escape character
                    strValue += ch;
                    // read next character
                    if (inFile.get(ch)) {
                        strValue += ch; // Add the escaped character
                    }
                    // else if we find another quote
                } else if (ch == '"') {
                    // Found unescaped quote - this ends the string and do nothing
                    break;
                } else {
                    // Regular character, append to string value
                    strValue += ch;
                }
            }
            // validate escape sequences in string
            for (size_t i = 0; i < strValue.length(); i++) {
                // check for escape character
                if (strValue[i] == '\\') {
                    // ensure there is a next character
                    if (i + 1 >= strValue.length()) {
                        // invalid escape sequence
                        throw runtime_error("Invalid escape sequence in string: \\");
                    }
                    // check for valid escape characters
                    char nextChar = strValue[i + 1];
                    /*
                        Valid escape characters are:
                        " \ / b f n r t uXXXXX
                        Anything else is invalid
                        Just throwing this all in an if statement.
                    */
                    if (nextChar != '"' && nextChar != '\\' && nextChar != '/' &&
                        nextChar != 'b' && nextChar != 'f' && nextChar != 'n' &&
                        nextChar != 'r' && nextChar != 't' && nextChar != 'u') {
                        cout << "Invalid escape sequence found here: \\" << strValue << endl;
                        throw runtime_error("Invalid escape sequence in string: \\" + std::string(1, nextChar));
                    }
                    i++;
                }
            }
            // return string token with quotes included
            return {TokenType::STRING, "\"" + strValue + "\""};
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

/*
    Our base case is this:
    1. If we find a LEFT_BRACKET, we parse an object
    2. If we find a LEFT_BRACE, we parse an array
    3. If we find a STRING, NUMBER, BOOL, or NULL_VALUE, we accept it as a valid primitive value
    4. If we find anything else, we throw an error indicating an unexpected token
    The goal here is just to evaluate structure we're looking at, and call the appropriate parsing function based on what we find.
    We can potentially have nested structures, so we just call it's self over and over again until we reach primitive values.
    I could not find another way to do this as we're just doing the same thing over and over again.
    Objects for your objects in objects that have more objects.
*/
void Parser::parseValue(int depth) {
    // push current token to tokens vector
    tokens.push_back(currentToken);
    // setting a maximum depth of 20, because why not
    if (depth > 20) {
        throw runtime_error("Exceeded maximum parsing depth - possible malformed JSON");
    }
    // determine type of value and parse accordingly
    if (match(Tokenizer::TokenType::LEFT_BRACKET)) {
        // object, increase depth
        parseObject(depth + 1);
    } else if (match(Tokenizer::TokenType::LEFT_BRACE)) {
        // array, increase depth
        parseArray(depth + 1);
        // primitive value
    } else if (match(Tokenizer::TokenType::STRING) ||
               match(Tokenizer::TokenType::NUMBER) ||
               match(Tokenizer::TokenType::BOOL) ||
               match(Tokenizer::TokenType::NULL_VALUE)) {
        // advance to next token, we've gotten a valid primitive value and can move on
        advance();
    } else {
        // invalid value
        if (currentToken.value == "undefined" || currentToken.value == "NaN" || currentToken.value == "Infinity") {
            throw runtime_error("Invalid value in JSON: " + currentToken.value);
        }
        throw runtime_error("Expected a value, got: " + currentToken.value + " instead");
    }
}

/*
TODO: make this here work
  "arrayOfObjects": [
    {"id": 1, "name": "Alice", "active": true},
    {"id": 2, "name": "Bob", "active": false},
    {"id": 3, "name": "Charlie", "active": true}
  ]
  */
bool Parser::validateObjectKeys() {
    // make a vector to store all the keys we've seen
    vector<string> keys;
    // loop through em
    for (size_t i = 0; i < tokens.size(); i++) {
        // if we find a string followed by a colon, it's a key
        if (tokens[i].type == Tokenizer::TokenType::STRING && i + 1 < tokens.size() && tokens[i + 1].type == Tokenizer::TokenType::COLON) {
            // check if key already exists
            string key = tokens[i].value;
            // if found in keys vector already, duplicate key
            if (std::find(keys.begin(), keys.end(), key) != keys.end()) {
                // duplicate key found
                cout << "Duplicate key found: " << key << endl;
                // return false - invalid
                return false;
            }
            // otherwise, add key to keys vector
            keys.push_back(key);
        }
    }
    // no duplicates found - valid
    return true;
}

/*
Object parsing function:
    1. Expect a LEFT_BRACKET '{' to start the object.
    2. Check for an empty object by looking for a RIGHT_BRACKET '}' immediately after.
    3. If not empty, parse key-value pairs:
       - Keys must be STRING tokens followed by a COLON ':'.
       - Values are parsed using parseValue(), allowing for nested structures.
    4. After each key-value pair, expect either a COMMA ',' to continue or a RIGHT_BRACKET '}' to end the object.
    5. Validate keys for duplicates using validateObjectKeys().
*/
void Parser::parseObject(int depth) {
    // expect left bracket '{'
    expect(Tokenizer::TokenType::LEFT_BRACKET, "Expected '{'");

    // if comments are found here, throw error
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
    // check key-value pairs -- broken rn
    if (!validateObjectKeys()) {
        throw runtime_error("Duplicate keys found in object");
    }
    // while true to keep parsing key-value pairs
    while (true) {
        // push current token (key)
        tokens.push_back(currentToken);
        // if not a string
        expect(Tokenizer::TokenType::STRING, "Expected string key in object");
        // push colon ':'
        tokens.push_back(currentToken);
        expect(Tokenizer::TokenType::COLON, "Expected ':' after object key");

        // if we've gotten here we can either have a primitive value, or something else that is capable of holding more stuff.
        // call parseValue to figure it out
        parseValue(depth);

        // we're back here after finally receiving a primitive value or finishing a nested structure
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
            // we're expecting either a comma or right bracket here
            throw runtime_error("Expected ',' or '}' in object");
        }
    }
}

/*
Array parsing function:
    1. Expect a LEFT_BRACE '[' to start the array.
    2. Check for an empty array by looking for a RIGHT_BRACE ']' immediately after.
    3. If not empty, parse values using parseValue(), allowing for nested structures.
    4. After each value, expect either a COMMA ',' to continue or a RIGHT_BRACE ']' to end the array.
*/
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
        // check for comments
        if (currentToken.value == "/" || currentToken.value == "*") {
            throw runtime_error("Comments are not allowed in JSON");
        }
        // if we've gotten here we can either have a primitive value, or something else that is capable of holding more stuff.
        // call parseValue to figure it out
        parseValue(depth);
        // Check what comes next, we've received a value
        if (match(Tokenizer::TokenType::RIGHT_BRACE)) {
            // push right brace - it's the end of the array
            tokens.push_back(currentToken);
            // advance and break
            advance();
            break;
            // more elements, match it
        } else if (match(Tokenizer::TokenType::COMMA)) {
            // push comma
            tokens.push_back(currentToken);
            //  advance to next token
            advance();

            // After comma, we MUST have another value, not a closing bracket
            if (match(Tokenizer::TokenType::RIGHT_BRACE)) {
                throw runtime_error("Trailing comma in array - comma cannot appear before ']'");
            }
            // still no comments allowed
            if (currentToken.value == "/" || currentToken.value == "*") {
                throw runtime_error("Comments are not allowed in JSON");
            }
        } else {
            // we're expecting either a comma or right brace here
            throw runtime_error("Expected ',' or ']' in array");
        }
    }
}

/*
    This is the main parse function - called to start the process of building the token list.
    we start by checking if the first token is a valid starting token for JSON (either '{' or '[').
    Then we recursively parse the value, which can be an object, array, or primitive.
    Finally, we expect the end of file token to ensure the entire input has been consumed.
*/
std::vector<Tokenizer::Token> Parser::parse() {
    // clear any existing tokens
    tokens.clear();
    // initial check for valid starting token
    if (!match(Tokenizer::TokenType::LEFT_BRACKET) &&
        !match(Tokenizer::TokenType::LEFT_BRACE)) {
        throw std::runtime_error("JSON must start with '{' or '['");
    }
    depth = 0;
    // parse the value - this starts the recursive building of the token list
    // we arent actually returning anything so, these are going to be a series of void functions that manipulate the tokens vector directly
    // in the class instance
    parseValue(depth);
    // push end of file token
    tokens.push_back(currentToken);
    // expect end of file token to ensure entire input has been consumed
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

/*
    This function formats the JSON tokens into a properly indented structure and writes it to the output file.
    It manages indentation levels based on the structure of the JSON (objects and arrays) and ensures proper line breaks.
    Or atleast it's supposed to, this part seems a bit broken still.
*/
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
                // if indentation is needed and not yet applied
                if (indentNext && !indentApplied) {
                    // apply indentation
                    outFile << std::string(indentLevel * 2, ' ');
                    // mark indentation as applied
                    indentApplied = true;
                    // reset indentNext flag
                    indentNext = false;
                }
                // write left bracket
                outFile << "{";
                // if next token is not right bracket, add newline
                if (nextToken.type != Tokenizer::TokenType::RIGHT_BRACKET)
                    outFile << "\n";
                // set flags for next indentation
                indentNext = true;
                indentApplied = false;
                indentLevel++;
                break;
            case Tokenizer::TokenType::RIGHT_BRACKET:
                // write newline before right bracket
                outFile << "\n";
                // decrease indentation level
                indentLevel--;
                // write right bracket with proper indentation
                outFile << std::string(indentLevel * 2, ' ') << "}";
                break;
            case Tokenizer::TokenType::LEFT_BRACE:
                // if indentation is needed and not yet applied
                if (indentNext && !indentApplied) {
                    // apply indentation
                    outFile << std::string(indentLevel * 2, ' ');
                    // mark indentation as applied
                    indentApplied = true;
                    // reset indentNext flag
                    indentNext = false;
                }
                // write left brace
                outFile << "[";
                // if next token is not right brace, add newline
                if (nextToken.type != Tokenizer::TokenType::RIGHT_BRACE)
                    outFile << "\n";
                // set flags for next indentation
                indentNext = true;
                indentApplied = false;
                indentLevel++;
                break;
            case Tokenizer::TokenType::RIGHT_BRACE:
                // write newline before right brace
                outFile << "\n";
                // decrease indentation level
                indentLevel--;
                // write right brace with proper indentation
                outFile << std::string(indentLevel * 2, ' ') << "]";
                break;
            case Tokenizer::TokenType::COLON:
                // write colon
                outFile << ": ";
                break;
            case Tokenizer::TokenType::COMMA:
                // write comma
                outFile << ",";
                // if next token is not right brace or right bracket, add newline
                if (nextToken.type != Tokenizer::TokenType::RIGHT_BRACE &&
                    nextToken.type != Tokenizer::TokenType::RIGHT_BRACKET)
                    outFile << "\n";
                // set flags for next indentation
                indentNext = true;
                indentApplied = false;
                break;
            case Tokenizer::TokenType::STRING:
                // write string with proper indentation if needed
                if (indentNext && !indentApplied) {
                    outFile << std::string(indentLevel * 2, ' ');
                    indentApplied = true;
                    indentNext = false;
                }
                // write string
                outFile << token.value;
                break;
            case Tokenizer::TokenType::NUMBER:
                // write number with proper indentation if needed
                if (indentNext && !indentApplied) {
                    outFile << std::string(indentLevel * 2, ' ');
                    indentApplied = true;
                    indentNext = false;
                }
                // write number
                outFile << token.value;
                break;
            case Tokenizer::TokenType::BOOL:
                // write boolean with proper indentation if needed
                if (indentNext && !indentApplied) {
                    outFile << std::string(indentLevel * 2, ' ');
                    indentApplied = true;
                    indentNext = false;
                }
                // write boolean
                outFile << token.value;
                break;
            case Tokenizer::TokenType::NULL_VALUE:
                // write null with proper indentation if needed
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
        // try to parse the JSON file into tokens
        auto tokens = parseJsonFile(inFile);
        // try to format and write the tokens to output file
        formatJsonToFile(tokens, outFile);
    } catch (const std::runtime_error &e) {
        // handle parsing errors
        throw runtime_error("Error during JSON parsing/formatting: " + std::string(e.what()));
        throw;
    }
}

void testJsonParsing() {
    // just tossing all of em in an array
    const std::vector<std::string> testFiles = {
        "./testing/testingFinal.json",
        // "./testing/fail1.json",
        // "./testing/fail2.json",
        // "./testing/fail3.json",
        // "./testing/fail4.json",
        // "./testing/fail5.json",
        // "./testing/fail6.json",
        // "./testing/fail7.json",
        // "./testing/fail8.json",
        // "./testing/fail9.json",
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
        // open file
        std::ifstream inFile(filePath);
        // check if file opened successfully
        if (!inFile) {
            throw runtime_error("Failed to open test file: " + filePath);
            continue;
        }
        // try parsing
        try {
            // parse the JSON file into tokens
            auto tokens = parseJsonFile(inFile);
            // if we got here, parsing succeeded and we got some useful output
            std::cout << "Parsed successfully: " << filePath << std::endl;
        } catch (const std::runtime_error &e) {
            throw runtime_error("Error parsing file " + filePath + ": " + e.what());
        }
        inFile.close();
    }
}