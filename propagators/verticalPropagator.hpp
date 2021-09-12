#include "propagator.hpp"

#ifndef VERTICAL_PROPAGATOR_H
#define VERTICAL_PROPAGATOR_H

class VerticalPropagator : public Propagator {
    public:
        VerticalPropagator(Piece* rootPiece, Move* movePerformed);
        void openPropagation(Game& game) override;
        void closePropagation(Game& game) override;
};

#endif