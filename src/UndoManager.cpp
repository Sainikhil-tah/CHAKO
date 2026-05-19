#include "UndoManager.h"
#include <cstring>
void UndoManager::saveSnapshot(const Board& board) {
    if ((int)undoStack.size()>=MAX_HISTORY){
        // Drop the oldest entry to keep memory bounded
        undoStack.erase(undoStack.begin());
    }
    undoStack.push_back(capture(board));
}
bool UndoManager::undo(Board& board) {
    if (undoStack.empty()){
        return false;
    }
    // Push current state onto redo stack before restoring
    redoStack.push_back(capture(board));
    restore(board, undoStack.back());
    undoStack.pop_back();
    return true;
}
bool UndoManager::redo(Board& board) {
    if (redoStack.empty()) return false;
    // Push current state onto undo stack before re-applying
    undoStack.push_back(capture(board));
    restore(board, redoStack.back());
    redoStack.pop_back();
    return true;
}
Snapshot UndoManager::capture(const Board& board){
    Snapshot snap;
    for (int r=0;r<BOARD_SIZE;++r){
        for (int c=0;c<BOARD_SIZE;++c){
            snap.board[r][c] = board.board[r][c];
        }
    }
    snap.sideToMove=board.sideToMove;
    snap.castlingRights=board.castlingRights;
    snap.enPassantCol=board.enPassantCol;
    snap.halfMoveClock=board.halfMoveClock;
    snap.fullMoveNumber=board.fullMoveNumber;
    snap.whiteMoves=board.whiteMoves;
    snap.blackMoves=board.blackMoves;
    snap.capturedByWhite=board.capturedByWhite;
    snap.capturedByBlack=board.capturedByBlack;
    return snap;
}

void UndoManager::restore(Board& board, const Snapshot& snap) {
    board.loadFromRaw(snap.board,snap.sideToMove,snap.castlingRights,snap.enPassantCol,snap.halfMoveClock,snap.fullMoveNumber,snap.whiteMoves,snap.blackMoves,snap.capturedByWhite,snap.capturedByBlack);
}
 
