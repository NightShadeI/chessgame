#include "propagator.hpp"

#ifndef POSITIVE_DIAGONAL_PROPAGATOR_H
#define POSITIVE_DIAGONAL_PROPAGATOR_H

class PositiveDiagonalPropagator : public Propagator {
    public:
        PositiveDiagonalPropagator(Piece* rootPiece, Piece* targetPiece);
        void propagate(Game& game) override;
};

#endif