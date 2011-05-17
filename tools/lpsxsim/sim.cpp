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

#define NAME "lpssim"
#define AUTHOR "Muck van Weerdenburg"

#include <iostream>
#include <string>
#include <cctype>

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "mcrl2/aterm/aterm2.h"

#include "mcrl2/core/messaging.h"
#include "mcrl2/aterm/aterm_ext.h"
#include "mcrl2/lps/specification.h"
// #include "mcrl2/lps/nextstate.h"
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

class sim_tool : public rewriter_tool< input_tool >
{

    typedef rewriter_tool<input_tool> super;

  private:
    static void PrintState(ATerm state, NextState* ns)
    {
      for (size_t i=0; i<ns->getStateLength(); i++)
      {
        if (i > 0)
        {
          std::cout << ", ";
        }

        ATermAppl a = ns->getStateArgument(state,i);
        if (mcrl2::data::is_variable(a))
        {
        	std::cout << "_";
        }
        else
        {
        	std::cout << pp(a);
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
        "lpssim",
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

      std::cout << "initial state: [ ";
      PrintState(simulator.GetState(),simulator.GetNextState());
      std::cout << " ]" << std::endl << std::endl;

      bool notdone = true;
      while (notdone)
      {
        ATermList next_states=ATempty;
        try
        {
          next_states = simulator.GetNextStates();
          size_t i = 0;
          for (ATermList l=next_states; !ATisEmpty(l); l=ATgetNext(l))
          {
            ATermAppl Transition = ATAgetFirst(ATLgetFirst(l));
            ATerm NewState = ATgetFirst(ATgetNext(ATLgetFirst(l)));
            std::cout << i <<": " << pp(Transition) << "  ->  [ ";
            PrintState(NewState,simulator.GetNextState());
            std::cout << " ]" << std::endl << std::endl;
            i++;
          }
          if (ATisEmpty(next_states))
          {
            std::cout << "deadlock" << std::endl << std::endl;
          }
        }
        catch (mcrl2::runtime_error e)
        {
          std::cerr << "an error occurred while calculating the transitions from this state;\n" << e.what() << std::endl;
        }

        while (true)
        {
          std::string s;

          (std::cout << "? ").flush();
          getline(std::cin, s);
          if ((s.length() > 0) && (s[s.length()-1] == '\r'))
          {
            // remove CR
            s.resize(s.length()-1);
          }

          if (std::cin.eof() || (s == "q") || (s == "quit"))
          {
            if (std::cin.eof())
            {
              std::cout << std::endl;
            }
            notdone = false;
            break;
          }
          else if ((s == "h") || (s == "help"))
          {
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
                      "   h/help           print this help message\n"
                      "   q/quit           quit\n";
          }
          else if (isdigit(s[0]))
          {
            size_t idx;
            sscanf(s.c_str(),"%lu",&idx);
            if (idx < ATgetLength(next_states))
            {
              std::cout << std::endl << "transition: " << pp(ATAgetFirst(ATLelementAt(next_states,idx))) << std::endl << std::endl;
              simulator.ChooseTransition(idx);
              std::cout << "current state: [ ";
              PrintState(simulator.GetState(),simulator.GetNextState());
              std::cout << " ]" << std::endl << std::endl;
              break;
            }
            else
            {
              std::cout << "invalid transition index";
              if (ATgetLength(next_states) > 0)
              {
                std::cout << " " << idx << " (maximum is " << ATgetLength(next_states)-1 << ")";
              }
              std::cout << std::endl;
            }
          }
          else if ((s == "i") || (s == "initial"))
          {
            simulator.SetTracePos(0);
            std::cout << std::endl << "initial state: [ ";
            PrintState(simulator.GetState(),simulator.GetNextState());
            std::cout <<  " ]" << std::endl << std::endl;
            break;
          }
          else if ((s == "u") || (s == "undo"))
          {
            if (simulator.GetTracePos() > 0)
            {
              simulator.Undo();
              std::cout << std::endl << "current state: [ ";
              PrintState(simulator.GetState(),simulator.GetNextState());
              std::cout << " ]" << std::endl << std::endl;
              break;
            }
            else
            {
              std::cout << "already at start of trace" << std::endl;
            }
          }
          else if ((s == "r") || (s == "redo"))
          {
            ATermAppl trans = simulator.GetNextTransitionFromTrace();
            if (trans != NULL)
            {
              simulator.Redo();
              std::cout << std::endl << "transition: "<< pp(trans) << std::endl << std::endl;
              std::cout << "current state: [ ";
              PrintState(simulator.GetState(),simulator.GetNextState());
              std::cout << " ]" << std::endl << std::endl;
              break;
            }
            else
            {
              std::cout << "already at end of trace" << std::endl;
            }
          }
          else if ((s.substr(0,2) == "g ") || (s.substr(0,5) == "goto "))
          {
            std::istringstream sin(((s[1] == ' ') ? s.substr(2) : s.substr(5)));
            size_t idx;
            sin >> idx;
            if (idx < simulator.GetTraceLength())
            {
              simulator.SetTracePos(idx);
              std::cout << std::endl << "current state: [ ";
              PrintState(simulator.GetState(),simulator.GetNextState());
              std::cout << " ]" << std::endl << std::endl;
              break;
            }
            else
            {
              std::cout << "invalid trace position " << idx << " (maximum is " << (simulator.GetTraceLength()-1) << ")" << std::endl;
            }
          }
          else if ((s == "t") || (s == "trace"))
          {
        	std::cout << std::endl << "current trace:" << std::endl << std::endl;
            ATermList trace = simulator.GetTrace();
            size_t pos = simulator.GetTracePos();
            for (size_t i=0; !ATisEmpty(trace); trace=ATgetNext(trace), ++i)
            {
              ATermAppl Transition = ATAgetFirst(ATLgetFirst(trace));
              ATerm NewState = ATgetFirst(ATgetNext(ATLgetFirst(trace)));
              if (i==pos)
                { std::cout << ">"; }
              else
                { std::cout << " "; }
              std::cout << i;
              if (i == 0)
              {
            	  std::cout << "    ";
              }
              else
              {
                std::cout << pp(Transition)<< "  ->";
              }
              std::cout <<"  [ ";
              if (i==pos)
                { std::cout << ">"; }
              else
                { std::cout << " "; }
              PrintState(NewState,simulator.GetNextState());
              std::cout << " ]" << std::endl << std::endl;
            }
          }
          else if ((s.substr(0,2) == "s ") || (s.substr(0,5) == "save "))
          {
            std::string filename((s[1] == ' ') ? s.substr(2) : s.substr(5));
            try
            {
              simulator.SaveTrace(filename);
              std::cout << "trace saved" << std::endl;
            }
            catch (std::string err)
            {
              std::cout << "error saving trace: " << err << std::endl;
            }
          }
          else if ((s.substr(0,2) == "l ") || (s.substr(0,5) == "load "))
          {
            std::string filename((s[1] == ' ') ? s.substr(2) : s.substr(5));
            try
            {
              simulator.LoadTrace(filename);
              std::cout << "trace loaded" << std::endl;
              std::cout << std::endl << "initial state: [ ";
              PrintState(simulator.GetState(),simulator.GetNextState());
              std::cout << " ]" << std::endl << std::endl;
              break;
            }
            catch (std::string err)
            {
              std::cout << "error loading trace: " << err << std::endl;
            }
          }
          else
          {
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

