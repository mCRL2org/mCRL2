// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "GamePartition.h"
#include <algorithm>
#include <sstream>  // for debug_str()
#include <cassert>

GamePartition::GamePartition( const ParityGame &old_game,
                              const VertexPartition &vpart, int proc )
{
    // Select vertices in the partition of the game graph:
    const verti V = old_game.graph().V();
    std::vector<verti> intern;
    intern.reserve(vpart.num_assigned(V, proc));
    for (verti v = vpart.first(proc); v < V; v = vpart.next(proc, v))
    {
        assert(vpart(v) == proc);
        intern.push_back(v);
    }
    assert(intern.size() == intern.capacity());

    // We assume `intern' is sorted and therefore `internal' will
    // be sorted too. This makes it easier to create subpartitions later.
    assert(::is_sorted(intern.begin(), intern.end(), std::less<verti>()));

    std::vector<verti> verts(intern.begin(), intern.end());

    // Find vertices incident to internal vertices
    for (std::vector<verti>::const_iterator it = intern.begin();
            it != intern.end(); ++it)
    {
        // Add predecessors of internal vertices
        for (StaticGraph::const_iterator jt = old_game.graph().pred_begin(*it);
                jt != old_game.graph().pred_end(*it); ++jt) verts.push_back(*jt);

        // Add successors of internal vertices
        for (StaticGraph::const_iterator jt = old_game.graph().succ_begin(*it);
                jt != old_game.graph().succ_end(*it); ++jt) verts.push_back(*jt);
    }

    // Make vertex set unique
    std::sort(verts.begin(), verts.end());
    verts.erase(std::unique(verts.begin(), verts.end()), verts.end());

    // Create game
    game_.make_subgame(old_game, verts.begin(), verts.end());

    // Create vertex index maps
    global_ = verts;
    for (verti v = 0; v < (verti)global_.size(); ++v)
    {
        local_[global_[v]] = v;
    }
    internal_ = intern;
    for ( std::vector<verti>::iterator it = internal_.begin();
            it != internal_.end(); ++it )
    {
        *it = local_[*it];
    }

    // FIXME?  We are currently storing successor/predecessor edges for
    //         external vertices too, but these are never used!
}

GamePartition::GamePartition( const GamePartition &part,
                              const std::vector<verti> &verts )
{
    std::set_intersection( verts.begin(), verts.end(),
                           part.internal_.begin(), part.internal_.end(),
                           std::back_inserter(internal_) );

    std::vector<verti> new_verts;
    // FIXME: we need to remove vertices that do not have any edges incident
    //        to the internal vertex set, but this is a bit ugly:
    {
        const StaticGraph &g = part.game_.graph();
        std::unordered_set<verti> used(internal_.begin(), internal_.end());
        for (std::vector<verti>::const_iterator it = verts.begin();
                it != verts.end(); ++it)
        {
            verti v = *it;
            assert(v < g.V());
            bool found = used.find(v) != used.end();
            for (StaticGraph::const_iterator it = g.succ_begin(v);
                    !found && it != g.succ_end(v); ++it)
            {
                if (used.find(*it) != used.end()) found = true;
            }
            for (StaticGraph::const_iterator it = g.pred_begin(v);
                    !found && it != g.pred_end(v); ++it)
            {
                if (used.find(*it) != used.end()) found = true;
            }
            if (found) new_verts.push_back(v);
        }
    }

    game_.make_subgame(part.game_, new_verts.begin(), new_verts.end());
    global_.resize(new_verts.size());
    for (verti i = 0; i < (verti)global_.size(); ++i)
    {
        global_[i] = part.global_[new_verts[i]];
        local_[global_[i]] = i;
    }

    // Map internal vertices to new local indices.
    // FIXME: is there a more efficient way to do this?
    for ( std::vector<verti>::iterator it = internal_.begin();
            it != internal_.end(); ++it )
    {
        *it = local_[part.global_[*it]];
    }

    // DEBUG: check consistency of vertex index mapping
    /*
    for (verti i = 0; i < (verti)global_.size(); ++i)
    {
        assert(local_[global_[i]] == i);
    }
    for (std::unordered_map<verti, verti>::const_iterator it = local_.begin();
            it != local_.end(); ++it)
    {
        assert(global_[it->second] == it->first);
    }
    */
}

void GamePartition::swap(GamePartition &gp)
{
    std::swap(game_, gp.game_);
    std::swap(internal_, gp.internal_);
    std::swap(global_, gp.global_);
    // N.B. std::swap() is not yet overloaded for hash maps in C++ TR1:
    local_.swap(gp.local_);
}

std::string GamePartition::debug_str(const std::vector<char> &sel) const
{
    std::ostringstream os;
    bool first = true;
    os << "{ ";
    for (GamePartition::const_iterator it = begin(); it != end(); ++it)
    {
        if (sel.empty() || sel[*it])
        {
            if (first) first = false; else os << ", ";
            os << global(*it);
        }
    }
    os << " }";
    return os.str();
}

/* FIXME: this could be implemented more efficiently (e.g. with a precomputed
          vector<bool> bitarray) if this is used intensively. */
bool GamePartition::is_internal(verti v) const
{
    return binary_search(internal_.begin(), internal_.end(), v);
}
