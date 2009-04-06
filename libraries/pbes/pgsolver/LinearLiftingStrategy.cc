#include "LinearLiftingStrategy.h"

LinearLiftingStrategy::LinearLiftingStrategy(
    const ParityGame &game, bool backward )
    : LiftingStrategy(game), backward_(backward), failed_lifts_(0)
{
}

verti LinearLiftingStrategy::next(verti prev_vertex, bool prev_lifted)
{
    const verti last_vertex = graph_.V() - 1;

    if (prev_vertex == NO_VERTEX)
    {
        /* First vertex; pick either first or last depending on direction. */
        return backward_ ? last_vertex : 0;
    }

    if (prev_lifted)
    {
        failed_lifts_ = 0;
    }
    else
    {
        failed_lifts_ += 1;
        if (failed_lifts_ == graph_.V()) return NO_VERTEX;
    }

    if (backward_)
    {
        return prev_vertex == 0 ? last_vertex : prev_vertex - 1;
    }
    else
    {
        return prev_vertex == last_vertex ? 0 : prev_vertex + 1;
    }
}
