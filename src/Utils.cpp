#include"Utils.h"
#include<windows.h>
#include<iostream>
#include<cstdlib>
#include<cctype>
std::string Utils::unicode(char p){
    switch (p) {
        // i inverted thes 
        case 'k': return "\u2654";  // ♔ 
        case 'q': return "\u2655";  // ♕
        case 'r': return "\u2656";  // ♖
        case 'b': return "\u2657";  // ♗
        case 'n': return "\u2658";  // ♘
        case 'p': return "\u2659";  // ♙
        case 'K': return "\u265A";  // ♚ this is better for white 
        case 'Q': return "\u265B";  // ♛
        case 'R': return "\u265C";  // ♜
        case 'B': return "\u265D";  // ♝
        case 'N': return "\u265E";  // ♞
        case 'P': return "\u265F";  // ♟
        default:  return ".";
    }
}
int Utils::charToCol(char c){
    return (std::tolower(c) - 'a');
}
int Utils::rankToRow(char r){
    return 8-(std::tolower(r)-'0');
}
char Utils::colToChar(int col){
    return static_cast<char>('a'+col);
}
char Utils::rowToRank(int row){
    return static_cast<char>('0'+8-row);
}
void Utils::clearScreen(){
    #ifdef _WIN32
        std::system("cls");
    #else 
        std::system("clear");
    #endif
}
void Utils::printSeparator(){
    std::cout << "  +-----------------------------------------+\n";
}
std::string Utils::trim(const std::string& s) {
    int start=0;
    int end=s.size()-1;
    while (start<s.size()&&std::isspace(s[start])){
        start++;
    }
    while (end>=start&&std::isspace(s[end])) {
        end--;
    }
    return s.substr(start, end - start + 1);
}
