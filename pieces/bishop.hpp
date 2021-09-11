#include "piece.hpp"

#ifndef BISHOP_H
#define BISHOP_H

class Bishop : public Piece {
    public:
        Bishop(int x, int y, int ty);
        std::string getPieceName() override;
        int getXTranslation() override;
        bool isValidMove(Game& game, int newX, int newY) override;
        int getPieceValue() override;
        std::vector<Move*> getMoves(Game& game) override;
};

#endif