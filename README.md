# Description
This is a simple command line tool written in C++ that processes command line arguments and performs basic operations based on the provided flags.
I am not going to have this add to the path or be installed system-wide, it is just a simple tool for learning and experimentation.
I saw this project on https://codingchallenges.fyi/challenges/challenge-wc, and figured it would be more practical than a ascii art frogger game.
This may be expanded into something like a json parser or, who knows, but for the scope of the final it will be a simple word/line/byte counter first.
Anything additional will require specific flags to be passed in and return debug information if not validated against all test cases. (as many as I can remember or find online at least)

# Features
- Byte counter
- Line counter
- Word counter

# System and Compiler
- Developed and tested on Linux and Windows
- Compiled with g++ (GNU Compiler Collection)

# Compilation Instructions
There are two shell scripts provided for compiling the project:
1. `build.sh`: For building the project and generating the executable.
2. `test.sh`: For testing the compiled executable.

# Usage Instructions
To use the command line tool, run the executable with the desired options. Below are the available options:
1. Run `test.sh -[flag] [fileName.Example]` to output the desired count based on the flag provided.

2. Navigate to the directory containing the executable and run `./your_executable_name -[flag] [fileName.Example]`.
    - Note: This will vary based on where you have your desired file located. You may need to specify the full path to the file.

### Available Options:
- `-h`: Displays help information about the tool.
- `-c`: outputs number of bytes in a file
- `-l`: outputs number of lines in a file
- `-w`: outputs number of words in a file
- `-j`: formats input JSON file to be more human-readable in duplicate file with -formatted.json suffix
<!-- - `-f <filename>`: specifies the input file to be processed, potentially specifying multiple -->
