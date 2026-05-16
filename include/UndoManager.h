#pragma once
#include<vector>
#include "Snapshot.h"
#include "Board.h"
// Manages the undo/redo stack using full board snapshots.
// Each snapshot contains the complete game state before a move.
class UndoManager{
    public:
        static constexpr int MAX_HISTORY=200;
        // save snap shot into stack after each move 
        void saveSnapshot(const Board& board);
        bool undo(Board& board);
        bool redo(Board& board);
        // these below three can be removed now 
        bool canUndo() const{
            return !undoStack.empty();
        }
        bool canRedo() const{
            return !redoStack.empty();
        }
        void clearRedo(){
            redoStack.clear();
        }
    private:
        std::vector<Snapshot> undoStack;
        std::vector<Snapshot> redoStack;
        // these functions are static because they dont use the data of the class just uses board to copy pos
        // Copy board state into a Snapshot.
        static Snapshot capture(const Board& board);
        // Restore board from a Snapshot.
        static void restore(Board& board, const Snapshot& snap);
};
