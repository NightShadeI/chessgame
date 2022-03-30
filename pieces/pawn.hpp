#include "piece.hpp"

#ifndef PAWN_H
#define PAWN_H

class Pawn : public Piece {
    public:
        Pawn(int x, int y, int ty);
        inline std::string getPieceName() override { return "Pawn"; }
        inline PieceName getPieceType() override { return PieceName::PAWN; }
        inline int getXTranslation() override { return 14; }
        bool isValidMove(Game& game, int newX, int newY);
        vector<unique_ptr<Move>> getMoves(Game& game) override;
        inline int getPieceValue() override { return 10; }
        void setup(Game& game) override;
        void cleanThreats(Game& game) override;
        void updateThreats(Game& game, int newX, int newY, Piece* captured) override;
    private:
        inline unique_ptr<Move> generateMove(Game& game, int newX, int newY);
};

#endif