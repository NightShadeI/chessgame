#include "propagator.hpp"

#ifndef BISHOP_PROPAGATOR_H
#define BISHOP_PROPAGATOR_H

class BishopPropagator : public Propagator {
    public:
        BishopPropagator(Piece* rootPiece, Move* movePerformed);
        void propagate(Game& game) override;
};

#endif