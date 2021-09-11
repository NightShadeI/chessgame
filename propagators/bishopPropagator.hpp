#include "propagator.hpp"

#ifndef BISHOP_PROPAGATOR_H
#define BISHOP_PROPAGATOR_H

class BishopPropagator : public Propagator {
    public:
        BishopPropagator(Piece* rootPiece, Piece* targetPiece);
        void propagate() override;
};

#endif