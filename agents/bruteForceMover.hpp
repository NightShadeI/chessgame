#include <unordered_map>

#include "agent.hpp"
#include "ttEntry.hpp"

#ifndef BRUTE_FORCE_MOVER_H
#define BRUTE_FORCE_MOVER_H

class BruteForceMover : public Agent {
    public:
        BruteForceMover(int d);
        int depth;
        int movesExplored;
        int tableHits;
        unordered_map<unsigned long long, unique_ptr<ttEntry>> tTable;
        inline int movementScore(Move* m, Move* optimalMove);
        int quiescence(Game& game, int mult, int alpha, int beta, int d, int mateIn);
        int bruteForce(Game& game, int mult, int alpha, int beta, int d, int mateIn, bool allowInvalid=true);
        unique_ptr<Move> getMove(Game& game) override;
        void displayDebugTrace(Game& game, int toDepth);
};

#endif