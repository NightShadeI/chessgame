#include "piece.hpp"

#ifndef BISHOP_H
#define BISHOP_H

class Bishop : public Piece {
    public:
        Bishop(int x, int y, int ty);
        inline std::string getPieceName() override { return "Bishop"; }
        inline PieceName getPieceType() override { return PieceName::BISHOP; }
        inline int getXTranslation() override { return 8; }
        bool isValidMove(Game& game, int newX, int newY) override;
        inline int getPieceValue() override { return 30; }
        inline bool isSlidingPiece() override { return true; }
        vector<unique_ptr<Move>> getMoves(Game& game) override;
};

#endif