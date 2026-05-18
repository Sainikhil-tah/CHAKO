#include "Board.h"
#include "MoveGenerator.h"
#include "SearchEngine.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
/*
the way python sends commands 
 go depth 4 
reply with bestmove 

for every move start from scratch 

*/
// Global UCI board and engine
static Board board;
// standard depth is set at 4 
static SearchEngine engine(4);
int parseDepthFromGo(const std::string& line) {
    std::stringstream ss(line);
    // in uci command is go depth d
    std::string token;
    int depth = 4; // default depth
    ss >> token; // "go"
    while (ss >> token) {
        if (token=="depth") {
            ss >> depth;
        }
    }
    return depth;
}
// Convert UCI move string into your internal Move object
// e2e4
// e7e8q not e7e8Q 
Move parseMove(const std::string& s, Board& board) {
    MoveGenerator mg;
    std::vector<Move> legal = mg.generateLegalMoves(board);
    for (const auto& m : legal) {
        if (m.toAlgebraic() == s) {
            return m;
        }
    }
    return Move();
}
void applyMoves(Board& board, std::stringstream& ss) {
    std::string moveStr;
    while (ss >> moveStr){
        Move m = parseMove(moveStr, board);
        if (m.piece != EMPTY) {
            board.makeMove(m);
        }
        else {
            std::cerr <<"FAILED TO PARSE MOVE: "<< moveStr  << std::endl;
        }
    }
}
void runUCI() {
    board.initStartPosition();
    engine.setDepth(4);
    engine.setTimeLimit(5000);
    std::string line;
    while (std::getline(std::cin, line)){
        if (line=="uci"){
            std::cout << "id name NikhilEngine\n";
            std::cout << "id author Nikhil\n";
            std::cout << "uciok\n";
            std::cout.flush();
        }
        else if(line=="isready"){
            std::cout << "readyok\n";
            std::cout.flush();
        }
        else if (line == "ucinewgame") {
            board.initStartPosition();
        }
        else if (line.rfind("position", 0) == 0) {
            std::stringstream ss(line);
            std::string token;
            ss >> token; // "position"
            ss >> token; // "startpos" or "fen"
            if (token == "startpos") {
                board.initStartPosition();
                if (ss >> token) {
                    if (token == "moves") {
                        // send the complete stream and apply it 
                        applyMoves(board, ss);
                    }
                }
            }
        }
        else if (line.rfind("go", 0) == 0) {
            int depth = parseDepthFromGo(line);
            engine.setDepth(depth);
            // std::cerr << "go command: " << line << std::endl;
            // std::cerr << "using depth = " << engine.getDepth() << std::endl;
            Move best=engine.findBestMove(board);
            MoveGenerator mg;
            std::vector<Move> legal=mg.generateLegalMoves(board);
            bool found = false;
            for (const auto& m:legal) {
                if (m==best) {
                    found=true;
                    break;
                }
            }
            // best moves isnt leagal 
            if (!found) {
                std::cerr << "ERROR: engine returned illegal/null best move"
                          << std::endl;
                std::cout << "bestmove 0000\n";
                std::cout.flush();
                continue;
            }
            std::cout << "bestmove " << best.toAlgebraic() << "\n";
            std::cout.flush();
        }
        // d for debug 
        else if (line=="d") {
            std::cout << board.serialize() << "\nEND_BOARD\n";
            std::cout.flush();
        }
        else if (line == "quit") {
            break;
        }
        // unknown command 
        else {
            std::cerr << "Unknown UCI command: "
                      << line
                      << std::endl;
        }
    }
}
