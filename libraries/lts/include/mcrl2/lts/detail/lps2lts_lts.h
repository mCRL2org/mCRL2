// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef MCRL2_LTS_DETAIL_LPS2LTS_LTS_H
#define MCRL2_LTS_DETAIL_LPS2LTS_LTS_H

#include <memory>
#include <fstream>
#include "svc/svc.h"
#include "mcrl2/lps/nextstate/standard.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lts/lts_lts.h"

namespace mcrl2
{
namespace lts
{
struct lps2lts_lts_options
{
  mcrl2::lts::lts_type outformat;
  bool outinfo;
  lps::NextState* nstate;
  std::auto_ptr< mcrl2::lps::specification > spec;
};

class lps2lts_lts
{
    lps2lts_lts_options lts_opts;
    size_t initial_state;
    std::ofstream aut;
    std::string lts_filename;
    lts_lts_t generic_lts;
    atermpp::indexed_set aterm2state, aterm2label;

  public:
    lps2lts_lts():
      initial_state(0)
    {
    }

    ~lps2lts_lts()
    {
    }

    void reset();
    void open_lts(const char* filename, lps2lts_lts_options& opts);
    void save_initial_state(size_t idx, atermpp::aterm state);
    void save_transition(size_t idx_from, atermpp::aterm from, const mcrl2::lps::multi_action action, size_t idx_to, atermpp::aterm to);
    void close_lts(size_t num_states, size_t num_trans);
    void remove_lts();

};
}
}

#endif // MCRL2_LTS_DETAIL_LPS2LTS_LTS_H
