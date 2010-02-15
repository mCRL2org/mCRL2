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
#include "mcrl2/lts/lts.h"
#include "liblts_dotparser.h"

using namespace mcrl2::core;

#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisList(x) (ATgetType(x) == AT_LIST)

using namespace std;

namespace mcrl2
{
namespace lts
{

bool p_lts::read_from_dot(string const& filename)
{
  ifstream is(filename.c_str());

  if ( !is.is_open() )
  {
    gsVerboseMsg("cannot open DOT file '%s' for reading\n",filename.c_str());
    return false;
  }

  bool r = read_from_dot(is);

  is.close();

  return r;
}

bool p_lts::read_from_dot(istream &is)
{
  if ( parse_dot(is,*lts_object) )
  {
    std::vector< unsigned int > vec_of_possible_inits;

    AFun no_incoming_fun = ATmakeAFun("no_incoming",2,ATfalse);
    AFun value_fun = ATmakeAFun("Value",2,ATfalse);
    ATermAppl id = ATmakeAppl2(ATmakeAFun("Type",2,ATfalse),
                        (ATerm) ATmakeAppl0(ATmakeAFun("id",0,ATtrue)),
                        (ATerm) ATmakeAppl0(ATmakeAFun("String",0,ATtrue)));
    ATermAppl label = ATmakeAppl2(ATmakeAFun("Type",2,ATfalse),
                        (ATerm) ATmakeAppl0(ATmakeAFun("label",0,ATtrue)),
                        (ATerm) ATmakeAppl0(ATmakeAFun("String",0,ATtrue)));
    for (unsigned int i=0; i<nstates; i++)
    {
      if ( ATisEqualAFun(no_incoming_fun,ATgetAFun((ATermAppl) state_values[i])) )
      {
        vec_of_possible_inits.push_back( i );
      }

      ATermAppl name = ATAgetArgument((ATermAppl) state_values[i],0);
      ATermAppl val = ATAgetArgument((ATermAppl) state_values[i],1);
      state_values[i] = (ATerm) ATmakeList2((ATerm) ATmakeAppl2(value_fun,(ATerm) name,(ATerm) id),(ATerm) ATmakeAppl2(value_fun,(ATerm) val,(ATerm) label));
    }

    if ( vec_of_possible_inits.empty() )
    {
      init_state = 0;
      if ( nstates > 0 )
      {
        gsWarningMsg("could not find suitable initial state; taking first state (%s) as initial\n",ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument((ATermAppl)state_values[0],0),0))));
      }
    } else 
    {
      init_state = vec_of_possible_inits[0]; 
      if (vec_of_possible_inits.size() > 1)
      {
        gsWarningMsg("multiple suitable initial states; taking first suitable state (%s) as initial\n",ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument((ATermAppl)state_values[vec_of_possible_inits[0]],0),0))));
        if(gsVerbose)
        {
          gsVerboseMsg("set off initial states is:\n{");
          for(std::vector< unsigned int >::iterator i = vec_of_possible_inits.begin(); i != vec_of_possible_inits.end(); ++i ) 
          {
            if (i != --vec_of_possible_inits.end())
            {
              gsVerboseMsg("%s, ", ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument((ATermAppl)state_values[vec_of_possible_inits[*i]],0),0))) );
            } else {
              gsVerboseMsg("%s}\n" , ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument((ATermAppl)state_values[vec_of_possible_inits[*i]],0),0))) );
            }
          }
        }else{
          gsWarningMsg("use verbose to print all other initial states\n");
        }
      }
    }

    type = lts_dot;

    return true;
  } else {
    return false;
  }
}


bool p_lts::write_to_dot(string const& filename, lts_dot_options opts)
{
  ofstream os(filename.c_str());
  if ( !os.is_open() )
  {
    gsVerboseMsg("cannot open DOT file '%s' for writing\n",filename.c_str());
    return false;
  }

  bool r = write_to_dot(os,opts);

  os.close();

  return r;
}

bool p_lts::write_to_dot(ostream &os, lts_dot_options opts)
{
  os << "digraph \"" << *opts.name << "\" {" << endl; // Language definition seems to suggest that the name is optional, but tools seem to think otherwise
  // os << "size=\"7,10.5\";" << endl;
  os << "center = TRUE;" << endl;
  os << "mclimit = 10.0;" << endl;
  os << "nodesep = 0.05;" << endl;
  os << "node [ width=0.25, height=0.25, label=\"\" ];" << endl;
  if ( nstates > 0 )
  {
    if ( type == lts_dot )
    {
      os << ATwriteToString(ATgetArgument(ATAgetFirst((ATermList) state_values[init_state]),0));
    } else {
      os << init_state;
    }
    os << " [ peripheries=2 ];" << endl;
    if ( opts.print_states && state_info )
    {
      for (unsigned int i=0; i<nstates; i++)
      {
        if ( type == lts_dot )
        {
          os << ATwriteToString(ATgetArgument(ATAgetFirst((ATermList) state_values[i]),0));
          os << " [ label=\"" << ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(ATgetNext((ATermList) state_values[i])),0))) << "\" ];" << endl;
        } else {
          os << i << " [ label=\"" << p_state_value_str(i) << "\" ];" << endl;
        }
      }
    }
  }
  for (unsigned int i=0; i<ntransitions; i++)
  {
    if ( type == lts_dot )
    {
      os << ATwriteToString(ATgetArgument(ATAgetFirst((ATermList) state_values[transitions[i].from]),0));
      os << " -> " << ATwriteToString(ATgetArgument(ATAgetFirst((ATermList) state_values[transitions[i].to]),0));
      os << " [ label=\"" << p_label_value_str(transitions[i].label) << "\" ];" << endl;
    } else {
      os << transitions[i].from << "->" << transitions[i].to << "[label=\"" << p_label_value_str(transitions[i].label) << "\"];" << endl;
    }
  }

  os << "}" << endl;

  return true;
}

}
}
