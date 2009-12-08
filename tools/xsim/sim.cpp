// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sim.cpp

#include "boost.hpp" // precompiled headers

#define NAME "sim"
#define AUTHOR "Muck van Weerdenburg"

#include <iostream>
#include <string>
#include <cctype>

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "aterm2.h"

#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/nextstate.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/exception.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

#include "simulator.h"

using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;

class sim_tool : public rewriter_tool< input_tool > {

  typedef rewriter_tool<input_tool> super;

  private:
    static void PrintState(ATerm state, NextState *ns)
    {
      for (int i=0; i<ns->getStateLength(); i++)
      {
        if ( i > 0 )
        {
          gsMessage(", ");
        }

        ATermAppl a = ns->getStateArgument(state,i);
        if ( mcrl2::data::data_expression(a).is_variable() )
        {
          gsMessage("_");
        } else {
          gsMessage("%P",a);
        }
      }
    }

  protected:

    bool m_use_dummies;

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("dummy", "replace free variables in the LPS with dummy values", 'y');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      m_use_dummies = 0 < parser.options.count("dummy");
    }

  public:

    sim_tool()
        : super(
            "sim",
            "Muck van Weerdenburg",
            "command-line simulation of an LPS",
            "Simulate the LPS in INFILE via a text-based interface."
          ),
      m_use_dummies(false)
    {}

    bool run()
    {
      mcrl2::lps::specification lps_specification;

      lps_specification.load(m_input_filename);

      StandardSimulator simulator;
      simulator.rewr_strat = rewrite_strategy();
      simulator.use_dummies = m_use_dummies;
      simulator.LoadSpec(lps_specification);

      gsMessage("initial state: [ ");
      PrintState(simulator.GetState(),simulator.GetNextState());
      gsMessage(" ]\n\n");

      bool notdone = true;
      while ( notdone )
      { ATermList next_states=ATempty;
        try
        { next_states = simulator.GetNextStates();
          int i = 0;
          for (ATermList l=next_states; !ATisEmpty(l); l=ATgetNext(l) )
          {
            ATermAppl Transition = ATAgetFirst(ATLgetFirst(l));
            ATerm NewState = ATgetFirst(ATgetNext(ATLgetFirst(l)));
            gsMessage("%i: %P  ->  [ ",i,Transition);
            PrintState(NewState,simulator.GetNextState());
            gsMessage(" ]\n\n");
            i++;
          }
          if ( ATisEmpty(next_states) )
          { printf("deadlock\n\n");
          }
        } 
        catch (mcrl2::runtime_error e)
        // if ( simulator.ErrorOccurred() )
        { std::cerr << "an error occurred while calculating the transitions from this state;\n" << e.what() << "\n";
        // { gsMessage(std::string("an error occurred while calculating the transitions from this state;\n") + e.what() + "\n");
        }

        while ( true )
        {
          std::string s;

          (std::cout << "? ").flush();
          getline(std::cin, s);
          if ( (s.length() > 0) && (s[s.length()-1] == '\r') )
          { // remove CR
            s.resize(s.length()-1);
          }

          if ( std::cin.eof() || (s == "q") || (s == "quit") )
          {
            if ( std::cin.eof() )
              std::cout << std::endl;
            notdone = false;
            break;
          } else if ( (s == "h") || (s == "help") ) {
            std::cout <<
             "During the simulation the following commands are accepted (short/long):\n"
             "   N                take transition N to the corresponding state\n"
             "                    (where N is a number)\n"
             "   u/undo           go to previous state in trace\n"
             "   r/redo           go to next state in trace\n"
             "   i/initial        go to initial state (preserving trace)\n"
             "   g/goto N         go to position N in trace\n"
             "   t/trace          print trace (current state is indicated with '>')\n"
             "   l/load FILENAME  load trace from file FILENAME\n"
             "   s/save FILENAME  save trace to file FILENAME\n"
             "   h/help           print this help gsMessage\n"
             "   q/quit           quit\n";
          } 
          else if ( isdigit(s[0]) ) 
          { unsigned int idx;
            sscanf(s.c_str(),"%u",&idx);
            if (idx < (unsigned int)ATgetLength(next_states))
            {
              gsMessage("\ntransition: %P\n\n",ATAgetFirst(ATLelementAt(next_states,idx)));
              simulator.ChooseTransition(idx);
              gsMessage("current state: [ ");
              PrintState(simulator.GetState(),simulator.GetNextState());
              gsMessage(" ]\n\n");
              break;
            } else {
              std::cout << "invalid transition index";
              if ( ATgetLength(next_states) > 0 )
              {
                std::cout << " " << idx << " (maximum is " << ATgetLength(next_states)-1 << ")";
              }
              std::cout << std::endl;
            }
          } else if ( (s == "i") || (s == "initial") )
          {
            simulator.SetTracePos(0);
            gsMessage("\ninitial state: [ ");
            PrintState(simulator.GetState(),simulator.GetNextState());
            gsMessage(" ]\n\n");
            break;
          } else if ( (s == "u") || (s == "undo") )
          {
            if ( simulator.GetTracePos() > 0 )
            {
              simulator.Undo();
              gsMessage("\ncurrent state: [ ");
              PrintState(simulator.GetState(),simulator.GetNextState());
              gsMessage(" ]\n\n");
              break;
            } else {
              std::cout << "already at start of trace" << std::endl;
            }
          } else if ( (s == "r") || (s == "redo") )
          {
            ATermAppl trans = simulator.GetNextTransitionFromTrace();
            if ( trans != NULL )
            {
              simulator.Redo();
              gsMessage("\ntransition: %P\n\n",trans);
              gsMessage("current state: [ ");
              PrintState(simulator.GetState(),simulator.GetNextState());
              gsMessage(" ]\n\n");
              break;
            } else {
              std::cout << "already at end of trace" << std::endl;
            }
          } else if ( (s.substr(0,2) == "g ") || (s.substr(0,5) == "goto ") )
          {
            std::istringstream sin(((s[1] == ' ') ? s.substr(2) : s.substr(5)));
            int idx;
            sin >> idx;
            if ( idx >= 0 && idx < simulator.GetTraceLength() )
            {
              simulator.SetTracePos(idx);
              gsMessage("\ncurrent state: [ ");
              PrintState(simulator.GetState(),simulator.GetNextState());
              gsMessage(" ]\n\n");
              break;
            } else {
              std::cout << "invalid trace position " << idx << " (maximum is " << (simulator.GetTraceLength()-1) << ")" << std::endl;
            }
          } else if ( (s == "t") || (s == "trace") )
          {
            gsMessage("\ncurrent trace:\n\n");
            ATermList trace = simulator.GetTrace();
            int pos = simulator.GetTracePos();
            for (int i=0; !ATisEmpty(trace); trace=ATgetNext(trace), ++i)
            {
              ATermAppl Transition = ATAgetFirst(ATLgetFirst(trace));
              ATerm NewState = ATgetFirst(ATgetNext(ATLgetFirst(trace)));
              gsMessage("%s %i: ",(i==pos)?">":" ",i);
              if ( i == 0 )
              {
                gsMessage("    ");
              } else {
              gsMessage("%P  ->",Transition);
              }
              gsMessage("  [ ",(i==pos)?">":" ",i,Transition);
              PrintState(NewState,simulator.GetNextState());
              gsMessage(" ]\n\n");
            }
          } else if ( (s.substr(0,2) == "s ") || (s.substr(0,5) == "save ") )
          {
            std::string filename((s[1] == ' ') ? s.substr(2) : s.substr(5));
            try
            {
              simulator.SaveTrace(filename);
              std::cout << "trace saved" << std::endl;
            } catch ( std::string err )
            {
              std::cout << "error saving trace: " << err << std::endl;
            }
          } else if ( (s.substr(0,2) == "l ") || (s.substr(0,5) == "load ") )
          {
            std::string filename((s[1] == ' ') ? s.substr(2) : s.substr(5));
            try
            {
              simulator.LoadTrace(filename);
              std::cout << "trace loaded" << std::endl;
              gsMessage("\ninitial state: [ ");
              PrintState(simulator.GetState(),simulator.GetNextState());
              gsMessage(" ]\n\n");
              break;
            } catch ( std::string err )
            {
              std::cout << "error loading trace: " << err << std::endl;
            }
          } else {
            std::cout << "unknown command (try 'h' for help)" << std::endl;
          }
        }
      }

      return true;
    }
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return sim_tool().execute(argc, argv);
}

