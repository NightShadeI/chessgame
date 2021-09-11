#include "propagator.hpp"

#ifndef VERTICAL_PROPAGATOR_H
#define VERTICAL_PROPAGATOR_H

class VerticalPropagator : public Propagator {
    public:
        VerticalPropagator(Piece* rootPiece, Piece* targetPiece);
        void propagate() override;
};

#endif