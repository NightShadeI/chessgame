#include "propagator.hpp"

#ifndef HORIZONTAL_PROPAGATOR_H
#define HORIZONTAL_PROPAGATOR_H

class HorizontalPropagator : public Propagator {
    public:
        HorizontalPropagator(Piece* rootPiece, Move* movePerformed);
        void openPropagation(Game& game) override;
        void closePropagation(Game& game) override;
};

#endif