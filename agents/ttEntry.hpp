#include <memory>
#include <optional>

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
    std::optional<Move> bestMove;
    int score;
    int depth;
};

#endif