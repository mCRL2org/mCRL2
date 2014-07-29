// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Don't include this directly; include ParityGameSolver.h instead!

template<class ForwardIterator>
void merge_vertex_maps( ForwardIterator begin, ForwardIterator end,
                        const verti *old_map, verti old_map_size )
{
    while (begin != end)
    {
        verti &v = *begin++;
        v = (v < old_map_size) ? old_map[v] : NO_VERTEX;
    }
}
