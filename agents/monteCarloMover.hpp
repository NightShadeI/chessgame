#include "agent.hpp"

#ifndef MONTE_CARLO_MOVER_H
#define MONTE_CARLO_MOVER_H

class MonteCarloMover : public Agent {
    public:
        MonteCarloMover(int d, int t);
        int depth;
        int trails;
        unique_ptr<Move> getMove(Game& game) override;
};

#endif