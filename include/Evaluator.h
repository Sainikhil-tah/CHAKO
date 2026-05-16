#pragma once 
#include "Board.h"
// currently we evaluate the board from the white persecptive 
// +ve score weight advantage 
/*
about virtual functions
#include <iostream>
using namespace std;

class Base {
public:
    void speak() {
        cout << "Base speaks\n";
    }
};

class Derived : public Base {
public:
    void speak() {
        cout << "Derived speaks\n";
    }
};

int main() {
    Derived d;
    Base* ptr = &d;

    ptr->speak();
}
here we will get output base speaks 
because ptr is of type Base*.
For non-virtual functions, C++ decides which function to call at compile time based on the pointer/reference type.
This is called static binding or early binding.
ptr->speak gets converts to 
Base::speak() even when the actual object is derived 
*/
class Evaluator{
    public: 
        // made evaluate virtual because in future i want to add different type of evaluators derived from this
        // evalutor which considers opening stability , fessible movement of big pieces like rook,queen 
        // evalutor which taks the game timing middle game,end game into consideration
        // instead of virtual we can also pass another argument to determine the type of evaluation need and return it 
        virtual int evaluate(const Board& board) const;
        virtual ~Evaluator()=default;
        static int pieceValue(char p);
    protected:
        // should be private but made protected because we are adding the derived classes in the future
        int materialScore(const Board& board) const;
        int positionalScore(const Board& board) const;
        int pstValue(char p,int row,int col) const;
        // the reason for private is these are no way related for the game main these are related to the search engine and evaluator only
        
};
