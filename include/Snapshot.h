// to avoid redefination 
#pragma once
#include<vector>
#include<string>
#include "Constants.h"
#include "Move.h"
// the snapshot contains the condition of the board 
// which player is moving , the position of the board
// the castlingrights,enpassantcol,fifty rule counter,number of moves
// the history of white moves and black moves 
// the pieces captured by the white , black 
// for the snapshot struct is enough as there is no function defination 
struct Snapshot{
    char board[BOARD_SIZE][BOARD_SIZE];
    int sideToMove;
    int castlingRights;
    int enPassantCol;
    // if more than equal 100 halfMoveClock happen without the capture of item or movement of pawan Draw the Game
    int halfMoveClock;
    int fullMoveNumber;
    std::vector<Move> whiteMoves;
    std::vector<Move> blackMoves;
    std::vector<char> capturedByWhite;
    std::vector<char> capturedByBlack;
};

