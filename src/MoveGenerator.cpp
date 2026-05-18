#include "MoveGenerator.h"
#include <cctype>
std::vector<Move> MoveGenerator::generateLegalMoves(Board& board)const{
    std::vector<Move> pseudo=generatePseudoLegal(board);
    std::vector<Move> legal;
    legal.reserve(pseudo.size());
    int side=board.sideToMove;
    for (const Move& m : pseudo) {
        // CHANGED:
        // Old code:
        //     for (const Move& m : pseudo) {
    //     Board copy = board;
    //     copy.applyMove(m);
    //     if (!copy.isInCheck(side)) {
    //         legal.push_back(m);
    //     }
    // }.
    //     //
        // New code:
        //     Temporarily make the move on the same board,
        //     check if our own king is safe,
        //     then undo the move.
        //
        // This is much faster than copying the full Board every time.
        board.makeMove(m);
        if (!board.isInCheck(side)) {
            legal.push_back(m);
        }
        // CHANGED:
        // Restore board exactly to previous position.
        board.unmakeMove(m);
    }
    return legal;
}
bool MoveGenerator::hasLegalMove(Board& board) const {
    std::vector<Move> pseudo = generatePseudoLegal(board);
    int side = board.sideToMove;
    for (const Move& m : pseudo) {
        // CHANGED:
        // optimised no need to call generatelegal moves all the time return True just after finding one
        // No Board copy. Use make/unmake.
        board.makeMove(m);
        bool legal = !board.isInCheck(side);
        // IMPORTANT:
        // Always unmake before returning.
        board.unmakeMove(m);
        if (legal) {
            return true;
        }
    }
    return false;
}
// Capture-Only Generation (For Quiescence Search)
std::vector<Move> MoveGenerator::generateCaptureMoves(const Board& board) const {
    std::vector<Move> moves;
    // updated by &
    moves.reserve(30);
    int side=board.sideToMove;
    for (int r=0;r<BOARD_SIZE;++r) {
        for (int c=0;c<BOARD_SIZE;++c) {
            char p=board.board[r][c];
            if (p==EMPTY){
                continue;
            }
            bool isOurs=(side==WHITE)?Board::isWhite(p):Board::isBlack(p);
            if (!isOurs){
                continue;
            }
            char upper=static_cast<char>(std::toupper(static_cast<unsigned char>(p)));
            switch (upper) {
                case 'P': generatePawnCaptures(board, r, c, moves); break;
                case 'N': generateKnightCaptures(board, r, c, moves); break;
                case 'B': generateBishopCaptures(board, r, c, moves); break;
                case 'R': generateRookCaptures(board, r, c, moves); break;
                case 'Q': generateQueenCaptures(board, r, c, moves); break;
                case 'K': generateKingCaptures(board, r, c, moves); break;
            }
        }
    }
    return moves;
}
void MoveGenerator::generatePawnCaptures(const Board& b,int r,int c,std::vector<Move>& moves) const {
    char p=b.board[r][c];
    int side=b.sideToMove;
    int dir=(side==WHITE)?-1:1;
    int promoteRow=(side==WHITE)?0:7;
    int nr=r+dir;
    for (int dc:{-1,1}) {
        int nc=c+dc;
        if (!inBounds(nr, nc)) continue;
        char target=b.board[nr][nc];
        // Normal capture
        if (target != EMPTY && !Board::sameColor(p, target)) {
            if (nr==promoteRow) {
                // check for every possible promotion and emplace back everything
                addPromotions(b, r, c, nr, nc, target, moves);
            } else {
                moves.emplace_back(r, c, nr, nc, p, target);
            }
        }
        // En passant capture
        if (b.enPassantCol==nc) {
            int epRow=(side==WHITE)?3:4;
            if (r==epRow) {
                char epPawn=b.board[epRow][nc];
                if (epPawn != EMPTY && !Board::sameColor(p, epPawn)) {
                    moves.emplace_back(r, c, nr, nc, p, epPawn, EMPTY, true, false);
                }
            }
        }
    }
}
void MoveGenerator::generateKnightCaptures(const Board& b,int r,int c,std::vector<Move>& moves) const {
    char p = b.board[r][c];
    // Made arrays static const so they are not recreated every function call.
    static const int dr[8]={-2,-2,-1,-1,1,1,2,2};
    static const int dc[8]={-1,1,-2,2,-2,2,-1,1};
    for (int i=0;i<8;++i){
        int nr=r+dr[i];
        int nc=c+dc[i];
        if (!inBounds(nr,nc)){
            continue;
        }
        char target=b.board[nr][nc];
        if (target!=EMPTY && !Board::sameColor(p,target)) {
            moves.emplace_back(r, c, nr, nc, p, target);
        }
    }
}
void MoveGenerator::generateSlidingCaptures(const Board& b,int r,int c,int dr,int dc,std::vector<Move>& moves)const{
    char p=b.board[r][c];
    int nr=r+dr;
    int nc=c+dc;
    while (inBounds(nr,nc)) {
        char target=b.board[nr][nc];
        if (target!=EMPTY) {
            if (!Board::sameColor(p,target)) {
                moves.emplace_back(r,c,nr,nc,p,target);
            }
            break;
        }
        nr+=dr;
        nc+=dc;
    }
}
void MoveGenerator::generateBishopCaptures(const Board& b,int r,int c,std::vector<Move>& moves)const {
    // now assign the possible directions 
    generateSlidingCaptures(b,r,c,-1,-1,moves);
    generateSlidingCaptures(b,r,c,-1, 1,moves);
    generateSlidingCaptures(b,r,c,1,-1,moves);
    generateSlidingCaptures(b,r,c, 1,1,moves);
}
void MoveGenerator::generateRookCaptures(const Board& b,int r,int c,std::vector<Move>& moves)const {
    generateSlidingCaptures(b,r,c,-1,0,moves);
    generateSlidingCaptures(b,r,c,1,0,moves);
    generateSlidingCaptures(b,r,c,0,-1,moves);
    generateSlidingCaptures(b,r,c,0,1,moves);
}
void MoveGenerator::generateQueenCaptures(const Board& b,int r,int c,std::vector<Move>& moves)const{
    generateBishopCaptures(b,r,c,moves);
    generateRookCaptures(b,r,c,moves);
}
void MoveGenerator::generateKingCaptures(const Board& b,int r,int c,std::vector<Move>& moves)const{
    char p=b.board[r][c];
    for (int dr=-1;dr<=1;++dr){
        for (int dc=-1;dc<=1;++dc){
            if (dr==0 && dc==0){
                continue;
            }
            int nr=r+dr;
            int nc=c+dc;
            if (!inBounds(nr, nc)){ 
                continue;
            }
            char target=b.board[nr][nc];
            if (target!=EMPTY && !Board::sameColor(p,target)){
                moves.emplace_back(r, c, nr, nc, p, target);
            }
        }
    }
}
std::vector<Move> MoveGenerator::generatePseudoLegal(const Board& board) const {
    std::vector<Move> moves;
    moves.reserve(64);
    int side=board.sideToMove;
    for (int r=0;r<BOARD_SIZE;++r) {
        for (int c=0;c<BOARD_SIZE;++c) {
            char p=board.board[r][c];
            if (p==EMPTY){ 
                continue;
            }
            bool isOurs=(side==WHITE)?Board::isWhite(p) : Board::isBlack(p);
            if (!isOurs){
                continue;
            }
            char upper=static_cast<char>(std::toupper(static_cast<unsigned char>(p)));
            switch (upper) {
                case 'P': generatePawnMoves(board,r,c,moves); break;
                case 'N': generateKnightMoves(board,r,c,moves); break;
                case 'B': generateBishopMoves(board,r,c,moves); break;
                case 'R': generateRookMoves(board,r,c,moves); break;
                case 'Q': generateQueenMoves(board,r,c,moves); break;
                case 'K': generateKingMoves(board,r,c,moves); break;
            }
        }
    }
    return moves;
}
void MoveGenerator::generatePawnMoves(const Board& b,int r,int c,std::vector<Move>& moves)const{
    char p=b.board[r][c];
    int side=b.sideToMove;
    int dir=(side==WHITE)?-1:1;
    int startRow=(side==WHITE)?6:1;
    int promoteRow=(side==WHITE)?0:7;
    int nr=r+dir;
    // One-square pawn move
    if (inBounds(nr,c)&&b.board[nr][c]==EMPTY){
        if(nr==promoteRow){
            addPromotions(b,r,c,nr,c,EMPTY,moves);
        }else{
            moves.emplace_back(r,c,nr,c,p);
        }
        // Two-square pawn move
        if (r==startRow) {
            int nr2=r+2*dir;
            if (inBounds(nr2, c)&&b.board[nr2][c]==EMPTY) {
                moves.emplace_back(r,c,nr2,c,p);
            }
        }
    }
    // opp cature
    for (int dc:{-1,1}) {
        int nc=c+dc;
        if (!inBounds(nr,nc)){
             continue;
        }
        char target=b.board[nr][nc];
        // Normal capture
        if (target!=EMPTY&&!Board::sameColor(p, target)) {
            if (nr==promoteRow) {
                addPromotions(b,r,c,nr,nc,target,moves);
            } else {
                moves.emplace_back(r,c,nr,nc,p,target);
            }
        }
        // En passant
        if (b.enPassantCol==nc) {
            int epRow=(side==WHITE)?3:4;
            if (r==epRow) {
                char epPawn=b.board[epRow][nc];
                if (epPawn!=EMPTY && !Board::sameColor(p, epPawn)) {
                    moves.emplace_back(r,c,nr,nc,p,epPawn,EMPTY,true,false);
                }
            }
        }
    }
}

void MoveGenerator::addPromotions(const Board& b,int fr,int fc,int tr,int tc,char captured,std::vector<Move>& moves)const{
    char p=b.board[fr][fc];
    bool white=Board::isWhite(p);
    const char promos[]={white ? W_QUEEN:B_QUEEN,white ? W_ROOK:B_ROOK,white ? W_BISHOP:B_BISHOP,white ? W_KNIGHT:B_KNIGHT};
    for (char promo:promos) {
        moves.emplace_back(fr,fc,tr,tc,p,captured,promo);
    }
}
void MoveGenerator::generateKnightMoves(const Board& b,int r,int c,std::vector<Move>& moves)const{
    char p=b.board[r][c];
    // static const avoids recreating these arrays every call.
    static const int dr[8]={-2,-2,-1,-1, 1, 1, 2, 2};
    static const int dc[8]={-1, 1,-2, 2,-2, 2,-1,1};
    for (int i=0;i<8;++i) {
        int nr=r+dr[i];
        int nc=c+dc[i];
        if (!inBounds(nr,nc)){
            continue;
        }
        char target=b.board[nr][nc];
        if (target == EMPTY || !Board::sameColor(p,target)) {
            moves.emplace_back(r,c,nr,nc,p,target);
        }
    }
}
void MoveGenerator::generateSliding(const Board& b,int r,int c,int dr,int dc,std::vector<Move>& moves) const {
    char p=b.board[r][c];
    int nr=r+dr;
    int nc=c+dc;
    while(inBounds(nr, nc)){
        char target=b.board[nr][nc];
        if (target==EMPTY){
            moves.emplace_back(r,c,nr,nc,p);
        } else {
            if (!Board::sameColor(p,target)) {
                moves.emplace_back(r,c,nr,nc,p,target);
            }
            break;
        }
        nr+=dr;
        nc+=dc;
    }
}

void MoveGenerator::generateBishopMoves(const Board& b,int r,int c,std::vector<Move>& moves)const{
    generateSliding(b,r,c,-1,-1,moves);
    generateSliding(b, r, c, -1,1,moves);
    generateSliding(b,r,c,1,-1,moves);
    generateSliding(b,r,c,1,1,moves);
}

void MoveGenerator::generateRookMoves(const Board& b,int r,int c,std::vector<Move>& moves)const{
    generateSliding(b,r,c, -1,0,moves);
    generateSliding(b,r,c,1,0,moves);
    generateSliding(b,r,c,0,-1,moves);
    generateSliding(b,r,c,0,1,moves);
}

void MoveGenerator::generateQueenMoves(const Board& b,int r,int c,std::vector<Move>& moves)const{
    generateBishopMoves(b,r,c,moves);
    generateRookMoves(b,r,c,moves);
}
void MoveGenerator::generateKingMoves(const Board& b,int r,int c,std::vector<Move>& moves)const{
    char p=b.board[r][c];
    int side=b.sideToMove;
    // Normal king moves
    for (int dr=-1;dr<=1;++dr) {
        for (int dc=-1;dc<=1;++dc) {
            if (dr==0&&dc==0){
                continue;
            }
            int nr=r+dr;
            int nc=c+dc;
            if (!inBounds(nr,nc)){
                continue;
            }
            char target=b.board[nr][nc];
            if (target==EMPTY||!Board::sameColor(p,target)) {
                moves.emplace_back(r,c,nr,nc,p,target);
            }
        }
    }
    // Castling
    int backRank=(side==WHITE)?7:0;
    if (r != backRank||c!=4)return;
    // Cannot castle while already in check.
    if (b.isInCheck(side)) return;
    // King-side castling
    int ksRight=(side==WHITE)?CASTLE_WK:CASTLE_BK;
    if ((b.castlingRights&ksRight)&&b.board[backRank][5]==EMPTY&&b.board[backRank][6]==EMPTY&&!b.isSquareAttacked(backRank,5,side^1)&&!b.isSquareAttacked(backRank,6,side^1)){
        moves.emplace_back(r,c,backRank,6,p,EMPTY,EMPTY,false,true);
    }
    int qsRight=(side==WHITE)?CASTLE_WQ:CASTLE_BQ;
    if ((b.castlingRights&qsRight)&&b.board[backRank][1]==EMPTY&&b.board[backRank][2]==EMPTY&&b.board[backRank][3]==EMPTY&&!b.isSquareAttacked(backRank,2,side^1) &&!b.isSquareAttacked(backRank,3,side^1)){
        moves.emplace_back(r,c,backRank,2,p,EMPTY,EMPTY,false,true);
    }
}
bool MoveGenerator::inBounds(int r,int c) {
    return r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE;
}
