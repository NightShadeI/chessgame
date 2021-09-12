#include "propagator.hpp"

#ifndef NEGATIVE_DIAGONAL_PROPAGATOR_H
#define NEGATIVE_DIAGONAL_PROPAGATOR_H

class NegativeDiagonalPropagator : public Propagator {
    public:
        NegativeDiagonalPropagator(Piece* rootPiece, Move* movePerformed);
        void openPropagation(Game& game) override;
        void closePropagation(Game& game) override;
};

#endif