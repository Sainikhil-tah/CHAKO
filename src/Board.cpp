#include <iostream>
// to read complete line from terminal
#include <sstream>
#include <limits>
// all the moves are legal only as we are getting them from generate legal moves
#include "Move.h"
#include "Board.h"
Board::Board(){
    // sideToMove=WHITE;
    // castlingRights=CASTLE_WK|CASTLE_WQ|CASTLE_BK|CASTLE_BQ;
    // enPassantCol=NO_EP;
    // halfMoveClock=0;
    // fullMoveNumber=1;
    // for (int r=0;r<BOARD_SIZE;++r) {
    //     for (int c=0;c<BOARD_SIZE;++c) {
    //         board[r][c]=EMPTY;
    //     }
    // }
    initStartPosition();
}
void Board::initStartPosition(){
    for (int r=0;r<BOARD_SIZE;++r){
        for (int c=0;c<BOARD_SIZE;++c){
            board[r][c]=EMPTY;
        }
    }
    board[0][0]='r';
    board[0][1]='n';
    board[0][2]='b';
    board[0][3]='q';
    board[0][4]='k';
    board[0][5]='b';
    board[0][6]='n';
    board[0][7]='r';
    for (int c=0;c<BOARD_SIZE;++c){
        board[1][c]='p';
    }
    for (int c=0;c<BOARD_SIZE;++c){
        board[6][c]= 'P';
    }
    board[7][0]='R';
    board[7][1]='N';
    board[7][2]='B';
    board[7][3]='Q';
    board[7][4]='K';
    board[7][5]='B';
    board[7][6]='N';
    board[7][7]='R';
    sideToMove=WHITE;
    castlingRights=CASTLE_WK | CASTLE_WQ | CASTLE_BK | CASTLE_BQ;
    enPassantCol=NO_EP;
    enPassantRow=-1;
    halfMoveClock=0;
    fullMoveNumber=1;
    whiteMoves.clear();
    blackMoves.clear();
    capturedByWhite.clear();
    capturedByBlack.clear();
    while(!historyStack.empty()){
        historyStack.pop();
    }
}
void Board::makeMove(const Move& move){
    // now this move should be applied permanently 
    BoardState state;
    // this will be pushed into history stack 
    // the board status before move
    // if (sideToMove == WHITE) {
    //     whiteMoves.push_back(move);
    // } else {
    //     blackMoves.push_back(move);
    // }
    state.capturedPiece=board[move.toRow][move.toCol];
    state.castlingRights=castlingRights;
    state.enPassantCol=enPassantCol;
    state.halfMoveClock=halfMoveClock;
    state.fullMoveNumber=fullMoveNumber;
    state.isEnPassantCapture=false;
    state.epPawnRow=-1;
    if(move.isEnPassant){
        state.isEnPassantCapture=true;
        // here +1 means the below row as we have numbering from 0 to 7 from 0 -> black start
        state.epPawnRow=(sideToMove==WHITE)?move.toRow+1:move.toRow-1; 
        state.capturedPiece=board[state.epPawnRow][move.toCol];
        board[state.epPawnRow][move.toCol]=EMPTY;
    }
    // if(move.isEnPassant){
    //     board[epRow][move.toCol]=EMPTY;
    // }
    // this stores the state of board before the move
    // 
    // we should do this in game.cpp as these moves are just engine searchingg
    // if (state.capturedPiece!=EMPTY){
    //     if (sideToMove==WHITE) {
    //         capturedByWhite.push_back(state.capturedPiece);
    //     } else {
    //         capturedByBlack.push_back(state.capturedPiece);
    //     }
    // }
    historyStack.push(state);
    // after the move the enpass no longer exists
    enPassantCol=NO_EP;
    char piece=board[move.fromRow][move.fromCol];
    if(piece==EMPTY){
        std::cerr << "ERROR YOU HAVE SELECTED A EMPTY PIECE TO MOVE\n";
        std::cerr << move.toAlgebraic() << ' ' << "THIS IS EMPTY PIECE\n";
    }
    // no need to check for validation we only call this if the move is in the legal move
    board[move.toRow][move.toCol]=piece;
    board[move.fromRow][move.fromCol]=EMPTY;
    if(move.promotion!=EMPTY && ((piece=='P' && move.toRow==0)|| (move.toRow==7 && piece=='p'))){
        board[move.toRow][move.toCol]=move.promotion;
    }
    if (move.isCastle) {
        int row=move.fromRow;
        if (move.toCol==6) {
            board[row][5]=board[row][7];
            board[row][7]=EMPTY;
        } else {
            board[row][3]=board[row][0];
            board[row][0]=EMPTY;
        }
    }
    if (piece=='K') 
        castlingRights&=~(CASTLE_WK | CASTLE_WQ);
    if (piece=='k') 
        castlingRights&=~(CASTLE_BK | CASTLE_BQ);
    if (piece == 'R') {
        if(move.fromRow==7 && move.fromCol==0)
            castlingRights&=~CASTLE_WQ;
        if(move.fromRow==7 && move.fromCol==7)
            castlingRights&=~CASTLE_WK;
    }
    if (piece=='r') {
        if(move.fromRow==0 && move.fromCol==0)
            castlingRights&=~CASTLE_BQ;
        if(move.fromRow==0&&move.fromCol==7)
            castlingRights&=~CASTLE_BK;
    }
    if (move.toRow==7 && move.toCol==0)
        castlingRights &= ~CASTLE_WQ;
    if (move.toRow==7 && move.toCol==7)
        castlingRights &= ~CASTLE_WK;
    if (move.toRow==0 && move.toCol==0)
        castlingRights &= ~CASTLE_BQ;
    if (move.toRow==0 && move.toCol==7)
        castlingRights &= ~CASTLE_BK;
    if((piece=='P' || piece=='p') && std::abs(move.fromRow-move.toRow)==2){
        // this is should stored in the board 
        // when the opponent is making the move he can check if the c-1 or c+1 is enpass and he move to the correct place 
        // then make his move.enpas True 
        enPassantCol=move.fromCol;
    }
    if(piece=='P' || piece=='p' || state.capturedPiece!=EMPTY){
        halfMoveClock=0;
    }else{
        halfMoveClock++;
    }
    if(sideToMove==BLACK)
        fullMoveNumber++;
    sideToMove^=1;
}
void Board::unmakeMove(const Move& move){
    if (historyStack.empty()) return;
    BoardState state = historyStack.top();
    historyStack.pop();
    // if (Board::isWhite(move.piece)){
    //     if (!whiteMoves.empty()){
    //         whiteMoves.pop_back();
    //     }
    // } else if (Board::isBlack(move.piece)) {
    //     if (!blackMoves.empty()){
    //         blackMoves.pop_back();
    //     }
    // }
    sideToMove^=1;
    // if (state.capturedPiece!=EMPTY){
    //     if (Board::isWhite(move.piece)){
    //         if (!capturedByWhite.empty()){
    //             capturedByWhite.pop_back();
    //         }
    //     }else if (Board::isBlack(move.piece)){
    //         if (!capturedByBlack.empty()){
    //             capturedByBlack.pop_back();
    //         }
    //     }
    // }
    castlingRights=state.castlingRights;
    enPassantCol=state.enPassantCol;
    halfMoveClock=state.halfMoveClock;
    fullMoveNumber=state.fullMoveNumber;
    char piece=board[move.toRow][move.toCol];
    if(piece==EMPTY) {
        std::cerr << "ERROR: moving empty square\n";
        std::cerr << move.toAlgebraic() << "\n";
    }
    if (move.promotion!=EMPTY&&(move.toRow==0||move.toRow==7)){
        piece=(sideToMove==WHITE)?'P':'p';
    }
    board[move.fromRow][move.fromCol]=piece;
    // Restore captured piece
    if(state.isEnPassantCapture) {
        board[move.toRow][move.toCol]=EMPTY;
        board[state.epPawnRow][move.toCol]=state.capturedPiece;
    }
    else {
        board[move.toRow][move.toCol]=state.capturedPiece;
    }
    // if (state.isEnPassantCapture) {
    //     board[state.epPawnRow][move.toCol] = state.capturedPiece;
    //     board[move.toRow][move.toCol] = EMPTY;
    // }
    if (move.isCastle) {
        int row=move.fromRow;
        if (move.toCol==6) {
            board[row][7]=board[row][5];
            board[row][5]=EMPTY;
        } else {
            board[row][0]=board[row][3];
            board[row][3]=EMPTY;
        }
    }
}
// removing this to decrease per/move time 
// this is appiled on copied board it takes more time
// void Board::applyMove(const Move& move) {
//     makeMove(move);
//     if (!historyStack.empty()) {
//         historyStack.pop();
//     }
// }
bool Board::isInCheck(int side) const {
    // int kr, kc;
    int kr,kc;
    // we cant kill king should safeguard king  
    // if(!findKing(WHITE, kr, kc)) {
    //     std::cerr << "WHITE KING LOST\n";
    // }

    // if(!findKing(BLACK, kr, kc)) {
    //     std::cerr << "BLACK KING LOST\n";
    // }
    // calling this updates kr,kc
    if (!findKing(side,kr,kc)){
        std::cerr << "ERROR: King missing for side " << ((side==BLACK)?"BLACK":"WHITE") << "\n";
        return false;
    }
    return isSquareAttacked(kr,kc,side^1);
}
bool Board::isSquareAttacked(int row, int col, int attacker) const {
    // A knight can attack from 8 possible positions around the square.
    // static is made so even when to function is called 100's of time we create it only once
    static const int knightDr[8]={-2,-2,-1,-1,1,1,2,2};
    static const int knightDc[8]={-1,1,-2,2,-2,2,-1,1};
    char atkKnight=(attacker==WHITE)?'N':'n';
    for (int i=0;i<8;++i){
        int r=row+knightDr[i];
        int c=col+knightDc[i];
        if (r>=0&&r<8&&c>=0&&c<8&&board[r][c]==atkKnight){
            return true;
        }
    }
    // Rooks and queens attack horizontally and vertically.
    char atkRook=(attacker==WHITE)?'R':'r';
    char atkQueen=(attacker==WHITE)?'Q':'q';
    // created only once even when the function is called so many times
    static const int straight[4][2]={{-1,0},{1,0},{0,-1},{0,1}};
    for (const auto& d:straight) {
        int r=row+d[0];
        int c=col+d[1];
        // Keep moving in this direction until board ends or piece blocks
        while(r>=0&&r<8 && c>=0&&c<8) {
            char p=board[r][c];
            if (p!=EMPTY) {
                // First piece in this direction attacks if it is rook or queen
                if (p==atkRook||p==atkQueen){
                    return true;
                }
                // Any other piece blocks attack in this direction not pos to attack as bis , kn , any other cant attack straight 
                break;
            }
            r+=d[0];
            c+=d[1];
        }
    }
    // Bishops and queens attack diagonally.
    char atkBishop=(attacker== WHITE)?'B':'b';
    static const int diagonal[4][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};
    for (const auto& d:diagonal) {
        int r=row+d[0];
        int c=col+d[1];
        // Keep moving diagonally until board ends or piece blocks
        while (r>=0&&r<8 && c>=0&&c<8) {
            char p=board[r][c];
            if (p!=EMPTY) {
                // First piece in this direction attacks if it is bishop or queen
                if (p==atkBishop || p==atkQueen){
                    return true;
                }
                // Any other piece blocks attack in this direction
                break;
            }
            r+=d[0];
            c+=d[1];
        }
    }
    // White pawns move from larger row index to smaller row index.
    // So if (row, col) is attacked by a white pawn,
    // that pawn must be at row + 1 for black row-1
    if (attacker==WHITE) {
        // White pawn attacking this square must be one row below
        int pawnRow=row+1;
        if (pawnRow<8) {
            if(col-1>=0&&board[pawnRow][col-1]=='P'){
                return true;
            }
            if(col+1<8&&board[pawnRow][col+1]=='P') {
                return true;
            }
        }
    } else {
        // Black pawn attacking this square must be one row above
        int pawnRow=row-1;
        if (pawnRow>=0) {
            if (col-1>=0&&board[pawnRow][col-1]=='p') {
                return true;
            }
            if (col+1<8&&board[pawnRow][col+1]=='p') {
                return true;
            }
        }
    }
    // A king attacks all 8 surrounding squares.
    char atkKing = (attacker==WHITE)?'K':'k';
    for (int dr=-1;dr<=1;++dr){
        for (int dc=-1;dc<=1;++dc){
            // Skip the same square
            if (dr==0&&dc==0) {
                continue;
            }
            int r=row+dr;
            int c=col+dc;
            if (r>=0&&r<8&&c>=0&&c<8&&board[r][c]==atkKing){
                return true;
            }
        }
    }
    return false;
}
bool Board::findKing(int side,int& row,int& col)const{
    char king=(side == WHITE)?'K':'k';
    for (int r=0;r<BOARD_SIZE;++r){
        for (int c=0;c<BOARD_SIZE;++c){
            if (board[r][c]==king){ 
                row=r; 
                col=c; 
                return true; 
            }
        }
    }
    return false;
}
bool Board::isWhite(char p){
    return p >= 'A' && p <= 'Z' && p != EMPTY;
}
bool Board::isBlack(char p){ 
    return p >= 'a' && p <= 'z'; 
}
bool Board::isEmpty(char p){ 
    return p == EMPTY; 
}
bool Board::sameColor(char a,char b){
    return (isWhite(a)&&isWhite(b))||(isBlack(a)&&isBlack(b));
}
std::string Board::serialize() const {
    std::ostringstream oss;
    for (int r=0;r<BOARD_SIZE;++r) {
        for (int c=0;c<BOARD_SIZE;++c){
            // give space so we can direcltly use iss
            oss <<board[r][c]<<' ';
        }
        oss <<'\n';
    }
    oss <<sideToMove<<'\n';
    oss <<castlingRights<<'\n';
    oss <<enPassantCol<<'\n';
    oss <<halfMoveClock<<'\n';
    oss <<fullMoveNumber<<'\n';
    oss <<whiteMoves.size()<<'\n';
    for (auto& m:whiteMoves){
        oss<<m.toAlgebraic()<<'\n';
    }
    oss <<blackMoves.size()<<'\n';
    for (auto& m:blackMoves){
        oss << m.toAlgebraic() << '\n';
    }
    oss << capturedByWhite.size()<<'\n';
    for (char c:capturedByWhite){
        oss << c;
    }
    oss << '\n';
    oss << capturedByBlack.size() << '\n';
    for (char c : capturedByBlack){
        oss << c;
    }
    oss << '\n';
    return oss.str();
}
// minimise the use of  iss 
bool Board::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string line;
    // Read board
    // serialize() writes: board[r][c] << ' '
    // so we must read using >>, not getline + line[c]
    for (int r=0;r<BOARD_SIZE;++r) {
        for (int c=0;c<BOARD_SIZE;++c) {
            if (!(iss >> board[r][c]))
                return false;
        }
    }
    // Read basic game state
    if (!(iss >> sideToMove >> castlingRights >> enPassantCol
              >> halfMoveClock >> fullMoveNumber))
        return false;
    // Remove leftover newline
    iss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    // Read white moves
    int wCount;
    if (!(iss >> wCount))
        return false;
    iss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    whiteMoves.clear();
    for (int i = 0; i < wCount; ++i) {
        if (!std::getline(iss, line))
            return false;
        if (line.size() < 4)
            return false;
        Move m;
        m.fromCol=line[0]-'a';
        m.fromRow='8'-line[1];
        m.toCol=line[2]-'a';
        m.toRow='8'-line[3];
        m.promotion=(line.size()>4)?line[4]:EMPTY;
        whiteMoves.push_back(m);
    }
    // Read black moves
    int bCount;
    if (!(iss >> bCount))
        return false;
    iss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    blackMoves.clear();
    for (int i = 0; i < bCount; ++i) {
        if (!std::getline(iss, line))
            return false;
        if (line.size()<4)
            return false;
        Move m;
        m.fromCol = line[0] - 'a';
        m.fromRow = '8' - line[1];
        m.toCol   = line[2] - 'a';
        m.toRow   = '8' - line[3];
        m.promotion = (line.size() > 4) ? line[4] : EMPTY;
        blackMoves.push_back(m);
    }
    // Read captured by white
    int capWCount;
    if (!(iss >> capWCount))
        return false;
    iss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    capturedByWhite.clear();
    if (!std::getline(iss, line))
        return false;
    if ((int)line.size() != capWCount)
        return false;
    for (char c : line)
        capturedByWhite.push_back(c);
    // Read captured by black
    int capBCount;
    if (!(iss >> capBCount))
        return false;
    iss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    capturedByBlack.clear();
    if (!std::getline(iss, line))
        return false;
    if ((int)line.size() != capBCount)
        return false;
    // we didnt use ' ' space while pushing into >> so iss doesnot work 
    for (char c : line)
        capturedByBlack.push_back(c);
    // Clear undo/history stack
    while (!historyStack.empty())
        historyStack.pop();
    return true;
}
void Board::loadFromRaw(const char raw[BOARD_SIZE][BOARD_SIZE], int side, int castle, int ep, int halfClock, int fullNum, const std::vector<Move>& wMoves, const std::vector<Move>& bMoves, const std::vector<char>& capW, const std::vector<char>& capB){
    for (int r=0;r<BOARD_SIZE; ++r){
        for (int c=0;c<BOARD_SIZE;c++){
            board[r][c]=raw[r][c];
        }
    }
    sideToMove=side;
    castlingRights=castle;
    enPassantCol=ep;
    halfMoveClock=halfClock;
    fullMoveNumber=fullNum;
    whiteMoves=wMoves;
    blackMoves=bMoves;
    capturedByWhite=capW;
    capturedByBlack=capB;
    while(!historyStack.empty()){
        // when loaded game from past dont allow redo 
        // player can cheat 
        historyStack.pop();
    }
}
