# Final Project: Command Line Tool in C++

## Description

This is a simple command line tool written in C++ that processes command line arguments and performs basic operations based on the provided flags.
I am not going to have this add to the path or even try to facilitate this working system wide for the final, it is just a simple tool for learning and experimentation.
I saw this project on https://codingchallenges.fyi/challenges/challenge-wc, and figured it would be more practical than a ascii art frogger game.
This has a JSON formatter included as well, which is a bit more complex than the other features. However I wanted to just see how far I could get with that before hitting any
critical roadblocks. The only one I really hit was time constraints considering other classes and such. I stopped short of getting the formatter perfect, but it does a decent job of
keeping it atleast consistent in what seems like A common sense way. (complete.json has been formatted with prettier for comparison)

### Notes

- I wanted to put the JSON formatter into two classes.
  - One for lexical analysis (tokenizing)
    - All this does is take in the raw JSON text and break it down into an array of strings representing the tokens.
      - [`{`, `"someKey"`, `: `, `"someValue"`, `}`]
  - One for parsing and formatting
    - This takes in the array of tokens from the tokenizer and validates the structure according to JSON standards as much as it can.
    - It then outputs a formatted version of the JSON to a new file.
- I used: https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c2-use-class-if-the-class-has-an-invariant-use-struct-if-the-data-members-can-vary-independently
  as a guide for when to use struct vs class in C++. I also did a bunch of other digging and deduced that classes should be declared in header files and their methods defined in cpp files.
- Testing I just grabbed the two most official looking JSON standards I could find online and made test cases based on those.
  - https://www.rfc-editor.org/rfc/pdfrfc/rfc8259.txt.pdf - The JavaScript Object Notation (JSON) Data Interchange Format
  - https://ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf - The JSON Data Interchange Standard
- I started with using a some old entity relation diagrams I used previously for work, and generate dummy data JSON based on those to use as test cases.
- Some features are a little odd, like the parser has -most- of the error handling in place however some is handled in token generation.
  - This is because certain errors are easier to catch during tokenization rather than parsing, such as invalid number formats (leading zeros, multiple decimal points) and unclosed strings.
  - Some of the error messages are not EXACTLY indicative of the actual problem, but they absolutely do point you in the right direction. IE; `Expected , or '}'` instead of unclosed string.
- The JSON formatter is still not quite perfect, but it generally does a good job of formatting valid JSON files into a more human-readable format. Its not quite up to what something like prettier or jq would do, but its close enough for this.
- clang-format is purely for my own convenienc to keep code style consistent. This can be ignored if desired and the default formatting will take precedence.

# Features

- Character counter (to be added)
- Byte counter
- Line counter
- Word counter
- JSON formatter (basic functionality)
- JSON testing

# System and Compiler

I started this project on a Linux machine, but finished it and tested it on windows, and verified everything was good to go again on both.
The idea here was to follow a 'measure twice cut once' principle to ensure cross platform compatibility. This I think would have been hard to mess up too badly anyway as I stuck to standard C++ libraries and features anyways but, a big part of this project was overcoming the not knowing what I don't know aspect of C++ development so I wanted to be sure just in case.

- Text editor used: Visual Studio Code, or any you prefer
- Compiled with g++ (GNU Compiler Collection) directly in terminal/command prompt.
  - I got REALLY tired of:
    1. cleaning output directories
    2. compiling with all the right flags
    3. compiling the executable to the correct output directory
  - So I stuck all of it into a shell script to make life a bit easier. Instructions below.

# Compilation Instructions

There are two shell scripts provided for compiling the project, because typing everything as many times as was needed was a no go.
I found out that I could use -Werror to treat warnings as errors which was super helpful to ensure code quality. Kind of like how linting works in interpreted languages.

1. `build.sh`: For building the project and generating the executable.

```bash
# build script: build.sh
# remove old output files
rm -rf ./output/*
echo "Cleaned output directory."
# create output directory if it doesn't exist
mkdir -p ./output
# compile the source files into an executable
g++ -Werror ./src/main.cpp ./src/cmdL.cpp ./src/jsonFormatting.cpp -o ./output/t.exe
# -Werror  - treat warnings as errors
# check if the build was successful
if [ $? -ne 0 ]; then
    # build failed
    echo "Build failed."
    exit 1
fi
# build succeeded
echo "Build succeeded."
```

2. `test.sh`: For testing the compiled executable.

```bash
# test script: test.sh
# runs the compiled executable with provided command line arguments
./output/t.exe "$@"
```

# Usage Instructions

To use the command line tool, run the executable with the desired options. Below are the available options:

1. Run `test.sh -[flag] [fileName.Example] [outputFileName.Example]` to output the desired count based on the flag provided.

   - Ensure you are in the root directory of the project where `test.sh` is located.
   - Ensure you pass in the correct file names and extensions for your use case.

2. Navigate to the directory containing the executable and run `./your_executable_name -[flag] [fileName.Example] [outputFileName.Example]`.

   - Note: This will vary based on where you have your desired file located. You may need to specify the full path to the file.
   - not recommended for general use, but useful.

3. For the final project the following will be what we use:
   - `-h`: Displays help information about the tool.
   - `-c`: outputs number of bytes in a file
   - `-f`: outputs number of characters in a file
   - `-l`: outputs number of lines in a file
   - `-w`: outputs number of words in a file\
   - `-a`: outputs all of the above counts in a single run
   - `-j`: formats input JSON file to be more human-readable in duplicate file with -formatted.json suffix - works... mostly.
   - `-t`: tests all of the JSON test cases according to the standards found online

## Available Options:

- `-h`: Displays help information about the tool.
- `-c`: outputs number of bytes in a file
- `-f`: outputs number of characters in a file
- `-l`: outputs number of lines in a file
- `-w`: outputs number of words in a file
- `-a`: outputs all of the above counts in a single run
- `-j`: formats input JSON file to be more human-readable in duplicate file with -formatted.json suffix.
- `-t`: tests all of the JSON test cases according to the standards found online

# Project Structure

- `src/`: Contains the source code files.
- `output/`: Directory for output files generated by the tool.
- `testing/`: Contains test input files for the tool.
- `.clang-format`: Configuration file for code formatting
- `test.sh`: Shell script for testing the executable.
- `build.sh`: Shell script for building the project.

# Testing

Testing I am looking to generate as many different pass/fail cases as I can think of or find online.
For this I have a BOAT LOAD of extra ERD's that could be put into 'real-word' scenarios or, at least ones I've used in the past for work without sharing any proprietary information.
There were two major points I needed to do testing iterations on:

- WC Standards
- JSON Standards
- Formatting output correctly for JSON output files
  - still a little broken but, mostly works in at lease A consistent manner.

## WC Standards

For now I'm working to follow these standards:
https://pubs.opengroup.org/onlinepubs/9799919799/ - The Open Group Base Specifications Issue 7, 2018 edition - wc(1) - word, line, byte, and character count
I am going to try my best to follow these guidelines for the word/line/byte counting features. Right now my base test is on `The Project Gutenberg eBook of The Art of War`
in .txt format.
For now the only thing I'm really worried about is:

1. Can it handle large files? (they should all be TXT files for this)
2. Does it count chars/lines/words/bytes correctly as per the standard?
3. Does it handle edge cases like empty files, files with only whitespace, etc.?

## JSON Standards

For now I'm working with these two standards:
https://www.rfc-editor.org/rfc/pdfrfc/rfc8259.txt.pdf - The JavaScript Object Notation (JSON) Data Interchange Format
https://ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf - The JSON Data Interchange Standard

### Test Cases for JSON Formatting

Each test case has a fail*.json (invalid) and pass*.json (valid) pair. Pass conditions are based on RFC 8259 and ECMA-404 standards:

1. **fail1/pass1 - Trailing Commas**

   - Pass Condition: No trailing commas allowed after last element in arrays or objects
   - Regulation: RFC 8259 Section 5 & 6 - arrays and objects must not have trailing commas -->

2. **fail2/pass2 - String Delimiters**

   - Pass Condition: Strings must use double quotes (") only, not single quotes (')
   - Regulation: RFC 8259 Section 7 - strings must be enclosed in quotation marks (U+0022)

3. **fail3/pass3 - Object Key Quoting**

   - Pass Condition: All object keys must be quoted strings
   - Regulation: RFC 8259 Section 4 - object member names must be strings (quoted)

4. **fail4/pass4 - Comments**

   - Pass Condition: No comments allowed (// or /\* \*/)
   - Regulation: RFC 8259 - JSON grammar does not include comments

5. **fail5/pass5 - Primitive Values**

   - Pass Condition: Only true, false, null allowed as literals (no undefined, NaN, Infinity)
   - Regulation: RFC 8259 Section 3 - valid values are object, array, number, string, true, false, null

6. **fail6/pass6 - Nesting Depth**

   - Pass Condition: Reasonable nesting depth (implementation-dependent, typically 20-100 levels max)
   - Regulation: RFC 8259 Section 9 - parsers may set limits on nesting depth to avoid stack overflow

7. **fail7/pass7 - Escape Sequences**
   For this one I just went with "anything other than the valid ones" as the fail case.
   the new line escape sequence I couldnt quite get working right so I just left it out.

   - Pass Condition: Only valid escape sequences: \", \\, \/, \b, \f, \n, \r, \t, \uXXXX
   - Regulation: RFC 8259 Section 7 - defines valid escape sequences, \x is not valid

8. **fail8/pass8 - Number Formatting**

   - Pass Condition: No leading zeros (except for 0.x), no hex/octal notation
   - Regulation: RFC 8259 Section 6 - numbers cannot have leading zeros, must be decimal

9. **fail9/pass9 - String Termination**

   - Pass Condition: All strings must be properly closed with ending quotation mark
   - Regulation: RFC 8259 Section 7 - strings must begin and end with quotation marks

10. **fail10/pass10 - Bracket Matching**

    - Pass Condition: Opening brackets [ must match with ], { must match with }
    - Regulation: RFC 8259 Sections 5 & 6 - proper structural tokens required

11. **fail11/pass11 - Duplicate Keys**

    - Pass Condition: Object keys should be unique (behavior of duplicates is undefined)
    - Regulation: RFC 8259 Section 4 - duplicate names within an object are discouraged

12. **fail12/pass12 - Array Trailing Comma**

    - Pass Condition: No comma after last array element
    - Regulation: RFC 8259 Section 5 - array values separated by commas, no trailing comma

13. **fail13/pass13 - Structure Completion**

    - Pass Condition: All opened braces/brackets must be closed
    - Regulation: RFC 8259 Section 2 - JSON text must be a complete structure

14. **fail14/pass14 - Number Sign**

    - Pass Condition: Positive numbers have no sign prefix, negative use minus only (no plus sign)
    - Regulation: RFC 8259 Section 6 - optional minus, no plus sign defined

15. **fail15/pass15 - Control Characters**
    This one I couldn't quite get working right but, its close enough.
    - I cant for the life of me figure out how to detect the difference between the new line and control character. Most of these are what I struggled on the most.
      So for this one I'm conceding to for now.
    - Pass Condition: Control characters (U+0000 to U+001F) must be escaped in strings
    - Regulation: RFC 8259 Section 7 - control characters must be escaped

### Testing Outline

1. Generate test json files, fail{num}.json EG: fail1.json, fail2.json, etc. with specific errors in them.
2. Generate test json files, pass{num}.json EG: pass1.json, pass2.json, etc. that are valid json files.
3. Run the parser against each file and verify that it correctly identifies valid and invalid files.
4. Fix bugs as they appear and re-test until all tests pass.
5. Generate testFinal.json that is a large JSON file with all expected passing features and run the tool against it to ensure it can handle larger files/output the corrrectly formatted version.
