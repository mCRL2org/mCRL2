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
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/parse.h"

using namespace mcrl2::core;
using namespace mcrl2::lts;
using namespace mcrl2::lts::detail;
using namespace mcrl2::log;


static void write_to_fsm(std::ostream& os, const lts_fsm_t& l)
{
  // determine number of state parameters
  size_t num_params;
  num_params=l.process_parameters().size();

  // print parameters with used values
  mCRL2log(verbose) << "writing parameter table..." << std::endl;
  for (size_t i=0; i<num_params; i++)
  {

    const std::vector < std::string > vals = l.state_element_values(i);
    os << l.process_parameter(i).first << "(" << vals.size() << ") " << l.process_parameter(i).second << " ";

    for (std::vector < std::string >::const_iterator j=vals.begin(); j!=vals.end(); ++j)
    {
      os << " \"" << *j << "\"";
    }
    os << std::endl;
  }

  // print states
  mCRL2log(verbose) << "writing states..." << std::endl;
  os << "---" << std::endl;
  for (size_t i=0; i<l.num_states(); i++)
  {
    size_t idx = i;
    // make sure that the initial state is first
    if (i == 0)
    {
      idx = l.initial_state();
    }
    else if (i == l.initial_state())
    {
      idx = 0;
    }

    if (l.has_state_info())
    {
      const state_label_fsm state_pars=l.state_label(idx);

      for (size_t j=0; j<state_pars.size() ; j++)
      {
        if (j > 0)
        {
          os << " ";
        }
        os << state_pars[j];

      }
      os << std::endl;
    }
  }

  // print transitions
  mCRL2log(verbose) << "writing transitions..." << std::endl;
  os << "---" << std::endl;
  const std::vector<transition> &trans=l.get_transitions();
  for (std::vector<transition>::const_iterator t=trans.begin(); t!=trans.end(); ++t)
  {
    transition::size_type from = t->from();
    // correct state numbering
    if (from == 0)
    {
      from = l.initial_state();
    }
    else if (from == l.initial_state())
    {
      from = 0;
    }
    transition::size_type to = t->to();
    if (to == 0)
    {
      to = l.initial_state();
    }
    else if (to == l.initial_state())
    {
      to = 0;
    }
    // correct state numbering
    os << from+1 << " " << to+1 << " \"";
    os << mcrl2::lts::detail::pp(l.action_label(t->label()));
    os << "\"" << std::endl;
  }
}

void mcrl2::lts::lts_fsm_t::load(const std::string& filename)
{
  if (filename.empty())
  {
    try
    {
      parse_fsm_specification_simple(std::cin, *this);
    }
    catch (mcrl2::runtime_error&)
    {
      throw mcrl2::runtime_error("Error parsing .fsm file from standard input.");
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
      parse_fsm_specification_simple(is, *this);
    }
    catch (mcrl2::runtime_error&)
    {
      throw mcrl2::runtime_error("Error parsing .fsm file");
    }
    is.close();
  }
//  if (num_states()==0)
//  {
//    add_state();
//  }
//  set_initial_state(0);
}

void mcrl2::lts::lts_fsm_t::loadnew(const std::string& filename)
{
  if (filename.empty())
  {
    parse_fsm_specification(std::cin, *this);
  }
  else
  {
    std::ifstream in(filename.c_str());
    if (!in)
    {
      throw mcrl2::runtime_error("Cannot open .fsm file " + filename + ".");
    }
    parse_fsm_specification(in, *this);
  }
  if (num_states() == 0)
  {
    add_state();
  }
  set_initial_state(0);
}

void mcrl2::lts::lts_fsm_t::save(const std::string& filename) const
{
  if (filename=="")
  {
    write_to_fsm(std::cout,*this);
  }
  else
  {
    std::ofstream os(filename.c_str());

    if (!os.is_open())
    {
      throw mcrl2::runtime_error("Cannot create .fsm file '" + filename + ".");
      return;
    }


    write_to_fsm(os,*this);
    os.close();
  }
}

