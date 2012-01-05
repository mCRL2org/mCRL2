// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts.cpp

#include <cstring>
#include "mcrl2/lps/specification.h"
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/core/print.h"
#include "mcrl2/lts/detail/lps2lts_lts.h"
#include "mcrl2/lts/detail/lts_convert.h"
#include "mcrl2/lts/lts_io.h"


using namespace std;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::lts;
using namespace mcrl2::lts::detail;
using namespace mcrl2::log;

namespace mcrl2
{
namespace lts
{
#define NAME "lps2lts"

void lps2lts_lts::open_lts(const char* filename, lps2lts_lts_options& opts)
{
  lts_filename = std::string(filename);

  lts_opts = opts;
  switch (lts_opts.outformat)
  {
    case lts_none:
      mCRL2log(verbose) << "not saving state space." << std::endl;
      break;
    case lts_aut:
      mCRL2log(verbose) << "writing state space in AUT format to '" << filename << "'." << std::endl;
      /* lts_opts.outinfo = false; */
      aut.open(filename);
      if (!aut.is_open())
      {
        mCRL2log(error) << "cannot open '" << filename << "' for writing" << std::endl;
        exit(1);
      }
      break;
    default:
      mCRL2log(verbose) << "writing state space in " << mcrl2::lts::detail::string_for_type(lts_opts.outformat)
                        << " format to '" << filename << "'." << std::endl;
      generic_lts.set_data(lts_opts.spec->data());
      generic_lts.set_process_parameters(lts_opts.spec->process().process_parameters());
      generic_lts.set_action_labels(lts_opts.spec->action_labels());
      aterm2state = ATindexedSetCreate(10000,50);
      aterm2label = ATindexedSetCreate(100,50);
      break;
  }
}

void lps2lts_lts::save_initial_state(size_t idx, ATerm state)
{
  initial_state = idx;
  switch (lts_opts.outformat)
  {
    case lts_none:
      break;
    case lts_aut:
      aut << "des (0,0,0)                                      " << endl;
      break;
    default:
    {
      bool is_new;
      const size_t t = ATindexedSetPut(aterm2state,state,&is_new);
      if (is_new /* && lts_opts.outinfo */)
      {
        const size_t u = generic_lts.add_state(state_label_lts(lts_opts.nstate->makeStateVector(state)));
        assert(u==t);
        static_cast <void>(u); // Avoid a warning when compiling in non debug mode.
      }
      generic_lts.set_initial_state(t);
    }
    break;
  }
}

void lps2lts_lts::save_transition(size_t idx_from, ATerm from, const mcrl2::lps::multi_action action, size_t idx_to, ATerm to)
{
  switch (lts_opts.outformat)
  {
    case lts_none:
      break;
    case lts_aut:
      if (idx_from == initial_state)
      {
        idx_from = 0;
      }
      if (idx_to == initial_state)
      {
        idx_to = 0;
      }
      aut << "(" << idx_from << ",\"";
      aut << pp(action);
      aut << "\"," << idx_to << ")\n";
      break;
    default:
    {
      bool is_new;
      const size_t from_state = ATindexedSetPut(aterm2state,from,&is_new);
      if (is_new /* && lts_opts.outinfo */)
      {
        const size_t t = generic_lts.add_state(state_label_lts(lts_opts.nstate->makeStateVector(from)));
        assert(t==from_state);
        static_cast <void>(t); // Avoid a warning when compiling in non debug mode.
      }
      const size_t to_state = ATindexedSetPut(aterm2state,to,&is_new);
      if (is_new /* && lts_opts.outinfo */)
      {
        const size_t t = generic_lts.add_state(state_label_lts(lts_opts.nstate->makeStateVector(to)));
        assert(t==to_state);
        static_cast <void>(t); // Avoid a warning when compiling in non debug mode.
      }
      const size_t label = ATindexedSetPut(aterm2label,(ATerm)(ATermAppl)mcrl2::lps::detail::multi_action_to_aterm(action),&is_new);
      if (is_new)
      {
        const size_t t = generic_lts.add_action((ATerm)(ATermAppl)mcrl2::lps::detail::multi_action_to_aterm(action), action.actions().empty());
        assert(t==label);
        static_cast <void>(t); // Avoid a warning when compiling in non debug mode.
      }
      generic_lts.add_transition(transition(from_state,label,to_state));
    }
    break;
  }
}

void lps2lts_lts::close_lts(size_t num_states, size_t num_trans)
{
  switch (lts_opts.outformat)
  {
    case lts_none:
      break;
    case lts_aut:
      aut.flush();
      aut.seekp(0);
      aut << "des (0," << num_trans << "," << num_states << ")";
      aut.close();
      break;
    default:
    {
      generic_lts.set_data(lts_opts.spec->data());
      generic_lts.set_process_parameters(lts_opts.spec->process().process_parameters());
      generic_lts.set_action_labels(lts_opts.spec->action_labels());
      if (!lts_opts.outinfo)
      {
        /* State labels should not be stored, so remove them */
        generic_lts.clear_state_labels();
      }
      switch (lts_opts.outformat)
      {
        case lts_none:
        {
          assert(0);
          break;
        }
        case lts_lts:
        {
          generic_lts.save(lts_filename);
          break;
        }
        case lts_aut:
        {
          assert(0);
          break;
        }
        case lts_fsm:
        {
          lts_fsm_t l;
          detail::lts_convert(generic_lts,l);
          l.save(lts_filename);
          break;
        }
#ifdef USE_BCG
        case lts_bcg:
        {
          lts_bcg_t l;
          detail::lts_convert(generic_lts,l);
          l.save(lts_filename);
          break;
        }
#endif
        case lts_dot:
        {
          lts_dot_t l;
          detail::lts_convert(generic_lts,l);
          l.save(lts_filename);
          break;
        }
        case lts_svc:
        {
          lts_svc_t l;
          detail::lts_convert(generic_lts,l);
          l.save(lts_filename);
          break;
        }
        default:
          assert(0); // lts_aut and lts_none cannot occur.
      }

      ATindexedSetDestroy(aterm2label);
      ATindexedSetDestroy(aterm2state);
      break;
    }
  }

  // Avoid static initialisation/descruction fiasco
  lts_opts.spec.reset();
}

void lps2lts_lts::remove_lts()
{
  switch (lts_opts.outformat)
  {
    case lts_aut:
      aut.close();
      break;
    case lts_none:
      break;
    default:
      ATindexedSetDestroy(aterm2label);
      ATindexedSetDestroy(aterm2state);
      break;
  }
  remove(lts_filename.c_str());
}

}
}


