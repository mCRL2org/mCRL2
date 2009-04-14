#ifndef LINEAR_LIFTING_STRATEGY_H_INCLUDED
#define LINEAR_LIFTING_STRATEGY_H_INCLUDED

#include "SmallProgressMeasures.h"

/*! A simple lifting strategy that attempts to lift vertices in order (moving
    either forward or backward) and stops when no more vertices can be lifted.

    (The Multi-Core Solver for Parity Games paper calls this "Swiping")
*/

class LinearLiftingStrategy : public LiftingStrategy
{
public:
    LinearLiftingStrategy(const ParityGame &game, bool backward);
    verti next(verti prev_vertex, bool prev_lifted);

    bool backward() const { return backward_; }

private:
    const bool backward_;       //!< indicates the direction to move
    verti failed_lifts_;        //!< number of consecutive failed lift attempts
};

#endif /* ndef LINEAR_LIFTING_STRATEGY_H_INCLUDED */
