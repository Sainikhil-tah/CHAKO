#pragma once 
#include<string>
#include<vector>
#include<chrono>
#include "Board.h"
#include "MoveGenerator.h"
#include "SearchEngine.h"
#include "UndoManager.h"
// use class to prevent wrong comparison
enum class GameMode{HumanVsHuman,HumanVsEngine};
enum class GameType{Bullet,Blitz,Rapid,Classic,NoLimit};
// without class HumanVsHuman==Bullet 
// Bullet set gameduration to 2min 
// Blitz set gameduration to 5min
// classic set game suration to 1hr
// rapid set gamelimit to 10min
// NoLimit set Time to 10**9 ms and dont print time 
class Game{
    public:
        Game();
        int getDepth() const;
        void run();
    private: 
        Board board;
        MoveGenerator moveGen;
        SearchEngine engine;
        UndoManager undoMgr;
        GameMode gameMode;
        // removed as printing time is tough as we are currently clearing screen withrespect to moves 
        // GameType gameType;
        int humanSide;
        void printBoard() const;
        void printMoveHistory() const;
        void printCaptured() const;
        void printFullUI() const;
        void printStatus(const std::string& msg) const;
        // for save and load game 
        void showMainMenu();
        bool doHumanTurn(std::string& statusMsg);
        void doEngineTurn(std::string& statusMsg);
        // processes the user input and checks if he can move that piece or not and statusMsg is the string that need to be printed changed via & 
        // can make it void
        bool processSquareInput(const std::string& line, std::string& statusMsg);
        // handles the commands for terminal load,redo,undo,save,help this is to differ from move and cmd in terminal input 
        bool handleCommand(const std::string& cmd, std::string& statusMsg);

        bool isGameOver(std::string& result) const;
        void saveGame(const std::string& filename, std::string& statusMsg);
        void loadGame(const std::string& filename, std::string& statusMsg);
        static bool parseSquare(const std::string& s, int& row, int& col);
        std::vector<Move> movesFrom(int row, int col) const;
        std::chrono::steady_clock::time_point gameStartTime;
        long long gameDurationMs=0;
        // this is the property of engine 
        // we can change via engine.setTimeLimit()
        // long long moveTimeLimitMs;
};
