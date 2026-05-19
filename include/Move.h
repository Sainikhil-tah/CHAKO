// to prevent redefining 
#pragma once
#include<string>
#include "Constants.h"

// we will define a class to store all the data a move need to carry 
class Move{
    // make it public as we need to access it from main.cpp file 
    public:
        //the row and col we are at 
        int fromRow,fromCol;
        //the row and col we are moving to
        int toRow,toCol;
        //the piece we are moving
        char piece;
        //the piece we captured if captured any otherwise NULL
        char captured;
        //if the piece was promoted (this is important because in the undo,redo move we should know from what it got promoted and to which promotion it took)
        char promotion;
        //did this piece under go enpassant
        bool isEnPassant;
        //did this piece under go castling
        bool isCastle;
        // these last three are essential for undo and redo of moves 
        //constructor 
        Move();
        //constructor
        Move(int fr,int fc,int tr,int tc,char piece=EMPTY,char captured=EMPTY,char promo=EMPTY,bool ep=false,bool castle=false);
        // we need a function which converts the move into the valid chess notation like e2e3 e7e8q  this is important so we can get stockfish-uci responses
        // make the function const as it should not change any data 
        std::string toAlgebraic() const;
        // this is required it makes checking easy 
        // Move a==Move b simply b is a value in vector of legal moves
        // we check if the current move is there in legal Move
        // operator overloading 
        bool operator==(const Move& other) const;
};
