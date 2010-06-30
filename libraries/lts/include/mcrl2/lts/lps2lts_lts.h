// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2lts_lts.h

#ifndef _LPS2LTS_LTS_H
#define _LPS2LTS_LTS_H

#include <memory>
#include "boost/cstdint.hpp"
#include "aterm2.h"
#include "mcrl2/lps/nextstate.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lts/lts.h"

struct lts_options 
{ mcrl2::lts::lts_type outformat;
  bool outinfo;
  NextState *nstate;
  std::auto_ptr< mcrl2::lps::specification > spec;
};

void reset();
void open_lts(const char *filename, lts_options &opts);
void save_initial_state(boost::uint64_t idx, ATerm state);
void save_transition(boost::uint64_t idx_from, ATerm from, ATermAppl action, boost::uint64_t idx_to, ATerm to);
void close_lts(boost::uint64_t num_states, boost::uint64_t num_trans);
void remove_lts();

#endif // _LPS2LTS_LTS_H
