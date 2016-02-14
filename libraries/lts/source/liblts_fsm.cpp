// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_fsm.cpp

#include <string>
#include <fstream>
#include "mcrl2/core/print.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/parse.h"
#include "mcrl2/lts/detail/liblts_swap_to_from_probabilistic_lts.h"

namespace mcrl2 {

namespace lts {

struct fsm_writer
{
  std::ostream& out;
  size_t number_of_initial_state;
  const probabilistic_lts_fsm_t& fsm;

  fsm_writer(std::ostream& out_, const probabilistic_lts_fsm_t& fsm_)
    : out(out_), number_of_initial_state(fsm_.initial_probabilistic_state().begin()->state()), fsm(fsm_)
  {
    assert(fsm_.initial_probabilistic_state().size()>0);
  }

  // This functions swaps 0 with the number number_of_initial_state of the initial state.
  size_t swap_initial_state(const size_t i)
  {
    if (i==number_of_initial_state)
    {
      return 0;
    }
    if (i==0)
    {
      return number_of_initial_state;
    }
    return i;
  }

  void write_parameters()
  {
    // print parameters with used values
    mCRL2log(log::verbose) << "writing parameter table..." << std::endl;
    for (std::size_t i = 0; i < fsm.process_parameters().size(); i++)
    {
      const std::vector<std::string>& values = fsm.state_element_values(i);
      out << fsm.process_parameter(i).first << "(" << values.size() << ") " << fsm.process_parameter(i).second << " ";
      for (const std::string& s: values)
      {
        out << " \"" << s << "\"";
      }
      out << std::endl;
    }
  }

  void write_states()
  {
    mCRL2log(log::verbose) << "writing states..." << std::endl;
    assert(fsm.initial_probabilistic_state().size()>0);
    for (std::size_t i = 0; i < fsm.num_states(); i++)
    {
      if (fsm.has_state_info())
      {
        const state_label_fsm& state_parameters = fsm.state_label(swap_initial_state(i));
        for (std::size_t j = 0; j < state_parameters.size(); j++)
        {
          if (j > 0)
          {
            out << " ";
          }
          out << state_parameters[j];
        }
        out << std::endl;
      }
    }
  }

  // If there is only a single state with probility 1, write a single state, otherwise 
  // write "[state1 probability1, state2 probability2, ..., state_n probability_n]".
  void write_probabilistic_state(const detail::lts_fsm_base::probabilistic_state& probabilistic_state)
  {
    if (probabilistic_state.size()==1)
    {
      out << swap_initial_state(probabilistic_state.begin()->state())+1;
    }
    else
    {
      out << "[";
      bool first=true;
      for(const lps::state_probability_pair< size_t, probabilistic_arbitrary_precision_fraction>& p: probabilistic_state)
      {
        if (first)
        {
          first=false;
        }
        else
        {
          out << ' ';
        }
        out << swap_initial_state(p.state()) + 1 << " " << p.probability();
      }
      out << "]";
    }
  }

  void write_transitions()
  {
    mCRL2log(log::verbose) << "writing transitions..." << std::endl;
    for (const transition& t: fsm.get_transitions())
    {
      // correct state numbering, by adding 1.
      out << swap_initial_state(t.from()) + 1 << " ";
      write_probabilistic_state(fsm.probabilistic_state(t.to())); 
      out << " \"" << mcrl2::lts::pp(fsm.action_label(t.label())) << "\"" << std::endl;
    }
  }

  void write()
  {
    write_parameters();
    out << "---" << std::endl;
    write_states();
    out << "---" << std::endl;
    write_transitions();
    // If there is a initial distribution with more than one state, write the initial distribution.
    if (fsm.initial_probabilistic_state().size()>1)
    {
      out << "---" << std::endl;
      write_probabilistic_state(fsm.initial_probabilistic_state()); 
      out << "\n" << std::endl;
    }
  }
};

void probabilistic_lts_fsm_t::load(const std::string& filename)
{
  if (filename.empty())
  {
    try
    {
      parse_fsm_specification(std::cin, *this);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string("Error parsing .fsm file from standard input.\n") + e.what());
    }
  }
  else
  {
    std::ifstream is(filename.c_str());

    if (!is.is_open())
    {
      throw mcrl2::runtime_error("Cannot open .fsm file " + filename + ".");
    }
    try
    {
      parse_fsm_specification(is, *this);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string("Error parsing .fsm file.\n") + e.what());
    }
    is.close();
  }
}

void probabilistic_lts_fsm_t::save(const std::string& filename) const
{
  if (filename=="")
  {
    fsm_writer(std::cout, *this).write();
  }
  else
  {
    std::ofstream os(filename.c_str());

    if (!os.is_open())
    {
      throw mcrl2::runtime_error("Cannot create .fsm file '" + filename + ".");
      return;
    }

    fsm_writer(os, *this).write();
    os.close();
  }
}

void lts_fsm_t::load(const std::string& filename)
{
  probabilistic_lts_fsm_t l;
  l.load(filename);
  detail::swap_to_non_probabilistic_lts
             <state_label_fsm,
              action_label_string,
              detail::lts_fsm_base::probabilistic_state,
              detail::lts_fsm_base>(l,*this);
}

void lts_fsm_t::save(const std::string& filename) const
{
  probabilistic_lts_fsm_t l;
  detail::translate_to_probabilistic_lts
            <state_label_fsm,
             action_label_string,
             detail::lts_fsm_base::probabilistic_state,
             detail::lts_fsm_base>(*this,l);
  l.save(filename);
}


} // namespace lts

} // namespace mcrl2
