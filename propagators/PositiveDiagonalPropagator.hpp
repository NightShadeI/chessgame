#include "propagator.hpp"

#ifndef POSITIVE_DIAGONAL_PROPAGATOR_H
#define POSITIVE_DIAGONAL_PROPAGATOR_H

class PositiveDiagonalPropagator : public Propagator {
    public:
        PositiveDiagonalPropagator(Piece* rootPiece, Move* movePerformed);
        void openPropagation(Game& game) override;
        void closePropagation(Game& game) override;
};

#endif