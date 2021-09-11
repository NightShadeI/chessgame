#include "pieces/piece.hpp"

#ifndef MOVE_H
#define MOVE_H

struct Move
{
    Piece* moved;
    Piece* captured;
    int xFrom;
    int yFrom;
    int xTo;
    int yTo;
};

#endif
