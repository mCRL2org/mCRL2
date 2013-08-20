// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Don't include this file directly! Include SmallProgressMeasures.h instead.

#include <vector>
#include <deque>

inline int SmallProgressMeasures::vector_cmp(verti v, verti w, size_t N) const
{
    return vector_cmp(vec(v), vec(w), N);
}

inline int SmallProgressMeasures::vector_cmp( const verti vec1[],
                                              const verti vec2[], size_t N ) const
{
    if (is_top(vec1)) return is_top(vec2) ? 0 : +1;  // v is top
    if (is_top(vec2)) return -1;                     // w is top, but v isn't

    for (size_t n = 0; n < N; ++n)
    {
        if (vec1[n] < vec2[n]) return -1;
        if (vec1[n] > vec2[n]) return +1;
    }

    return 0;
}

inline verti SmallProgressMeasures::get_ext_succ(verti v, bool take_max) const
{
    const verti *it  = game_.graph().succ_begin(v),
                *end = game_.graph().succ_end(v);

    assert(it != end);  /* assume we have at least one successor */

    size_t N = len(v);
    verti res = *it++;
    for ( ; it != end; ++it)
    {
        int d = vector_cmp(*it, res, N);
        if (take_max ? d > 0 : d < 0) res = *it;
    }
    return res;
}

template<class OutputIterator>
void SmallProgressMeasures::get_winning_set( ParityGame::Player player,
                                             OutputIterator result )
{
    const StaticGraph &graph = game_.graph();
    const verti V = graph.V();

    if (player == p_)
    {
        // Conservatively estimate vertices won by player.
        std::vector<char> marked(V, 0);
        std::vector<char> queued(V, 0);
        std::deque<verti> dirty;
        for (verti v = 0; v < V; ++v)
        {
            if (is_top(v))
            {
                marked[v] = true;
            }
            else
            {
                queued[v] = true;
                dirty.push_back(v);
            }
        }
        while (!dirty.empty())
        {
            const verti v = dirty.front();
            dirty.pop_front();
            assert(queued[v] && !marked[v]);
            queued[v] = false;
            if ((int)game_.player(v) == p_)
            {
                // Look for an unmarked successor with a progress value
                // less than (or equal to, if priority is even) that of v:
                bool mark = true;
                for ( StaticGraph::const_iterator it = graph.succ_begin(v);
                      it != graph.succ_end(v); ++it )
                {
                    if ( !marked[*it] && vector_cmp(v, *it, len(v))
                            >= static_cast<int>(game_.priority(v)%2 != p_) )
                    {
                        mark = false;
                        break;
                    }
                }
                marked[v] = mark;
            }
            else  // v is controlled by opponent
            {
                // Look for a marked successor, or an unmarked one with a
                // progress value less than (or equal to, if priority is
                // even) that of v:
                for ( StaticGraph::const_iterator it = graph.succ_begin(v);
                      it != graph.succ_end(v); ++it )
                {
                    const verti w = *it;
                    if ( marked[*it] || vector_cmp(v, w, len(v))
                            < static_cast<int>(game_.priority(v)%2 != p_) )
                    {
                        marked[v] = true;
                        break;
                    }
                }
            }
            if (marked[v])
            {
                // Mark possibly losing vertex and queue its predecessors:
                for ( StaticGraph::const_iterator it = graph.pred_begin(v);
                      it != graph.pred_end(v); ++it )
                {
                    if (!marked[*it] && !queued[*it])
                    {
                        queued[*it] = true;
                        dirty.push_back(*it);
                    }
                }
            }
        }
        // Now collect guaranteed winning vertices:
        for (verti v = 0; v < V; ++v)
        {
            if (!marked[v]) *result++ = v;
        }
    }
    else
    {
        // All vertices with Top progress measures are won by opponent:
        for (verti v = 0; v < V; ++v)
        {
            if (is_top(v)) *result++ = v;
        }
    }
}

bool SmallProgressMeasures::lift_to_top(verti v)
{
    if (is_top(v)) return false;
    set_top(v);
    ls_->lifted(v);
    return true;
}

void SmallProgressMeasures::set_top(verti v)
{
    assert(!is_top(v));
    vec(v)[0] = NO_VERTEX;
    size_t prio = game_.priority(v);
    if (prio%2 != p_) decr_M(prio/2);
}
