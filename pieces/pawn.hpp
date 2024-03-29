#include "piece.hpp"

#ifndef PAWN_H
#define PAWN_H

class Pawn : public Piece {
    public:
        Pawn(int x, int y, int ty);
        inline std::string getPieceName() override { return "Pawn"; }
        inline PieceName getPieceType() override { return PieceName::PAWN; }
        inline int getXTranslation() override { return 14; }
        bool isValidMove(Game& game, int newX, int newY);
        bool isStartingLocation() override;
        vector<Move> getMoves(Game& game) override;
        inline int getPieceValue() override { return 10; }
    private:
        inline Move generateMove(Game& game, int newX, int newY);
};

#endif