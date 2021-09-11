#include "propagator.hpp"

#ifndef NEGATIVE_DIAGONAL_PROPAGATOR_H
#define NEGATIVE_DIAGONAL_PROPAGATOR_H

class NegativeDiagonalPropagator : public Propagator {
    public:
        NegativeDiagonalPropagator(Piece* rootPiece, Piece* targetPiece);
        void propagate() override;
};

#endif