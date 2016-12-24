// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef GAME_PARTITION_H_INCLUDED
#define GAME_PARTITION_H_INCLUDED

#include "VertexPartition.h"
#include "ParityGame.h"

/*! A game partition is a subgame induced by a starting set of internal vertices
    extended with all vertices that share an edge with an internal vertex.

    In addition to storing the subgame for the extended vertex set, the partition
    stores a mapping of local to global and global to local vertex indices, and
    the local indices of the internal vertices.
*/
class GamePartition
{
public:
    typedef std::vector<verti>::const_iterator const_iterator;

public:
    /*! Construct a partition of the global game for the given process using
        the specified partition. */
    GamePartition( const ParityGame &old_game,
                   const VertexPartition &vpart, int proc );

    /*! Constructs a partition as the intersection of an existing partition with
        a vertex subset, specified as a list of vertex indices local to the
        game partition. */
    GamePartition(const GamePartition &part, const std::vector<verti> &verts);

    /*! Swaps the contents of this GamePartition with another one. */
    void swap(GamePartition &gp);

    //! Returns whether a local vertex index corresponds to an internal vertex.
    bool is_internal(verti v) const;

    /*! Returns the parity game associated for this partition, which describes
        the vertices relevant to this partition. */
    const ParityGame &game() const { return game_; }

    /*! Returns the local index for a vertex given its global index, or
        NO_VERTEX if the index does not correspond to a local vertex. */
    verti local(verti v) const
    {
        std::unordered_map<verti, verti>::const_iterator it = local_.find(v);
        if (it == local_.end()) return NO_VERTEX;
        return it->second;
    }

    /*! Returns the global index for a vertex given its local index.
        It is illegal to call this method with an argument that does not
        correspond to a vertex in the partition. */
    verti global(verti v) const
    {
        return global_[v];
    }

    /*! Returns an iterator to the beginning of the list of local indices of
        internal vertices of this partition. */
    const_iterator begin() const { return internal_.begin(); }

    /*! Returns an iterator to the end of the list of local indices of
        internal vertices of this partition. */
    const_iterator end() const { return internal_.end(); }

    /*! Returns the size of the internal vertex set in this partition. */
    verti internal_size() const { return (verti)internal_.size(); }

    /*! Returns the size of the external vertex set in this partition. */
    verti external_size() const { return total_size() - internal_size(); }

    /*! Returns the total number of vertices (internal and external) in this
        partition. */
    verti total_size() const { return global_.size(); }

    /*! Returns whether this is an empty partition (i.e. total_size() == 0) */
    bool empty() const { return global_.empty(); }

    /*! For debugging: returns the internal vertex set of the partition as a
        string, or a subset of according to `sel' if it is non-empty. */
    std::string debug_str(const std::vector<char> &sel = std::vector<char>())
        const;

private:
    ParityGame game_;               //! Local subgame
    std::vector<verti> internal_;   //! Local indices of internal vertex set
    std::vector<verti> global_;     //! Local to global vertex index map
    std::unordered_map<verti, verti> local_;  //! Global to local vertex index map
};

inline
void swap(GamePartition &a, GamePartition &b)
{
    a.swap(b);
}

#endif /* ndef GAME_PARTITION_H_INCLUDED */
