#include "piece.hpp"

#ifndef KING_H
#define KING_H

class King : public Piece {
    public:
        King(int x, int y, int ty);
        inline std::string getPieceName() override { return "King"; }
        inline PieceName getPieceType() override { return PieceName::KING; }
        bool isValidMove(Game& game, int newX, int newY) override;
        bool isStartingLocation() override;
        vector<Move> getMoves(Game& game) override;
        inline int getPieceValue() override { return 1000; }
};

#endif