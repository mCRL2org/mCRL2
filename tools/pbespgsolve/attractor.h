// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ATTRACTOR_H_INCLUDED
#define ATTRACTOR_H_INCLUDED

#include "ParityGame.h"

/*! Helper function: returns whether all elements in range [begin:end) are
    elements of `set'. */
template<class ForwardIterator, class SetT>
bool is_subset_of(ForwardIterator begin, ForwardIterator end, const SetT &set);

/*! Computes the attractor set of the given vertex set for a specific player,
    and stores it in-place in `vertices'. If `strategy' is not NULL, it is
    updated for all vertices added that are controlled by `player'. */
template<class SetT, class StrategyT>
void make_attractor_set( const ParityGame &game, ParityGame::Player player,
                         SetT &vertices, StrategyT &strategy );

/* Variant that takes an explicit queue of initial vertices. This queue must
    be initialized to `vertices' or a subset thereof. */
template<class SetT, class DequeT, class StrategyT>
void make_attractor_set( const ParityGame &game, ParityGame::Player player,
    SetT &vertices, DequeT &todo, StrategyT &strategy );

#include "attractor_impl.h"

#endif /* ndef ATTRACTOR_H_INCLUDED */
