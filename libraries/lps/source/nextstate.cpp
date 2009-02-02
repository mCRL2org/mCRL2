// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file libnextstate.cpp

#include <aterm2.h>
#include <mcrl2/lps/nextstate.h>
#include <mcrl2/lps/nextstate/standard.h>

NextStateGenerator::~NextStateGenerator()
{
}

NextState::~NextState()
{
}

NextState *createNextState(
  ATermAppl spec,
  bool allow_free_vars,
  int state_format,
  Enumerator *e,
  bool clean_up_enumerator,
  NextStateStrategy strategy
)
{
  switch ( strategy )
  {
    case nsStandard:
      return new NextStateStandard(spec,allow_free_vars, state_format,e,clean_up_enumerator);
    default:
      if ( clean_up_enumerator )
      {
        delete e;
      }
    return NULL;
  }
}

NextState *createNextState(
  ATermAppl spec,
  bool allow_free_vars,
  int state_format,
  RewriteStrategy rewrite_strategy,
  EnumerateStrategy enumerator_strategy,
  NextStateStrategy strategy
)
{
  return
    createNextState(
      spec,
      allow_free_vars,
      state_format,
      createEnumerator(
        mcrl2::data::data_specification((ATermAppl) ATgetArgument(spec, 0)),
        createRewriter(
          mcrl2::data::data_specification((ATermAppl) ATgetArgument(spec, 0)),
          rewrite_strategy
        ),
        true,
        enumerator_strategy),
      true,
      strategy
    );
}
