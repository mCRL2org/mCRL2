// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_dot.cpp

#include <ostream>
#include <fstream>
#include <string>
#include <vector>
#include "mcrl2/core/print.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/lts/lts_io.h"
#include "liblts_dotparser.h"

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

void read_from_dot(lts &l,string const& filename)
{
  ifstream is(filename.c_str());

  if ( !is.is_open() )
  {
    throw mcrl2::runtime_error("cannot open DOT file '" + filename + "' for reading.");
  }

  read_from_dot(l,is);
  is.close();
}

void read_from_dot(lts &l, istream &is)
{
  parse_dot(is,l);
  std::vector< unsigned int > vec_of_possible_inits;

  AFun no_incoming_fun = ATmakeAFun("no_incoming",2,ATfalse);
  AFun value_fun = ATmakeAFun("Value",2,ATfalse);
  ATermAppl id = ATmakeAppl2(ATmakeAFun("Type",2,ATfalse),
                      (ATerm) ATmakeAppl0(ATmakeAFun("id",0,ATtrue)),
                      (ATerm) ATmakeAppl0(ATmakeAFun("String",0,ATtrue)));
  ATermAppl label = ATmakeAppl2(ATmakeAFun("Type",2,ATfalse),
                      (ATerm) ATmakeAppl0(ATmakeAFun("label",0,ATtrue)),
                      (ATerm) ATmakeAppl0(ATmakeAFun("String",0,ATtrue)));
  for (unsigned int i=0; i<l.num_states(); i++)
  {
    if ( ATisEqualAFun(no_incoming_fun,ATgetAFun((ATermAppl) l.state_value(i))) )
    {
      vec_of_possible_inits.push_back( i );
    }

    ATermAppl name = ATAgetArgument((ATermAppl) l.state_value(i),0);
    ATermAppl val = ATAgetArgument((ATermAppl) l.state_value(i),1);
    l.set_state_value(i, (ATerm) ATmakeList2((ATerm) ATmakeAppl2(value_fun,(ATerm) name,(ATerm) id),(ATerm) ATmakeAppl2(value_fun,(ATerm) val,(ATerm) label)));
  }

  if ( vec_of_possible_inits.empty() )
  {
    l.set_initial_state(0);
    if ( l.num_states() > 0 )
    {
      gsWarningMsg("could not find suitable initial state; taking first state (%s) as initial\n",ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument((ATermAppl)l.state_value(0),0),0))));
    }
  } else 
  {
    l.set_initial_state(vec_of_possible_inits[0]); 
    if (vec_of_possible_inits.size() > 1)
    {
      gsWarningMsg("multiple suitable initial states; taking first suitable state (%s) as initial\n",ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument((ATermAppl)l.state_value(vec_of_possible_inits[0]),0),0))));
      if(gsVerbose)
      {
        gsVerboseMsg("set off initial states is:\n{");
        for(std::vector< unsigned int >::iterator i = vec_of_possible_inits.begin(); i != vec_of_possible_inits.end(); ++i ) 
        {
          if (i != --vec_of_possible_inits.end())
          {
            gsVerboseMsg("%s, ", ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument((ATermAppl)l.state_value(vec_of_possible_inits[*i]),0),0))) );
          } else {
            gsVerboseMsg("%s}\n" , ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument((ATermAppl)l.state_value(vec_of_possible_inits[*i]),0),0))) );
          }
        }
      }else{
        gsWarningMsg("use verbose to print all other initial states\n");
      }
    }
  }

  l.set_type(lts_dot);
}


void write_to_dot(const lts &l, string const& filename, lts_dot_options opts)
{
  ofstream os(filename.c_str());
  if ( !os.is_open() )
  {
    throw mcrl2::runtime_error("cannot open DOT file '" + filename + "' for writing.");
    return;
  }

  write_to_dot(l,os,opts);
  os.close();
}

void write_to_dot(const lts &l, ostream &os, lts_dot_options opts)
{
  os << "digraph \"" << *opts.name << "\" {" << endl; // Language definition seems to suggest that the name is optional, but tools seem to think otherwise
  // os << "size=\"7,10.5\";" << endl;
  os << "center = TRUE;" << endl;
  os << "mclimit = 10.0;" << endl;
  os << "nodesep = 0.05;" << endl;
  os << "node [ width=0.25, height=0.25, label=\"\" ];" << endl;
  if ( l.num_states() > 0 )
  {
    if ( l.get_type() == lts_dot )
    {
      os << ATwriteToString(ATgetArgument(ATAgetFirst((ATermList) l.state_value(l.initial_state())),0));
    } else {
      os << l.initial_state();
    }
    os << " [ peripheries=2 ];" << endl;
    if ( opts.print_states && l.has_state_info() )
    {
      for (unsigned int i=0; i<l.num_states(); i++)
      {
        if ( l.get_type() == lts_dot )
        {
          os << ATwriteToString(ATgetArgument(ATAgetFirst((ATermList) l.state_value(i)),0));
          os << " [ label=\"" << ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(ATgetNext((ATermList) l.state_value(i))),0))) << "\" ];" << endl;
        } else {
          os << i << " [ label=\"" << l.state_value_str(i) << "\" ];" << endl;
        }
      }
    }
  }
  for (transition_const_range t=l.get_transitions();  !t.empty(); t.advance_begin(1))
  // for (unsigned int i=0; i<ntransitions; i++)
  {
    if ( l.get_type() == lts_dot )
    {
      os << ATwriteToString(ATgetArgument(ATAgetFirst((ATermList) l.state_value(t.front().from())),0));
      os << " -> " << ATwriteToString(ATgetArgument(ATAgetFirst((ATermList) l.state_value(t.front().to())),0));
      os << " [ label=\"" << l.label_value_str(t.front().label()) << "\" ];" << endl;
    } else {
      os << t.front().from() << "->" << t.front().to() << "[label=\"" << l.label_value_str(t.front().label()) << "\"];" << endl;
    }
  }

  os << "}" << endl;
}

}
}
}
