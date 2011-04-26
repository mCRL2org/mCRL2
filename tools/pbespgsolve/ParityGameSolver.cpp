#include "ParityGameSolver.h"
#include <assert.h>

void merge_strategies( std::vector<verti> &strategy,
                       const std::vector<verti> &substrat,
                       const std::vector<verti> &vertex_map )
{
    assert(substrat.size() == vertex_map.size());
    for (verti v = 0; v < (verti)vertex_map.size(); ++v)
    {
        strategy[vertex_map[v]] =
            (substrat[v] == NO_VERTEX) ? NO_VERTEX : vertex_map[substrat[v]];
    }
}
