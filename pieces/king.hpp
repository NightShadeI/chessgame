#include "piece.hpp"

#ifndef KING_H
#define KING_H

class King : public Piece {
    public:
        King(int x, int y, int ty);
        inline std::string getPieceName() override { return "King"; }
        inline PieceName getPieceType() override { return PieceName::KING; }
        bool isValidMove(Game& game, int newX, int newY) override;
        vector<unique_ptr<Move>> getMoves(Game& game) override;
        inline int getPieceValue() override { return 1000; }
        void setup(Game& game) override;
        void cleanThreats(Game& game) override;
        void updateThreats(Game& game, int newX, int newY, Piece* captured) override;
};

#endif