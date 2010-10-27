// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_dot.cpp

// #include <sstream>
// #include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <vector>
#include "mcrl2/core/print.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/lts/lts_io.h"
#include "liblts_dotparser.h"
// #include "mcrl2/lts/state_label.h"
// #include "mcrl2/lts/action_label.h"


using namespace mcrl2::core;

#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisList(x) (ATgetType(x) == AT_LIST)

using namespace std;

namespace mcrl2
{
namespace lts
{
namespace detail
{

void lts_dot_t::load(const string & filename)
{
  if (filename=="")
  { 
    parse_dot(cin,*this);
  }
  else 
  {
    std::ifstream is(filename.c_str());

    if ( !is.is_open() )
    {
      throw mcrl2::runtime_error("cannot open DOT file '" + filename + "' for reading.");
    }
    parse_dot(is,*this);
    is.close();
  }
  set_initial_state(0);

  /* std::vector< unsigned int > vec_of_possible_inits;

  AFun no_incoming_fun = ATmakeAFun("no_incoming",2,ATfalse);
  AFun value_fun = ATmakeAFun("Value",2,ATfalse);
  ATermAppl id = ATmakeAppl2(ATmakeAFun("Type",2,ATfalse),
                      (ATerm) ATmakeAppl0(ATmakeAFun("id",0,ATtrue)),
                      (ATerm) ATmakeAppl0(ATmakeAFun("String",0,ATtrue)));
  ATermAppl label = ATmakeAppl2(ATmakeAFun("Type",2,ATfalse),
                      (ATerm) ATmakeAppl0(ATmakeAFun("label",0,ATtrue)),
                      (ATerm) ATmakeAppl0(ATmakeAFun("String",0,ATtrue)));
  for (unsigned int i=0; i<num_states(); i++)
  {
    if ( ATisEqualAFun(no_incoming_fun,ATgetAFun((ATermAppl) state_value(i).aterm())) )
    {
      vec_of_possible_inits.push_back( i );
    }

    ATermAppl name = ATAgetArgument((ATermAppl) state_value(i).aterm(),0);
    ATermAppl val = ATAgetArgument((ATermAppl) state_value(i).aterm(),1);
    set_state_value(i, mcrl2::lts::detail::state_label_dot(ATmakeList2((ATerm) ATmakeAppl2(value_fun,(ATerm) name,(ATerm) id),(ATerm) ATmakeAppl2(value_fun,(ATerm) val,(ATerm) label))));
  }

  if ( vec_of_possible_inits.empty() )
  {
    set_initial_state(0);
    if ( num_states() > 0 )
    {
      gsWarningMsg("could not find suitable initial state; taking first state (%s) as initial\n",ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument((ATermAppl)state_value(0).aterm(),0),0))));
    }
  } 
  else 
  {
    set_initial_state(vec_of_possible_inits[0]); 
    if (vec_of_possible_inits.size() > 1)
    {
      gsWarningMsg("multiple suitable initial states; taking first suitable state (%s) as initial\n",ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument((ATermAppl)state_value(vec_of_possible_inits[0]).aterm(),0),0))));
      if(gsVerbose)
      {
        gsVerboseMsg("set off initial states is:\n{");
        for(std::vector< unsigned int >::iterator i = vec_of_possible_inits.begin(); i != vec_of_possible_inits.end(); ++i ) 
        {
          if (i != --vec_of_possible_inits.end())
          {
            gsVerboseMsg("%s, ", ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument((ATermAppl)state_value(vec_of_possible_inits[*i]).aterm(),0),0))) );
          } else {
            gsVerboseMsg("%s}\n" , ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument((ATermAppl)state_value(vec_of_possible_inits[*i]).aterm(),0),0))) );
          }
        }
      }
      else
      {
        gsWarningMsg("use verbose to print all other initial states\n");
      }
    }
  } */
}

void lts_dot_t::save(const string &filename) const 
{
  ofstream os(filename.c_str());
  if ( !os.is_open() )
  {
    throw mcrl2::runtime_error("cannot open DOT file '" + filename + "' for writing.");
    return;
  }

  // Language definition seems to suggest that the name is optional, but tools seem to think otherwise
  os << "digraph \"" << filename << "\" {" << endl; 
  // os << "size=\"7,10.5\";" << endl;
  os << "center = TRUE;" << endl;
  os << "mclimit = 10.0;" << endl;
  os << "nodesep = 0.05;" << endl;
  os << "node [ width=0.25, height=0.25, label=\"\" ];" << endl;
  if ( num_states() > 0 )
  {
    os << state_value(initial_state()).name();

    os << " [ peripheries=2 ];" << endl;
    if ( has_state_info() )
    {
      for (unsigned int i=0; i<num_states(); i++)
      {
        os << state_value(i).name() << " [ label=\"" << state_value(i).label() << "\" ];" << endl;
      }
    }
  }
  for (transition_const_range t=get_transitions();  !t.empty(); t.advance_begin(1))
  {
    os << state_value(t.front().from()).name() << "->" << state_value(t.front().to()).name() << "[label=\"" << 
            mcrl2::lts::detail::pp(label_value(t.front().label())) << "\"];" << endl;
  }

  os << "}" << endl;
  os.close();
}

}
}
}
