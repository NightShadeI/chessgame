#include "agent.hpp"

#ifndef BRUTE_FORCE_MOVER_H
#define BRUTE_FORCE_MOVER_H

class BruteForceMover : public Agent {
    public:
        BruteForceMover(int d);
        int depth;
        int movesExplored;
        Move* bestMove;
        int bruteForce(Game& game, int mult, int alpha, int beta, int d = 0);
        Move& getMove(Game& game) override;
};

#endif