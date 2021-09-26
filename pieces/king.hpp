#include "piece.hpp"

#ifndef KING_H
#define KING_H

class King : public Piece {
    public:
        King(int x, int y, int ty);
        std::string getPieceName() override;
        PieceName getPieceType() override;
        bool isValidMove(Game& game, int newX, int newY) override;
        vector<unique_ptr<Move>> getMoves(Game& game) override;
        int getPieceValue() override;
        void setup(Game& game) override;
        void cleanThreats(Game& game) override;
        void updateThreats(Game& game, int newX, int newY, Piece* captured) override;
};

#endif