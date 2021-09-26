#include <memory>

#include "../game.hpp"
#include "../move.hpp"

#ifndef AGENT_H
#define AGENT_H

class Agent {
    public:
        Agent();
        virtual unique_ptr<Move> getMove(Game& game);
        void doMove(Game& game);
};

#endif