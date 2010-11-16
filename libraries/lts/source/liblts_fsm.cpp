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
#include "mcrl2/lts/lts_io.h"
#include "liblts_fsmparser.h"

using namespace mcrl2::core;
using namespace mcrl2::lts;
using namespace mcrl2::lts::detail;


static void write_to_fsm(std::ostream &os, const lts_fsm_t &l)
{
  // determine number of state parameters
  unsigned int num_params;
  num_params=l.process_parameters().size();

  // print parameters with used values
  gsVerboseMsg("writing parameter table...\n");
  for(unsigned int i=0; i<num_params; i++)
  {

    const std::vector < std::string > vals = l.state_element_values(i); 
    os << l.process_parameter(i).first << "(" << vals.size() << ") " << l.process_parameter(i).second << " ";

    for(std::vector < std::string >::const_iterator j=vals.begin(); j!=vals.end(); ++j)
    {
      os << " \"" << *j << "\"";
    }
    os << std::endl;
  }

  // print states
  gsVerboseMsg("writing states...\n");
  os << "---" << std::endl;
  for(unsigned int i=0; i<l.num_states(); i++)
  {
    unsigned int idx = i;
    // make sure that the initial state is first
    if (i == 0)
    {
      idx = l.initial_state();
    } 
    else if (i == l.initial_state())
    {
      idx = 0;
    }
    
    const state_label_fsm state_pars=l.state_label(idx);

    for(unsigned int j=0; j<state_pars.size() ; j++)
    {
      if (j > 0) 
      {
        os << " ";
      }
      os << state_pars[j];

    }
    os << std::endl;
  }

  // print transitions
  gsVerboseMsg("writing transitions...\n");
  os << "---" << std::endl;
  for (transition_const_range t=l.get_transitions();  !t.empty(); t.advance_begin(1))
  {
    transition::size_type from = t.front().from();
    // correct state numbering
    if ( from == 0 )
    {
      from = l.initial_state();
    } else if ( from == l.initial_state() )
    {
      from = 0;
    }
    transition::size_type to = t.front().to();
    if ( to == 0 )
    {
      to = l.initial_state();
    } else if ( to == l.initial_state() )
    {
      to = 0;
    }
    // correct state numbering
    os << from+1 << " " << to+1 << " \"";
    os << mcrl2::lts::detail::pp(l.action_label(t.front().label()));
    os << "\"" << std::endl;
  }
}

void mcrl2::lts::lts_fsm_t::load(const std::string &filename)
{
  if (filename.empty())
  { 
    if (!parse_fsm(std::cin,*this))
    { throw mcrl2::runtime_error("error parsing .fsm file");
    }
  }
  else 
  { 
    std::ifstream is(filename.c_str());
    if (!parse_fsm(is,*this))
    { throw mcrl2::runtime_error("error parsing .fsm file");
    }
  }
}

void mcrl2::lts::lts_fsm_t::save(const std::string &filename) const
{
  if (filename=="")
  {
    write_to_fsm(std::cout,*this);
  }
  else
  {
    std::ofstream os(filename.c_str());
    write_to_fsm(os,*this);
    os.close();
  }
}

