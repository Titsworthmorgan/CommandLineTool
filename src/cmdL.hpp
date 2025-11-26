#ifndef CMDL_HPP
#define CMDL_HPP
#include <fstream>
#include <string>
using namespace std;
string displayHelp();
int countBytes(ifstream &inFile);
int countLines(ifstream &inFile);
int countWords(ifstream &inFile);
string parseJsonFiles(ifstream &file1, ifstream &file2);
void clearOss();
#endif // CMDL_HPP