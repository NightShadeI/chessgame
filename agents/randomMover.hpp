#include "agent.hpp"

#ifndef RANDOM_MOVER_H
#define RANDOM_MOVER_H

class RandomMover : public Agent {
    public:
        RandomMover();
        unique_ptr<Move> getMove(Game& game) override;
};

#endif