#pragma once
#include"Board.h"
#include"Move.h"
#include<vector>
class MoveGenerator {
    public:
        // vector to store legal moves
        std::vector<Move> generateLegalMoves(Board& board) const;
        bool hasLegalMove(Board& board) const;
        std::vector<Move> generateCaptureMoves(const Board& board) const;
    private:
        // generate all moves doesnot care if we are in check or not to make it into legal move we must make sure that after the move the king is not in check 
        std::vector<Move> generatePseudoLegal(const Board& board) const;
        void generatePawnMoves(const Board& b, int r, int c, std::vector<Move>& moves) const;
        void generateKnightMoves(const Board& b, int r, int c, std::vector<Move>& moves) const;
        void generateBishopMoves(const Board& b, int r, int c, std::vector<Move>& moves) const;
        void generateRookMoves(const Board& b, int r, int c, std::vector<Move>& moves) const;
        void generateQueenMoves(const Board& b, int r, int c, std::vector<Move>& moves) const;
        void generateKingMoves(const Board& b, int r, int c, std::vector<Move>& moves) const;
        // pieces like rook,queen,bishop.. move in a direction a specific direction until its valid so we use this sliding to see those moves
        void generateSliding(const Board& b, int r, int c, int dr, int dc, std::vector<Move>& moves) const;
        void addPromotions(const Board& b, int fr, int fc, int tr, int tc, char captured, std::vector<Move>& moves) const;
        // this can be moved into utlis.h 
        // static as it doesnot depend on object 
        static bool inBounds(int r, int c);
        // these are important for quiesence search after reaching a depth==0 to avoid horizon effect we use this to see any violent effect of moves are possible for any pieces like capturing high level piece or promotion
        void generatePawnCaptures(const Board& b, int r, int c, std::vector<Move>& moves) const;
        void generateKnightCaptures(const Board& b, int r, int c, std::vector<Move>& moves) const;
        void generateBishopCaptures(const Board& b, int r, int c, std::vector<Move>& moves) const;
        void generateRookCaptures(const Board& b, int r, int c, std::vector<Move>& moves) const;
        void generateQueenCaptures(const Board& b, int r, int c, std::vector<Move>& moves) const;
        void generateKingCaptures(const Board& b, int r, int c, std::vector<Move>& moves) const;
        void generateSlidingCaptures(const Board& b, int r, int c, int dr, int dc, std::vector<Move>& moves) const;
};
