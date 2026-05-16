// to avoid redefination 
#pragma once
#include<vector>
#include<string>
#include<stack>
#include "Move.h"
#include "Constants.h"

// tbh this is not required as they are defied in Constansts.h 
#ifndef BOARD_SIZE
#define BOARD_SIZE 8
#endif
#ifndef WHITE
#define WHITE 0
#endif
#ifndef BLACK
#define BLACK 1
#endif
#ifndef EMPTY
#define EMPTY '.'
#endif
#ifndef CASTLE_WK
#define CASTLE_WK 1
#define CASTLE_WQ 2
#define CASTLE_BK 4
#define CASTLE_BQ 8
#endif
#ifndef NO_EP
#define NO_EP -1
#endif
// forward declaration not required normally as we included Move.h 
class Move;
// Stores the extra information needed to undo a move.
// A Move only tells us what was played, for example:
//   from square -> to square
//   promotion piece
//   whether it was castling/en passant
// But when we make a move, some board metadata changes too:
//   -> castling rights may be removed
//   -> en passant availability may change
//   ->halfmove clock may reset/increase
//   -> fullmove number may change
//   -> a piece may be captured
// To correctly unmake/undo a move, we must restore the exact old values.
// Therefore, before applying each move, we save these values in BoardState
// and push it onto historyStack.
// During unmakeMove() we pop the latest BoardState and restore the board
// to exactly how it was before that move.
struct BoardState {
    // the key difference between the struct snapshot and the boardstate is 
    // boardstate doesnot include
    // doesnot captured by white , black 
    // oesnot have moves played by white,black 
    // doesnot have board representation
    // does not have Piece captured by the move.
    // EMPTY if no normal capture happened.
    char capturedPiece;
    // Castling rights before the move was made.
    // Needed because a king or rook move or rook capture can permanently as explained before
    // remove castling rights, and undo must restore the previous value.
    int castlingRights;
    // En passant column before the move was made.
    // Needed because en passant is available for only one move and is
    // usually reset after every move.
    int enPassantCol;
    // Halfmove clock before the move was made.
    // Used for the fifty-move rule / FEN state.
    int halfMoveClock;
    // True if the move was an en passant capture.
    // This helps unmakeMove() know that the captured pawn is not located
    // on the destination square, but behind it.
    // this is already there in Move if we want we can use that but include it so it will be complete
    bool isEnPassantCapture;
    // Row where the en passant captured pawn must be restored tbh this is not required we can use enPassantCol which is enough to say the row
    // the row is fixed anyways for enpassant
    // Useful because en passant captures remove a pawn from a different
    // square than the move's destination square.
    int epPawnRow;
    // Fullmove number before the move was made.
    // In chess, this increments after Black's move.
    int fullMoveNumber;
    // no need to extra whitemoves and blackmoves 
    // bcz we only doing and undo move 
    // this is fast rather making new copy in each depth 
};

class Board{
    public:
        //constructor
        Board();
        void initStartPosition();
        void applyMove(const Move& move);
        void makeMove(const Move& move);
        void unmakeMove(const Move& move);
        // these functions will be const as they shouldn't change the board states
        // loads the board from the data
        void loadFromRaw(const char raw[BOARD_SIZE][BOARD_SIZE],int side,int castle,int ep,int halfClock,int fullNum,const std::vector<Move>& wMoves,const std::vector<Move>& bMoves,std::vector<char>& capW,const std::vector<char>& capB);
        // to check if the king is in check 
        bool isInCheck(int side) const;
        // checks the respective king is present in respective row and col
        bool findKing(int side,int &row,int &col) const;
        // checks if the row and col is attacked by the respective attacker 
        bool isSquareAttacked(int row,int col,int attacker) const;
        
        // static functions -> can be used without declaring obj
        // these functions can be declared in utils.h alse
        // the reason i preffered to declare here rightnow the utlis.h has func related to clearscreen trim rowtorank coltochar 
        // these are more related to board whereas utils.h has related to managment of game notation , clearing screen
        // if u wish these can be moved into utils.h
        static bool isWhite(char p);
        static bool isBlack(char p);
        static bool isEmpty(char p);
        static bool sameColor(char a,char b);
        
        // converts the current board into string type like fen const because it shouldnot change the board structure 
        std::string serialize() const;
        // bool because if the board loading fails we need to catch the false and print reason rather than std::cerr 
        // this function should not be const as the board will be modified here
        bool deserialize(const std::string& data);
        // prevents extending of even with low material like a king+knight Vs king and king + bishop Vs king 
        // this is not much required as we already included 50 Move rule
        // made this because because waiting for 50 Moves is tiring and boring
        bool hasSufficientMaterial() const;

        // our board notation
        char board[BOARD_SIZE][BOARD_SIZE];
        // which side to move
        int sideToMove;
        // as explained earlier which kings can castle which side in future
        int castlingRights;
        // the col where enPassantCol happend
        int enPassantCol;
        // halfmove count
        int halfMoveClock;
        int fullMoveNumber;
        // Moves made by white
        std::vector<Move> whiteMoves;
        // Moves made by the Black
        std::vector<Move> blackMoves;
        // pieces captured by white 
        std::vector<char> capturedByWhite;
        // pieces captured by black 
        std::vector<char> capturedByBlack;
    private:
        // this should be private if this is public 
        // user can change the game history as he want
        std::vector<BoardState> historyStack;
};  
