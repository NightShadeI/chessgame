#include <vector>

#include "pieces/piece.hpp"

#ifndef MOVE_H
#define MOVE_H

const std::uint64_t WHITE_KINGSIDE_CASTLE_MASK = 6917529027641081856;
const std::uint64_t WHITE_QUEENSIDE_CASTLE_MASK = 1008806316530991104;
const std::uint64_t BLACK_KINGSIDE_CASTLE_MASK = 96;
const std::uint64_t BLACK_QUEENSIDE_CASTLE_MASK = 14;

enum MoveType {
    NORMAL,
    WHITE_KINGSIDE_CASTLE,
    WHITE_QUEENSIDE_CASTLE,
    BLACK_KINGSIDE_CASTLE,
    BLACK_QUEENSIDE_CASTLE
};

struct Move
{
    Piece* moved;
    Piece* captured;
    int xFrom;
    int yFrom;
    int xTo;
    int yTo;
    MoveType moveType = MoveType::NORMAL;

    inline static MoveType getMoveType(Piece& aPiece, const int newX, const int newY) {
        if (aPiece.getPieceType() != PieceName::KING) return MoveType::NORMAL;
        if (aPiece.totalMoves > 0) return MoveType::NORMAL;
        if (newY == 0) {
            if (newX == 2) return MoveType::BLACK_QUEENSIDE_CASTLE;
            if (newX == 6) return MoveType::BLACK_KINGSIDE_CASTLE;
        } else if (newY == 7) {
            if (newX == 2) return MoveType::WHITE_QUEENSIDE_CASTLE;
            if (newX == 6) return MoveType::WHITE_KINGSIDE_CASTLE;
        }
        return MoveType::NORMAL;
    }
};

const std::vector<bool> illegalMoveDetection {

    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,

    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, true , true , true,  false,

    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, true , true , true , false, false, false,

    false, false, false, false, true , true , true , false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,

    false, false, true , true , true , false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,

};

#endif
