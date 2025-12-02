#ifndef CMDL_HPP
#define CMDL_HPP
#include <fstream>
#include <string>
std::string displayHelp();
int countBytes(std::ifstream &inFile);
int countLines(std::ifstream &inFile);
int countWords(std::ifstream &inFile);
void clearOss();
#endif // CMDL_HPP