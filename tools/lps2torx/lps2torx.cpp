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
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/detail/aterm_io.h"
#include "mcrl2/core/print.h"
#include "mcrl2/lps/nextstate.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/tool.h"
#include "mcrl2/utilities/rewriter_tool.h"

using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2;
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

typedef rewriter_tool< tool > lps2torx_base;
class lps2torx_tool : public lps2torx_base
{
  private:
    bool               usedummies;
    bool               removeunused;
    rewriter::strategy strategy;
    int                stateformat;
    std::string        name_for_input;

  public:
    lps2torx_tool() :
      lps2torx_base(NAME,AUTHOR,
        "provide TorX explorer interface to an LPS",
        "Provide a TorX explorer interface to the LPS in INFILE. "
        "\n\n"
        "The LPS can be explored using TorX as described in torx_explorer(5)."
      )
    {
    }

    bool run()
    {
      std::string str_in = (name_for_input.empty())?"stdin":("'" + name_for_input + "'");
      gsVerboseMsg("reading LPS from %s\n", str_in.c_str());
      lps::specification lps_specification;
  
      lps_specification.load(name_for_input);
  
      if ( removeunused )
      {
        gsVerboseMsg("removing unused parts of the data specification.\n");
      }
  
      gsVerboseMsg("initialising...\n");
      torx_data td(10000);
  
      data::rewriter rewriter = ( removeunused ) ?
            data::rewriter(lps_specification.data(),
		mcrl2::data::used_data_equation_selector(lps_specification.data(), mcrl2::lps::specification_to_aterm(lps_specification, false)), strategy) :
            data::rewriter(lps_specification.data(), strategy);
      mcrl2::data::enumerator_factory< mcrl2::data::classic_enumerator< > > enumerator_factory(lps_specification.data(), rewriter);
  
      NextState *nstate = createNextState(
        lps_specification,
        enumerator_factory,
        !usedummies,
        stateformat
      );
  
      ATerm initial_state = nstate->getInitialState();
  
      ATerm dummy_action = (ATerm) ATmakeAppl0(ATmakeAFun("@dummy_action@",0,ATfalse));
      td.add_action_state(initial_state,dummy_action,initial_state);
  
      gsVerboseMsg("generating state space...\n");
  
      NextStateGenerator *nsgen = NULL;
      // bool err = false;
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
  
            /* if ( nsgen->errorOccurred() )
            {
              err = true;
              notdone = false;
            } */
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
      return true;
    }

  private:
    std::string synopsis() const
    {
      return "[OPTION]... INFILE";
    }

    void check_positional_options(const command_line_parser &parser)
    {
      if (1 < parser.arguments.size())
      {
        parser.error("too many file arguments");
      } else if (1 > parser.arguments.size())
      {
        parser.error("too few file arguments");
      }
    }

    void add_options(interface_description &desc)
    {
      lps2torx_base::add_options(desc);

      desc.
        add_option("dummy", make_mandatory_argument("BOOL"),
          "replace free variables in the LPS with dummy values based on the value of BOOL: 'yes' (default) or 'no'", 'y').
        add_option("unused-data",
          "do not remove unused parts of the data specification", 'u').
        add_option("state-format", make_mandatory_argument("NAME"),
          "store state internally in format NAME:\n"
          "  'vector' for a vector (fastest, default), or\n"
          "  'tree' for a tree (for memory efficiency)"
          , 'f');
    }

    void parse_options(const command_line_parser &parser)
    {
      lps2torx_base::parse_options(parser);

      usedummies   = true;
      removeunused = parser.options.count("unused-data") == 0;
      strategy     = parser.option_argument_as< rewriter::strategy >("rewriter");
      stateformat  = GS_STATE_VECTOR;

      if (parser.options.count("dummy")) {
        if (parser.options.count("dummy") > 1) {
          parser.error("multiple use of option -y/--dummy; only one occurrence is allowed");
        }
        std::string dummy_str(parser.option_argument("dummy"));
        if (dummy_str == "yes") {
          usedummies = true;
        } else if (dummy_str == "no") {
          usedummies = false;
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
          stateformat = GS_STATE_VECTOR;
        } else if (state_format_str == "tree") {
          stateformat = GS_STATE_TREE;
        } else {
          parser.error("option -f/--state-format has illegal argument '" + state_format_str + "'");
        }
      }

      if (parser.arguments.size() == 0) {
        parser.error("no INFILE specified");
      } else if (parser.arguments.size() == 1) {
        name_for_input = parser.arguments[0];
      } else {
        //parser.arguments.size() > 1
        parser.error("too many file arguments");
      }
    }
};

int main(int argc, char **argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return lps2torx_tool().execute(argc,argv);
}
