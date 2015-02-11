// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sim.cpp

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

#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/simulation.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2::lps;

using mcrl2::data::tools::rewriter_tool;

class sim_tool : public rewriter_tool< input_tool >
{
    typedef rewriter_tool<input_tool> super;

  private:
    static std::string print_state(state s)
    {
      std::string output;
      bool first = true;
      for (state::iterator i = s.begin(); i != s.end(); i++)
      {
        if (!first)
        {
          output += ", ";
        }
        first = false;

        if (mcrl2::data::is_variable(*i))
        {
          output += "_";
        }
        else
        {
          output += data::pp(*i);
        }
      }
      return "[" + output + "]";
    }

  protected:

    bool m_do_not_use_dummies;

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("nodummy", "do not replace global variables in the LPS with dummy values", 'y');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      m_do_not_use_dummies = 0 < parser.options.count("nodummy");
    }

  public:

    sim_tool()
      : super(
        "lpssim",
        "Muck van Weerdenburg",
        "command-line simulation of an LPS",
        "Simulate the LPS in INFILE via a text-based interface."
      ),
      m_do_not_use_dummies(false)
    {}

    bool run()
    {
      mcrl2::lps::specification spec;
      load_lps(spec, input_filename());

      if (!m_do_not_use_dummies)
      {
        lps::detail::instantiate_global_variables(spec);
      }

      lps::simulation simulation(spec, rewrite_strategy());
      size_t state_index = 0;


      bool notdone = true;
      while (notdone)
      {
        assert(state_index < simulation.trace().size());
        const simulation::state_t &current_state = simulation.trace()[state_index];
        std::cout << std::endl << (state_index == 0 ? "initial" : "current") << " state: " << print_state(current_state.source_state) << std::endl;

        for (size_t i = 0; i < current_state.transitions.size(); i++)
        {
          std::cout << i << ": " << lps::pp(current_state.transitions[i].action) << "  ->  " << print_state(current_state.transitions[i].destination) << std::endl;
        }

        if (current_state.transitions.size() == 0)
        {
          std::cout << "deadlock" << std::endl;
        }

        while (true)
        {
          std::string s;

          std::cout << "? ";
          std::cout.flush();
          getline(std::cin, s);
          if ((s.length() > 0) && (s[s.length()-1] == '\r'))
          {
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
            size_t index;
            std::stringstream ss(s);
            ss >> index;
            if (index < current_state.transitions.size())
            {
              simulation.truncate(state_index);
              simulation.select(index);
              std::cout << std::endl << "transition: " << lps::pp(current_state.transitions[index].action) << std::endl;
              state_index++;
              break;
            }
            else
            {
              std::cout << "invalid transition index";
              if (current_state.transitions.size() > 0)
              {
                std::cout << " " << index << " (maximum is " << current_state.transitions.size() - 1 << ")";
              }
              std::cout << std::endl;
            }
          }
          else if ((s == "i") || (s == "initial"))
          {
            state_index = 0;
            break;
          }
          else if ((s == "u") || (s == "undo"))
          {
            if (state_index > 0)
            {
              state_index--;
              break;
            }
            else
            {
              std::cout << "already at start of trace" << std::endl;
            }
          }
          else if ((s == "r") || (s == "redo"))
          {
            if (state_index + 1 < simulation.trace().size())
            {
              state_index++;
              std::cout << std::endl << "transition: " << lps::pp(current_state.transitions[current_state.transition_number].action) << std::endl;
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
            size_t index;
            sin >> index;
            if (index < simulation.trace().size())
            {
              state_index = index;
              break;
            }
            else
            {
              std::cout << "invalid trace position " << index << " (maximum is " << (current_state.transitions.size() - 1) << ")" << std::endl;
            }
          }
          else if ((s == "t") || (s == "trace"))
          {
            std::cout << std::endl << "current trace:" << std::endl;
            for (size_t i = 0; i < simulation.trace().size(); i++)
            {
              if (i == state_index)
              {
                std::cout << ">";
              }
              else
              {
                std::cout << " ";
              }

              if (i == 0)
              {
                std::cout << "initial state: ";
              }
              else
              {
                std::cout << lps::pp(simulation.trace()[i - 1].transitions[simulation.trace()[i - 1].transition_number].action) << "  ->  ";
              }

              std::cout << print_state(simulation.trace()[i].source_state) << std::endl;
            }
            break;
          }
          else if ((s.substr(0,2) == "s ") || (s.substr(0,5) == "save "))
          {
            std::string filename((s[1] == ' ') ? s.substr(2) : s.substr(5));
            try
            {
              simulation.save(filename);
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
              simulation.load(filename);
              state_index = 0;
              std::cout << "trace loaded" << std::endl;
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
  return sim_tool().execute(argc, argv);
}
