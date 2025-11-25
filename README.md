# Description
This is a simple command line tool written in C++ that processes command line arguments and performs basic operations based on the provided flags.

# Features

# System and Compiler
- Developed and tested on Linux and Windows
- Compatible with g++ and clang++ compilers

## Compilation Instructions
ensure you are in /src directory and run:

` g++ main.cpp cmdL.cpp -o ../output/commandlinetool.exe `

## Usage Instructions
After compilation, navigate to the `output` directory and run the executable with the following syntax:
` ./commandlinetool [options] `

### Available Options:
- `-h` or `--help`: Displays help information about the tool.
- `-c`: outputs number of bytes in a file
- `-l`: outputs number of lines in a file
- `-w`: outputs number of words in a file
- `-f <filename>`: specifies the input file to be processed

### Example Commands:
- To display help information:
    ` ./commandlinetool -h `
- To count bytes in a file:
    ` ./commandlinetool -c -f example.txt `
- To count lines in a file:
    ` ./commandlinetool -l -f example.txt `
- To count words in a file:
    ` ./commandlinetool -w -f example.txt `