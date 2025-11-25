# Description
This is a simple command line tool written in C++ that processes command line arguments and performs basic operations based on the provided flags.
I am not going to have this add to the path or be installed system-wide, it is just a simple tool for learning and experimentation.
I saw this project on https://codingchallenges.fyi/challenges/challenge-wc, and figured it would be more practical than a ascii art frogger game.

# Features

# System and Compiler
- Developed and tested on Linux and Windows
- Compiled with g++ (GNU Compiler Collection)

## Compilation Instructions
There are two shell scripts provided for compiling the project:
1. `build.sh`: For building the project and generating the executable.
2. `test.sh`: For testing the compiled executable.

## Usage Instructions
After compilation, navigate to the `output` directory and run the executable with the following syntax:
` ./t.exe [options] `

### Available Options:
- `-h` or `--help`: Displays help information about the tool.
- `-c`: outputs number of bytes in a file
- `-l`: outputs number of lines in a file
- `-w`: outputs number of words in a file
- `-f <filename>`: specifies the input file to be processed
