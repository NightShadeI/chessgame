#include "piece.hpp"

#ifndef KNIGHT_H
#define KNIGHT_H

class Knight : public Piece {
    public:
        Knight(int x, int y, int ty);
        std::string getPieceName() override;
        bool isValidMove(Game& game, int newX, int newY) override;
        std::vector<Move*> getMoves(Game& game) override;
        int getPieceValue() override;
};

#endif