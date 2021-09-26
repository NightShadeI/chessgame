#include <unordered_map>
#include <memory>

#include "agent.hpp"
#include "ttEntry.hpp"

#ifndef TEST_MOVER_H
#define TEST_MOVER_H

class testAgent : public Agent {
    public:
        testAgent(int d);
        int depth;
        int movesExplored;
        int tableHits;
        Move* bestMove;
        unordered_map<unsigned long long, unique_ptr<ttEntry>> tTable;
        inline int movementScore(Move* m, Move* optimalMove);
        int quiescence(Game& game, int alpha, int beta, int mult);
        int bruteForce(Game& game, int depth, int plyFromRoot, int alpha, int beta, int mult);
        unique_ptr<Move> getMove(Game& game) override;
};

#endif