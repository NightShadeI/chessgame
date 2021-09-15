#include "piece.hpp"

#ifndef ROOK_H
#define ROOK_H

class Rook : public Piece {
    public:
        Rook(int x, int y, int ty);
        std::string getPieceName() override;
        bool isValidMove(Game& game, int newX, int newY) override;
        std::vector<Move*> getMoves(Game& game) override;
        int getPieceValue() override;
        bool isSlidingPiece() override;
        void setup(Game& game) override;
        void cleanThreats(Game& game) override;
        void updateThreats(Game& game, int newX, int newY, Piece* captured) override;
};

#endif