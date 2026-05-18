#include "Evaluator.h"
#include <cctype>
// copied from WEB for Maxmimum Accuraccy 
// Piece-Square Tables (White's perspective, row 0 = rank 8)
// Values in centipawns.
// Black's PST is the mirror of White's (flipped vertically).
// Pawn: encourage center control and advancement
static const int PST_PAWN[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0 },
    { 50, 50, 50, 50, 50, 50, 50, 50 },
    { 10, 10, 20, 30, 30, 20, 10, 10 },
    {  5,  5, 10, 25, 25, 10,  5,  5 },
    {  0,  0,  0, 20, 20,  0,  0,  0 },
    {  5, -5,-10,  0,  0,-10, -5,  5 },
    {  5, 10, 10,-20,-20, 10, 10,  5 },
    {  0,  0,  0,  0,  0,  0,  0,  0 }
};
// Knight: prefer center squares
static const int PST_KNIGHT[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};
// Bishop: open diagonals
static const int PST_BISHOP[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-10,  5,  5, 10, 10,  5,  5,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10,  5,  0,  0,  0,  0,  5,-10},
    {-20,-10,-10,-10,-10,-10,-10,-20}
};
// Rook: prefer open files and 7th rank
static const int PST_ROOK[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0},
    {  5, 10, 10, 10, 10, 10, 10,  5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    {  0,  0,  0,  5,  5,  0,  0,  0}
};
// Queen: modest positional bonus, avoid early development
static const int PST_QUEEN[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5,  5,  5,  5,  0,-10},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    {  0,  0,  5,  5,  5,  5,  0, -5},
    {-10,  5,  5,  5,  5,  5,  0,-10},
    {-10,  0,  5,  0,  0,  0,  0,-10},
    {-20,-10,-10, -5, -5,-10,-10,-20}
};
// King: stay safe in the middlegame (corner)
static const int PST_KING[8][8] = {
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-20,-30,-30,-40,-40,-30,-30,-20},
    {-10,-20,-20,-20,-20,-20,-20,-10},
    { 20, 20,  0,  0,  0,  0, 20, 20},
    { 20, 30, 10,  0,  0, 10, 30, 20}
};
// Evaluator
int Evaluator::evaluate(const Board& board)const{
    return materialScore(board) + positionalScore(board);
}
int Evaluator::materialScore(const Board& board) const {
    int score = 0;
    for (int r = 0; r < BOARD_SIZE; ++r)
        for (int c = 0; c < BOARD_SIZE; ++c) {
            char p = board.board[r][c];
            if (p == EMPTY) continue;
            int val = pieceValue(p);
            score += Board::isWhite(p) ? val : -val;
        }
    return score;
}
int Evaluator::positionalScore(const Board& board) const {
    int score = 0;
    for (int r = 0; r < BOARD_SIZE; ++r)
        for (int c = 0; c < BOARD_SIZE; ++c) {
            char p = board.board[r][c];
            if (p == EMPTY) continue;
            int pst = pstValue(p, r, c);
            score += Board::isWhite(p) ? pst : -pst;
        }
    return score;
}

int Evaluator::pstValue(char p, int row, int col) const {
    // Black's PST is White's table mirrored vertically
    int r = Board::isWhite(p) ? row : (7 - row);
    char upper = static_cast<char>(std::toupper(p));
    switch (upper) {
        case 'P': return PST_PAWN  [r][col];
        case 'N': return PST_KNIGHT[r][col];
        case 'B': return PST_BISHOP[r][col];
        case 'R': return PST_ROOK  [r][col];
        case 'Q': return PST_QUEEN [r][col];
        case 'K': return PST_KING  [r][col];
        default:  return 0;
    }
}
// this should be made static 
int Evaluator::pieceValue(char p) {
    switch (std::toupper(p)) {
        case 'P': return VAL_PAWN;
        case 'N': return VAL_KNIGHT;
        case 'B': return VAL_BISHOP;
        case 'R': return VAL_ROOK;
        case 'Q': return VAL_QUEEN;
        case 'K': return VAL_KING;
        default:  return 0;
    }
}
