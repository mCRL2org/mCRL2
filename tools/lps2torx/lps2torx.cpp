// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2torx.cpp

#include "boost.hpp" // precompiled headers

#define NAME "lps2torx"
#define AUTHOR "Muck van Weerdenburg"

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <climits>
#include <cstring>
#include <cassert>
#include <iostream>
#include <sstream>
#include <aterm2.h>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/detail/aterm_io.h"
#include "mcrl2/core/print.h"
#include "mcrl2/lps/nextstate.h"
#include "mcrl2/data/enum.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/lps/data_elimination.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"
#include "mcrl2/utilities/command_line_rewriting.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace std;

#define is_tau(x) ATisEmpty((ATermList) ATgetArgument(x,0))
          
void print_torx_action(ostream &os, ATermAppl mact)
{
  if ( is_tau(mact) )
  {
    os << "tau";
  } else {
    ATermAppl act = (ATermAppl) ATgetFirst((ATermList) ATgetArgument(mact,0));
    PrintPart_CXX(cout,ATgetArgument(act,0), ppDefault);
    ATermList dl = (ATermList) ATgetArgument(act,1);
    for (; !ATisEmpty(dl); dl=ATgetNext(dl))
    {
      cout << "!";
      PrintPart_CXX(cout,ATgetFirst(dl), ppDefault);
    }
  }
}
  
typedef struct {
  int action;
  int state;
} index_pair;

class torx_data
{
  private:
    ATermIndexedSet stateactions;
    ATermTable state_indices;
    AFun fun_trip;
    unsigned int num_indices;

    ATerm triple(ATerm one, ATerm two, ATerm three)
    {
      return (ATerm) ATmakeAppl3(fun_trip,one,two,three);
    }

    ATerm third(ATerm trip)
    {
      return ATgetArgument((ATermAppl) trip,2);
    }

  public:
    torx_data(unsigned int initial_size)
    {
      stateactions = ATindexedSetCreate(initial_size,50);
      state_indices = ATtableCreate(initial_size,50);
      fun_trip = ATmakeAFun("@trip@",2,ATfalse);
      ATprotectAFun(fun_trip);
      num_indices = 0;
    }

    ~torx_data()
    {
      ATunprotectAFun(fun_trip);
      ATtableDestroy(state_indices);
      ATindexedSetDestroy(stateactions);
    }

    index_pair add_action_state(ATerm from, ATerm action, ATerm to)
    {
      ATbool is_new;
      index_pair p;

      p.action = ATindexedSetPut(stateactions,triple(from,action,to),&is_new);
      if ( is_new == ATtrue )
      {
        num_indices = num_indices + 1;
      }

      ATerm i;
      if ( (i = ATtableGet(state_indices,to)) == NULL )
      {
        ATtablePut(state_indices,to,(ATerm) ATmakeInt(p.action));
        p.state = p.action;
      } else {
        p.state = ATgetInt((ATermInt) i);
      }

      return p;
    }

    ATerm get_state(unsigned int index)
    {
      if ( index < num_indices )
      {
        return third(ATindexedSetGetElem(stateactions,index));
      } else {
        return NULL;
      }
    }
};

struct tool_options_type {
  bool            usedummies;
  bool            removeunused; 
  RewriteStrategy strategy;
  int             stateformat;
  std::string     name_for_input;
};

tool_options_type parse_command_line(int ac, char** av) {
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... INFILE\n",
    "Provide a TorX explorer interface to the LPS in INFILE. "
    "\n\n"
    "The LPS can be explored using TorX as described in torx_explorer(5).");

  clinterface.add_rewriting_options();

  clinterface.
    add_option("dummy", make_mandatory_argument("BOOL"),
      "replace free variables in the LPS with dummy values based on the value of BOOL: 'yes' (default) or 'no'", 'y').
    add_option("unused-data",
      "do not remove unused parts of the data specification", 'u').
    add_option("state-format", make_mandatory_argument("NAME"),
      "store state internally in format NAME:\n"
      "  'vector' for a vector (fastest, default), or\n"
      "  'tree' for a tree (for memory efficiency)"
      , 'f');

  command_line_parser parser(clinterface, ac, av);

  tool_options_type options;

  options.usedummies   = true;
  options.removeunused = parser.options.count("unused-data") == 0;
  options.strategy     = parser.option_argument_as< RewriteStrategy >("rewriter");
  options.stateformat  = GS_STATE_VECTOR;

  if (parser.options.count("dummy")) {
    if (parser.options.count("dummy") > 1) {
      parser.error("multiple use of option -y/--dummy; only one occurrence is allowed");
    }
    std::string dummy_str(parser.option_argument("dummy"));
    if (dummy_str == "yes") {
      options.usedummies = true;
    } else if (dummy_str == "no") {
      options.usedummies = false;
    } else {
      parser.error("option -y/--dummy has illegal argument '" + dummy_str + "'");
    }
  }

  if (parser.options.count("state-format")) {
    if (parser.options.count("state-format") > 1) {
      parser.error("multiple use of option -f/--state-format; only one occurrence is allowed");
    }
    std::string state_format_str(parser.option_argument("state-format"));
    if (state_format_str == "vector") {
      options.stateformat = GS_STATE_VECTOR;
    } else if (state_format_str == "tree") {
      options.stateformat = GS_STATE_TREE;
    } else {
      parser.error("option -f/--state-format has illegal argument '" + state_format_str + "'");
    }
  }

  if (parser.arguments.size() == 0) {
    parser.error("no INFILE specified");
  } else if (parser.arguments.size() == 1) {
    options.name_for_input = parser.arguments[0];
  } else {
    //parser.arguments.size() > 1
    parser.error("too many file arguments");
  }

  return options;
}

int main(int argc, char **argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  try {
    tool_options_type options = parse_command_line(argc, argv);

    std::string str_in = (options.name_for_input.empty())?"stdin":("'" + options.name_for_input + "'");
    gsVerboseMsg("reading LPS from %s\n", str_in.c_str());
    ATermAppl Spec = (ATermAppl) mcrl2::core::detail::load_aterm(options.name_for_input);
    if (!mcrl2::core::detail::gsIsLinProcSpec(Spec)) {
      throw mcrl2::runtime_error(str_in + " does not contain an LPS");
    }
 
    if ( options.removeunused )
    {
      gsVerboseMsg("removing unused parts of the data specification.\n");
      Spec = removeUnusedData(Spec);
    }
 
    gsVerboseMsg("initialising...\n");
    torx_data td(10000);
 
    NextState *nstate = createNextState(
      Spec,
      !options.usedummies,
      options.stateformat,
      createEnumerator(
        data_specification(ATAgetArgument(Spec,0)),
        createRewriter(data_specification(ATAgetArgument(Spec,0)),options.strategy),
        true
      ),
      true
    );
 
    ATerm initial_state = nstate->getInitialState();
 
    ATerm dummy_action = (ATerm) ATmakeAppl0(ATmakeAFun("@dummy_action@",0,ATfalse));
    td.add_action_state(initial_state,dummy_action,initial_state);
 
    gsVerboseMsg("generating state space...\n");
 
    NextStateGenerator *nsgen = NULL;
    bool err = false;
    bool notdone = true;
    while ( notdone && !cin.eof() )
    {
      string s;
 
      cin >> s;
      if ( s.size() != 1 )
      {
              cout << "A_ERROR UnknownCommand: unknown or unimplemented command '" << s << "'" << endl;
              continue;
      }
 
      switch ( s[0] )
      {
        case 'r': // Reset
          // R event TAB solved TAB preds TAB freevars TAB identical
          cout << "R 0\t1\t\t\t" << endl;
          break;
        case 'e': // Expand
          {
          int index;
          ATerm state;
          
          cin >> index;
          state = td.get_state( index );
          if ( state == NULL )
          {
          	cout << "E0 value " << index << " not valid" << endl;
          	break;
          }
      
          cout << "EB" << endl;
          nsgen = nstate->getNextStates(state,nsgen);
          ATermAppl Transition;
          ATerm NewState;
          while ( nsgen->next(&Transition,&NewState) )
          {
            index_pair p;
      
            p = td.add_action_state(state,(ATerm) Transition,NewState);
 
            // Ee event TAB visible TAB solved TAB label TAB preds TAB freevars TAB identical
            cout << "Ee " << p.action << "\t" << (is_tau(Transition)?0:1) << "\t1\t";
            print_torx_action(cout,Transition);
            cout << "\t\t\t";
            if ( p.action != p.state )
            {
              cout << p.state;
            }
            cout << endl;
          }
          cout << "EE" << endl;
 
          if ( nsgen->errorOccurred() )
          {
            err = true;
            notdone = false;
          }
          break;
          }
        case 'q': // Quit
          cout << "Q" << endl;
          notdone = false;
          break;
        default:
          cout << "A_ERROR UnknownCommand: unknown or unimplemented command '" << s << "'" << endl;
          break;
      }
    }
    delete nsgen;
    delete nstate;
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_FAILURE;
}
