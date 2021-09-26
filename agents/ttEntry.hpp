#include <memory>

#include "../move.hpp"

#ifndef TTENTRY_H
#define TTENTRY_H

enum ttType {
    LOWERBOUND,
    UPPERBOUND,
    EXACT
};

struct ttEntry {
    ttType type;
    unique_ptr<Move> bestMove;
    int score;
    int depth;
};

#endif