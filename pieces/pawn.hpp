#include "piece.hpp"

#ifndef PAWN_H
#define PAWN_H

class Pawn : public Piece {
    public:
        Pawn(int x, int y, int ty);
        std::string getPieceName() override;
        int getXTranslation() override;
        bool isValidMove(Game& game, int newX, int newY);
        std::vector<Move*> getMoves(Game& game) override;
        int getPieceValue() override;
    private:
        inline Move* generateMove(Game& game, int newX, int newY);
};

#endif