#include "piece.hpp"

#ifndef PAWN_H
#define PAWN_H

class Pawn : public Piece {
    public:
        Pawn(int x, int y, int ty);
        std::string getPieceName() override;
        PieceName getPieceType() override;
        int getXTranslation() override;
        bool isValidMove(Game& game, int newX, int newY);
        vector<unique_ptr<Move>> getMoves(Game& game) override;
        int getPieceValue() override;
        void setup(Game& game) override;
        void cleanThreats(Game& game) override;
        void updateThreats(Game& game, int newX, int newY, Piece* captured) override;
    private:
        inline unique_ptr<Move> generateMove(Game& game, int newX, int newY);
};

#endif