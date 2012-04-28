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

#include "mcrl2/exception.h"
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/aterm_ext.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/simulator.h"
#include "mcrl2/lps/specification.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2::lps;

class sim_tool : public rewriter_tool< input_tool >
{

    typedef rewriter_tool<input_tool> super;

  private:
    static void PrintState(const mcrl2::lps::state s)
    {
      for (size_t i=0; i<s.size(); i++)
      {
        if (i > 0)
        {
          std::cout << ", ";
        }

        const data_expression t = s[i];
        if (mcrl2::data::is_variable(t))
        {
          std::cout << "_";
        }
        else
        {
          std::cout << pp(t);
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

      std::cout << std::endl << "initial state: [ ";
      PrintState(simulator.GetState());
      std::cout << " ]" << std::endl;

      bool notdone = true;
      while (notdone)
      {
        std::vector < mcrl2::lps::state > next_states;
        std::vector < mcrl2::lps::multi_action > next_actions;

        try
        {
          next_states = simulator.GetNextStates();
          next_actions = simulator.GetNextActions();
          for (size_t i=0; i<next_states.size(); ++i)
          {
            const multi_action Transition = next_actions[i];
            const state NewState = next_states[i];
            std::cout << i <<": " << pp(Transition) << "  ->  [ ";
            PrintState(NewState);
            std::cout << " ]" << std::endl;
          }
          if (next_states.empty())
          {
            std::cout << "deadlock" << std::endl;
          }
        }
        catch (mcrl2::runtime_error& e)
        {
          mCRL2log(mcrl2::log::error) << "an error occurred while calculating the transitions from this state;\n" << e.what() << std::endl;
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
            sscanf(s.c_str(),"%zu",&idx);
            if (idx < next_states.size())
            {
              std::cout << std::endl << "transition: " << pp(next_actions[idx]) << std::endl;
              simulator.ChooseTransition(idx);
              std::cout << "current state: [ ";
              PrintState(simulator.GetState());
              std::cout << " ]" << std::endl;
              break;
            }
            else
            {
              std::cout << "invalid transition index";
              if (next_states.size() > 0)
              {
                std::cout << " " << idx << " (maximum is " << next_states.size()-1 << ")";
              }
              std::cout << std::endl;
            }
          }
          else if ((s == "i") || (s == "initial"))
          {
            simulator.SetTracePos(0);
            std::cout << std::endl << "initial state: [ ";
            PrintState(simulator.GetState());
            std::cout <<  " ]" << std::endl;
            break;
          }
          else if ((s == "u") || (s == "undo"))
          {
            if (simulator.GetTracePos() > 0)
            {
              simulator.Undo();
              std::cout << std::endl << "current state: [ ";
              PrintState(simulator.GetState());
              std::cout << " ]" << std::endl;
              break;
            }
            else
            {
              std::cout << "already at start of trace" << std::endl;
            }
          }
          else if ((s == "r") || (s == "redo"))
          {
            if (simulator.GetTraceLength() > simulator.GetTracePos())
            {
              multi_action trans = simulator.GetNextTransitionFromTrace();
              std::cout << std::endl << "transition: "<< pp(trans) << std::endl;
              simulator.Redo();
              std::cout << "current state: [ ";
              PrintState(simulator.GetState());
              std::cout << " ]" << std::endl;
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
            if (idx <= simulator.GetTraceLength())
            {
              simulator.SetTracePos(idx);
              std::cout << std::endl << "current state: [ ";
              PrintState(simulator.GetState());
              std::cout << " ]" << std::endl;
              break;
            }
            else
            {
              std::cout << "invalid trace position " << idx << " (maximum is " << (simulator.GetTraceLength()) << ")" << std::endl;
            }
          }
          else if ((s == "t") || (s == "trace"))
          {
            std::cout << std::endl << "current trace:" << std::endl;
            mcrl2::trace::Trace tr = simulator.GetTrace();
            size_t pos = simulator.GetTracePos();
            tr.setPosition(0);
            std::cout << "initial state: [ "; 
            PrintState(tr.currentState());
            std::cout << " ]" << std::endl;
            for (size_t i=0; i< tr.number_of_actions(); ++i)
            {
              tr.setPosition(i);
              state NewState = tr.nextState();
              multi_action Transition = tr.currentAction(); 
              if (i==pos)
                { std::cout << ">"; }
              else
                { std::cout << " "; }
              std::cout << i << ": " << pp(Transition)<< "  ->  [ ";
              if (i==pos)
                { std::cout << ">"; }
              else
                { std::cout << " "; }
              PrintState(NewState);
              std::cout << " ]" << std::endl;
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
            catch (mcrl2::runtime_error &err)
            {
              std::cout << "error saving trace: " << err.what() << std::endl;
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
              PrintState(simulator.GetState());
              std::cout << " ]" << std::endl;
              break;
            }
            catch (mcrl2::runtime_error &err)
            {
              std::cout << "error loading trace: " << err.what() << std::endl;
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

