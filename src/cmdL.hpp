#ifndef CMDL_HPP
#define CMDL_HPP
#include <fstream>
#include <string>
using namespace std;
string displayHelp();
int countBytes(ifstream &inFile);
int countLines(ifstream &inFile);
int countWords(ifstream &inFile);
void clearOss();
void parseAndWriteJsonFiles(ifstream &file1, ofstream &file2);
#endif // CMDL_HPP