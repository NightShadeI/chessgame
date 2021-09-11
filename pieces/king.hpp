#include "piece.hpp"

#ifndef KING_H
#define KING_H

class King : public Piece {
    public:
        King(int x, int y, int ty);
        std::string getPieceName() override;
        bool isValidMove(Game& game, int newX, int newY) override;
        std::vector<Move*> getMoves(Game& game) override;
        int getPieceValue() override;
};

#endif