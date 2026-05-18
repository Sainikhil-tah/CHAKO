#pragma once
#include "Board.h"
#include "MoveGenerator.h"
#include "Evaluator.h"
#include "Move.h"
#include <vector>
#include <chrono>

class SearchEngine {
public:
    SearchEngine(int depth);
    // as we have searchDepth as priv var 
    void setDepth(int d) { searchDepth = d; }
    int getDepth() const { return searchDepth; }

    // initially one for the time limit 
    void setTimeLimit(long long ms){timeLimitMs=ms;}
    long long getTimeLimit(){return timeLimitMs;}
    Move findBestMove(Board& board);
    long long nodesSearched=0;
    int ScoreCp=0;
    long long TimeMs=0;
    // uses the end-start
    long long getTimeElapsed() const {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    }
    // checks if getTimeElapsed is > 
    bool isTimeUp();
private:
    int searchDepth;
    // initially set time limit to 15s assuming 10 minute game
    // our time limit should depend on the game time also 
    // then assuming 80 moves per game set it into game time // 80
    // we will ask your to enter game duratime time rather mode 
    // lets format it into 
    // 0h0m0s this is the format 
    // we will convert this into ms 
    // to get what mode he choose we should include game.h ?
    // make a func public to change time 
    long long timeLimitMs=15000;
    // use steady clock to prevent user from changing time wrt to system changes
    std::chrono::steady_clock::time_point startTime;
    // we can declare object and make moveGen.genratemoves(board)
    // or in the movegenerator make them static functions? which is better 
    // but we have our evaluate as virtual so as we did polymorphism 
    // so leave it 
    // we declared here it because we need a obj to access as they are not static
    MoveGenerator moveGen;
    Evaluator evaluator;
    int negamax(Board& board, int depth, int alpha, int beta);
    int quiescence(Board& board, int alpha, int beta);
    void orderMoves(std::vector<Move>& moves) const;
    // this can be static should change it into static as this does not depend on object
    int getPieceValue(char piece) const;
    // tells about what to capture 
    // if pawan capture any big piece more value if reverse less value 
    // returns the value of the capture 
    int mvvLva(const Move& m) const;
};
