// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef VERTEX_PARTITION_H_INCLUDED
#define VERTEX_PARTITION_H_INCLUDED

#include "Graph.h"
#include "RefCounted.h"
#include <assert.h>

/*! \ingroup ParityGameData

    A vertex partition is a functor that maps vertices onto worker processes.

    Currently one type of partition is supported, that assigns fixed-size chunks
    of consecutive vertices onto increasing worker processes, wrapping around
    as needed.

    Typical chunk sizes could be 1 or ceil(V/workers), or maybe some arbitrary
    large value.
*/
class VertexPartition : public RefCounted
{
public:
    VertexPartition(int num_procs, verti chunk_size)
        : num_procs_(num_procs), chunk_size_(chunk_size)
    {
        assert(num_procs > 0);
        assert(chunk_size > 0);

        // Check if overflow could occur:
        assert(verti(chunk_size*num_procs)/chunk_size == verti(num_procs));
    }

    //! Returns to which process vertex `v` is assigned
    int operator()(verti v) const
    {
        return v/chunk_size_%num_procs_;
    }

    //! Returns the first vertex assigned to `proc`
    verti first(int proc) const
    {
        return chunk_size_*proc;
    }

    /*! Given a vertex `v` assigned to `proc`, returns the next vertex assigned
        to the same process. */
    verti next(int proc, verti v) const
    {
        (void)proc;  // unused
        if (++v%chunk_size_ == 0) v += chunk_size_*(num_procs_ - 1);
        return v;
    }

    //! Returns how many of the vertices in range [0..V) are assigned to `proc`.
    verti num_assigned(verti V, int proc) const
    {
        verti x = chunk_size_ * num_procs_;
        verti res = V/x * chunk_size_;
        verti rem = V%x;
        if (rem > chunk_size_*proc)
        {
            rem -= chunk_size_*proc;
            if (rem > chunk_size_) rem = chunk_size_;
            res += rem;
        }
        return res;
    }

    //! Returns the number of processes.
    int num_procs() const { return num_procs_; }

    /*! Returns the vertex chunk size.
        A chunk is a range of consecutive vertices assigned to the same process.
    */
    verti chunk_size() const { return chunk_size_; }

private:
    int num_procs_;         //! Number of processes.
    verti chunk_size_;      //! Size of vertex chunks.
};

#endif /* ndef VERTEX_PARTITION_H_INCLUDED */
