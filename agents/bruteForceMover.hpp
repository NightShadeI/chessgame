#include "agent.hpp"

#ifndef BRUTE_FORCE_MOVER_H
#define BRUTE_FORCE_MOVER_H

class BruteForceMover : public Agent {
    public:
        BruteForceMover(int d);
        int depth;
        int movesExplored;
        Move* bestMove;
        inline int movementScore(Move* m);
        int quiescence(Game& game, int mult, int alpha, int beta, int d = 0);
        int bruteForce(Game& game, int mult, int alpha, int beta, int d = 0, bool allowInvalid=true);
        Move& getMove(Game& game) override;
};

#endif