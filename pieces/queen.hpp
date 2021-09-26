#include "piece.hpp"

#ifndef QUEEN_H
#define QUEEN_H

class Queen : public Piece {
    public:
        Queen(int x, int y, int ty);
        std::string getPieceName() override;
        PieceName getPieceType() override;
        int getXTranslation() override;
        bool isValidMove(Game& game, int newX, int newY) override;
        vector<unique_ptr<Move>> getMoves(Game& game) override;
        int getPieceValue() override;
        bool isSlidingPiece() override;
        void setup(Game& game) override;
        void cleanThreats(Game& game) override;
        void updateThreats(Game& game, int newX, int newY, Piece* captured) override;
};

#endif