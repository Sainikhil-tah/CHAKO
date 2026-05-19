#include "SearchEngine.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <cctype>
const int VAL_PAWN_MVV=1;
const int VAL_KNIGHT_MVV=2;
const int VAL_BISHOP_MVV=3;
const int VAL_ROOK_MVV=4;
const int VAL_QUEEN_MVV=5;
const int VAL_KING_MVV=6;
#ifndef INF_SCORE
#define INF_SCORE 100000
#endif
#ifndef CHECKMATE_SCORE
#define CHECKMATE_SCORE 9000
#endif
#ifndef STALEMATE_SCORE
#define STALEMATE_SCORE 0
#endif
SearchEngine::SearchEngine(int depth)
    : nodesSearched(0), searchDepth(depth) {}
bool SearchEngine::isTimeUp() {
    return getTimeElapsed() >= timeLimitMs;
}
Move SearchEngine::findBestMove(Board& board){
    // Resets stats for the engine move.
    nodesSearched=0;
    TimeMs=0;
    cp_score=0;
    startTime=std::chrono::steady_clock::now();
    std::vector<Move> moves=moveGen.generateLegalMoves(board);
    if (moves.empty()){
        // Even if no legal move exists, update final stats.
        TimeMs=getTimeElapsed();
        cp_score=0;
        return Move();
    }
    orderMoves(moves);
    int bestScore=-INF_SCORE;
    Move bestMove=moves.front();
    int alpha=-INF_SCORE;
    int beta=INF_SCORE;
    // even if time is up make sure we have atleast one move to return 
    bool searchedAtLeastOneMove=false;
    for (const Move& m:moves){
        if (searchedAtLeastOneMove && isTimeUp()){
            break;
        }
        board.makeMove(m);
        int score=-negamax(board,searchDepth-1,-beta,-alpha);
        board.unmakeMove(m);
        searchedAtLeastOneMove=true;
        if (score>bestScore){
            bestScore=score;
            bestMove=m;
        }
        if (score>alpha) {
            alpha=score;
        }
        if (isTimeUp()) {
            break;
        }
    }
    TimeMs=getTimeElapsed();
    cp_score=bestScore;
    // std::cout << "info depth " << searchDepth
    //           << " nodes " << nodesSearched
    //           << " time " << TimeMs
    //           << " score cp " << cp_score
    //           << std::endl;
    return bestMove;
}
int SearchEngine::negamax(Board& board,int depth,int alpha,int beta) {
    ++nodesSearched;
    // If time is over inside search return static eval.
    // Root already guarantees at least one move is searched.
    if (isTimeUp()) {
        int eval=evaluator.evaluate(board);
        // evaluator returns White point of view.
        // negamax needs side-to-move point of view.
        return (board.sideToMove==WHITE)?eval:-eval;
    }
    if (depth==0) {
        // to overcome horizon effect
        return quiescence(board, alpha, beta);
    }
    std::vector<Move> moves=moveGen.generateLegalMoves(board);
    if (moves.empty()) {
        if (board.isInCheck(board.sideToMove)) {
            return -(CHECKMATE_SCORE-(searchDepth-depth));
        }
        return STALEMATE_SCORE;
    }
    if (board.halfMoveClock>=100) {
        return STALEMATE_SCORE;
    }
    orderMoves(moves);
    for (const Move& m :moves) {
        if (isTimeUp()) {
            break;
        }
        board.makeMove(m);
        int score=-negamax(board,depth-1,-beta,-alpha);
        board.unmakeMove(m);
        if (score>=beta) {
            return beta;
        }
        if (score>alpha) {
            alpha=score;
        }
    }
    return alpha;
}
int SearchEngine::quiescence(Board& board, int alpha, int beta) {
    // Quiescence nodes are also counted now
    ++nodesSearched;
    // If time is over inside quiescence, return static eval.
    if (isTimeUp()) {
        int eval = evaluator.evaluate(board);
        // evaluator returns White point of view.
        // negamax/quiescence needs side-to-move point of view.
        return (board.sideToMove==WHITE)?eval:-eval;
    }
    int standPat=evaluator.evaluate(board);
    // evaluator returns White point of view.
    // convert to side-to-move point of view.
    if (board.sideToMove==BLACK) {
        standPat=-standPat;
    }
    if (standPat>=beta) {
        return beta;
    }
    if (standPat>alpha){
        alpha=standPat;
    }
    std::vector<Move> captures = moveGen.generateCaptureMoves(board);
    orderMoves(captures);
    for (const Move& m : captures) {
        if (isTimeUp()){
            break;
        }
        board.makeMove(m);
        int score=-quiescence(board,-beta,-alpha);
        board.unmakeMove(m);
        if (score>=beta){
            return beta;
        }
        if (score>alpha){
            alpha=score;
        }
    }
    return alpha;
}
void SearchEngine::orderMoves(std::vector<Move>& moves) const{
    std::stable_sort(moves.begin(), moves.end(),
        [this](const Move& a, const Move& b){
            return mvvLva(a) > mvvLva(b);
        });
}
int SearchEngine::getPieceValue(char piece) const {
    switch (std::toupper(static_cast<unsigned char>(piece))){
        case 'P': return VAL_PAWN_MVV;
        case 'N': return VAL_KNIGHT_MVV;
        case 'B': return VAL_BISHOP_MVV;
        case 'R': return VAL_ROOK_MVV;
        case 'Q': return VAL_QUEEN_MVV;
        case 'K': return VAL_KING_MVV;
        default:  return 0;
    }
}
int SearchEngine::mvvLva(const Move& m) const{
    if (m.captured=='.') {
        return 0;
    }
    int score=0;
    if (m.promotion!='\0'&&m.promotion!='.'){
        switch (std::toupper(static_cast<unsigned char>(m.promotion))) {
            case 'Q':
                score+=900;
                break;
            case 'R':
                score+=500;
                break;
            case 'B':
                score+=300;
                break;
            case 'N':
                score+=300;
                break;
        }
    }
    return getPieceValue(m.captured)*10-getPieceValue(m.piece)+score;
}
