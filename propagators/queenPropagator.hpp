#include "propagator.hpp"

#ifndef QUEEN_PROPAGATOR_H
#define QUEEN_PROPAGATOR_H

class QueenPropagator : public Propagator {
    public:
        QueenPropagator(Piece* rootPiece, Piece* targetPiece);
        void propagate(Game& game) override;
};

#endif