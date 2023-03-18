#include "piece.hpp"

#ifndef QUEEN_H
#define QUEEN_H

class Queen : public Piece {
    public:
        Queen(int x, int y, int ty);
        inline std::string getPieceName() override { return "Queen"; }
        inline PieceName getPieceType() override { return PieceName::QUEEN; }
        inline int getXTranslation() override { return 5; }
        bool isValidMove(Game& game, int newX, int newY) override;
        bool isStartingLocation() override;
        vector<Move> getMoves(Game& game) override;
        inline int getPieceValue() override { return 90; }
        inline bool isSlidingPiece() override { return true; }
};

#endif