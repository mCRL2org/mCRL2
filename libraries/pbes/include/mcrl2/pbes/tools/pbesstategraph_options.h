// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbesstategraph_options.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TOOLS_PBESSTATEGRAPH_OPTIONS_H
#define MCRL2_PBES_TOOLS_PBESSTATEGRAPH_OPTIONS_H

#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/utilities/execution_timer.h"

namespace mcrl2 {

namespace pbes_system {

struct pbesstategraph_options
{
  data::rewrite_strategy rewrite_strategy;
  bool simplify;
  bool apply_to_original;
  bool use_local_variant;
  bool print_influence_graph;
  bool cache_marking_updates;
  int marking_algorithm;
  bool use_alternative_lcfp_criterion;
  bool use_alternative_gcfp_relation;
  bool use_alternative_gcfp_consistency;
  utilities::execution_timer* timer;     // if it is non-zero, it will be used to display timing information

  pbesstategraph_options()
  : rewrite_strategy(data::jitty),
    simplify(true),
    apply_to_original(false),
    use_local_variant(false),
    print_influence_graph(false),
    cache_marking_updates(false),
    marking_algorithm(0),
    use_alternative_lcfp_criterion(false),
    use_alternative_gcfp_relation(false),
    use_alternative_gcfp_consistency(false),
    timer(0)
  {}

  bool timing_enabled() const
  {
    return timer != 0;
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_PBESSTATEGRAPH_OPTIONS_H
