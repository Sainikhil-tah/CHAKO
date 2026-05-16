// to avoid redefination
#pragma once
#include <string>
#include "Constants.h"

// here we used namespace because these are belong to the class rather than the object 
// if we want we can define the class and use static so we can directly acess funtion without creating object 
namespace Utils{
    std::string unicode(char p);
    char colToChar(int col);
    char rowToRank(int row);
    int charToCol(char C);
    int rankToRow(char r);
    void clearScreen();
    void printSeparator();
    // trims the white space in the start and the end of s without changing s 
    // this function is just for safety we usually dont need this as any ways bcz for the string in toAlgebraic we wont add any extra white spaces
    std::string trim(const std::string& s);
}
