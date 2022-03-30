#include "piece.hpp"

#ifndef ROOK_H
#define ROOK_H

class Rook : public Piece {
    public:
        Rook(int x, int y, int ty);
        inline std::string getPieceName() override { return "Rook"; }
        inline PieceName getPieceType() override { return PieceName::ROOK; }
        bool isValidMove(Game& game, int newX, int newY) override;
        vector<unique_ptr<Move>> getMoves(Game& game) override;
        inline int getPieceValue() override { return 50; }
        inline bool isSlidingPiece() override { return true; }
        void setup(Game& game) override;
        void cleanThreats(Game& game) override;
        void updateThreats(Game& game, int newX, int newY, Piece* captured) override;
};

#endif