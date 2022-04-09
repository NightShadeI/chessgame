#include "piece.hpp"

#ifndef KNIGHT_H
#define KNIGHT_H

class Knight : public Piece {
    public:
        Knight(int x, int y, int ty);
        inline std::string getPieceName() override { return "Knight"; }
        inline PieceName getPieceType() override { return PieceName::KNIGHT; }
        bool isValidMove(Game& game, int newX, int newY) override;
        bool isStartingLocation() override;
        vector<unique_ptr<Move>> getMoves(Game& game) override;
        inline bool isDevelopmentalPiece() override { return true; }
        inline int getPieceValue() override { return 30; }
};

#endif