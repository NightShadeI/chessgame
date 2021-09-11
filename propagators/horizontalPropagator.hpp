#include "propagator.hpp"

#ifndef HORIZONTAL_PROPAGATOR_H
#define HORIZONTAL_PROPAGATOR_H

class HorizontalPropagator : public Propagator {
    public:
        HorizontalPropagator(Piece* rootPiece, Piece* targetPiece);
        void propagate(Game& game) override;
};

#endif