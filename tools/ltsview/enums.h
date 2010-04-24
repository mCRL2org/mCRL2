// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file enums.h
/// \brief Header file for Enumeration types

#ifndef ENUMS_H
#define ENUMS_H

enum MarkStyle
{
  NO_MARKS,
  MARK_DEADLOCKS,
  MARK_STATES,
  MARK_TRANSITIONS
};

enum MatchStyle
{
  MATCH_ANY,
  MATCH_ALL,
  MATCH_MULTI
};

enum PickState
{
  STATE,
  CLUSTER,
  SIMSTATE,
  PICKNONE
};

enum RankStyle
{
  ITERATIVE,
  CYCLIC
};

enum VisStyle
{
  CONES,
  TUBES
};

#endif
