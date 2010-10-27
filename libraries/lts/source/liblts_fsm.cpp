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
// #include <sstream>
// #include <iostream>
#include <fstream>
#include "boost/scoped_array.hpp"
// #include "aterm2.h"
// #include "mcrl2/core/messaging.h"
// #include "mcrl2/core/aterm_ext.h"
// #include "mcrl2/core/detail/struct_core.h"
// #include "mcrl2/core/parse.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/core/print.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lps/specification.h"
#include "liblts_fsmparser.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data::detail;
using namespace mcrl2;

#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisList(x) (ATgetType(x) == AT_LIST)

namespace mcrl2
{
namespace lts
{
namespace detail
{

/* static ATerm parse_mcrl2_action(
                   ATerm label, 
                   const data::data_specification &data_spec,
                   const lps::action_list &act_decls)
{
  std::stringstream ss(ATgetName(ATgetAFun((ATermAppl) label)));

  ATermAppl t = parse_mult_act(ss);
  if ( t == NULL )
  {
    throw mcrl2::runtime_error("cannot parse action as mCRL2.");
  } 
  
  lps::multi_action ma=lps::action_list(ATLgetArgument(t,0));
  lps::type_check(ma,data_spec,act_decls);

  return (ATerm) gsMakeMultAct(ma.actions());
} */

/* static ATerm parse_mcrl2_state(ATerm state, const data::data_specification &data_spec)
{
  unsigned int len = ATgetLength((ATermList) state);
  boost::scoped_array< ATerm > state_args(new ATerm[len]);

  for (unsigned int i=0; !ATisEmpty((ATermList) state); state=(ATerm) ATgetNext((ATermList) state),++i)
  {
    ATermAppl val = ATAgetFirst((ATermList) state);
    ATermAppl expr = ATAgetArgument(val,0);
    ATermAppl sort = ATAgetArgument(ATAgetArgument(val,1),1);

    // typechecking and data implementation of terms needs an lps
    // before data implementation
    // ATermAppl data_spec_for_type_check(specification_to_aterm(spec));

    std::stringstream sort_ss(ATgetName(ATgetAFun(sort)));
    sort = parse_sort_expr(sort_ss);
    if ( sort == NULL )
    {
      throw mcrl2::runtime_error("error parsing state argument sort\n");
    }
    // sort = type_check_sort_expr(sort,spec_for_type_check);
    data::sort_expression local_sort(sort);
    data::type_check(local_sort,data_spec);

    std::stringstream expr_ss(ATgetName(ATgetAFun(expr)));
    expr = parse_data_expr(expr_ss);
    if ( expr == NULL )
    {
      throw mcrl2::runtime_error("error parsing state argument\n");
    }
    data::data_expression local_expr(expr);
    data::type_check(local_expr,data_spec); // local_sort,spec_for_type_check);

    state_args[i] = (ATerm)(ATermAppl)local_expr;
  }

  ATerm r = (ATerm) ATmakeApplArray(ATmakeAFun("STATE",len,ATfalse),state_args.get());
  return r;
} */

// void read_from_fsm(lts_extra &l, std::istream &is, lts_type type, lps::specification const& spec)
void read_from_fsm(
               lts_fsm_t &l,
               std::istream& is)
{
  if (!parse_fsm(is,l))
  { throw mcrl2::runtime_error("error parsing .fsm file");
  }

  /* CODE BELOW MUST BE MOVED TO THE PART WHERE EXPLICIT TRANSLATIONS BETWEEN LTS's are made.
  if (l.has_data() && l.has_action_labels())
  {
    for (unsigned int i=0; i<l.num_labels(); i++)
    {
      l.set_label_value(i,action_label(parse_mcrl2_action(l.label_value(i).label(),l.data(),l.action_labels())));
    }
  }
  if (l.has_process_parameters() && l.num_states()>0 && 
             l.state_value(0).size() != l.process_parameters().size())
  {
    throw mcrl2::runtime_error("The length of the process parameter list does not coincide with the number of parameters in a state.");  
  } 
  
  if (l.has_data())
  {
    for (unsigned int i=0; i<l.num_states(); i++)
    {
      l.set_state_value(i,state_label((ATermAppl)parse_mcrl2_state((ATerm)l.state_value(i).aterm(),l.data())));
    }
  } */
}


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
    
    const state_label_fsm state_pars=l.state_value(idx);

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
    os << mcrl2::lts::detail::pp(l.label_value(t.front().label()));
    os << "\"" << std::endl;
  }
}

} // namespace detail

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

void lts_fsm_t::save(const std::string &filename) const
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


} // namespace lts
} // namespace mcrl2
