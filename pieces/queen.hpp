#include "piece.hpp"

#ifndef QUEEN_H
#define QUEEN_H

class Queen : public Piece {
    public:
        Queen(int x, int y, int ty);
        std::string getPieceName() override;
        int getXTranslation() override;
        bool isValidMove(Game& game, int newX, int newY) override;
        std::vector<Move*> getMoves(Game& game) override;
        int getPieceValue() override;
        void setup(Game& game) override;
        void updateThreats(Game& game, int newX, int newY, Piece* captured) override;
};

#endif