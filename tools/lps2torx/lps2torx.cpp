// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2torx.cpp

#define NAME "lps2torx"
#define AUTHOR "Muck van Weerdenburg, Frank Stappers"

#include <iostream>
#include <string>
#include <cctype>

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <deque>

#include "mcrl2/utilities/exception.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/lps/next_state_generator.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::lps;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class torx_tool : public rewriter_tool< input_tool >
{
    typedef rewriter_tool<input_tool> super;

  protected:
    std::string synopsis() const
    {
      return "[OPTION]... INFILE";
    }

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

      if (parser.arguments.size() == 0) {
        throw parser.error("no INFILE specified");
      } else if (parser.arguments.size() != 1) {
        throw parser.error("too many file arguments");
      }
    }

    /* Multi-actions:  a_1(d^1_1,...,d^1_m)|...|a_n(n^1_1, ..., d^n_m)
     * are printed as: a_1!d^1_1!...!d^1_m!...!a_n!n^1_1!...!d^n_m
     * */
    std::string print_torx_action(multi_action ma)
    {
      if (ma.actions().empty())
      {
        return "tau";
      }

      std::string result;
      for (auto i = ma.actions().begin(); i != ma.actions().end(); i++)
      {
        result += process::pp(i->label());
        for (data::data_expression_list::const_iterator j = i->arguments().begin(); j != i->arguments().end(); j++)
        {
          result += "!" + data::pp(*j);
        }

        auto next = i;
        next++;
        if (next != ma.actions().end())
        {
          result += "!";
        }
      }
      return result;
    }

  public:

    torx_tool()
      : super(NAME,AUTHOR,
          "provide TorX explorer interface to an LPS",
          "Provide a TorX explorer interface to the LPS in INFILE. "
          "\n\n"
          "The LPS can be explored using TorX as described in torx_explorer(5)."
      ),
      m_use_dummies(false)
    {}

    bool run()
    {
      specification spec;
      load_lps(spec, m_input_filename);

      next_state_generator generator(spec, data::rewriter(spec.data(), rewrite_strategy()));

      state current = generator.initial_state();
      std::deque<state> states;
      std::map<state, int> state_numbers;

      bool notdone = true;
      while (notdone)
      {

        while (true)
        {
          std::string s;

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
                      "   e N              expand transition N of the corresponding state\n"
                      "                    (where N is a number)\n"
                      "   r                reset to initial state\n"
                      "   h/help           print this help message\n"
                      "   q/quit           quit\n";
          }
          else if (s == "r")
          {
            // R event TAB solved TAB preds TAB freevars TAB identical
            std::cout << "R 0\t1\t\t\t" << std::endl;

            states.clear();
            states.push_back(current);

            state_numbers.clear();
            state_numbers[current] = 0;

            break;
          }
          else if (s[0] == 'e')
          {
            if(s[1] != ' ' || s.size() <= 2)
            {
              std::cout << "incorrect syntax (try 'h' for help)" << std::endl;
              break;
            }

            size_t index = atoi(s.substr(1, s.size()-1 ).c_str());
            if (index >= states.size())
            {
              std::cout << "E0 value " << index << " not valid" << std::endl;
              break;
            }

            try
            {
              std::cout << "EB" << std::endl;
              current = states[index];

              for(size_t summand_index = 0; summand_index < spec.process().action_summands().size(); ++summand_index)
              {
                next_state_generator::enumerator_queue_t enumeration_queue;
                for (next_state_generator::iterator i = generator.begin(current, summand_index, &enumeration_queue); i != generator.end(); i++)
                {
                  /* Rebuild transition string in Torx format*/
                  std::cout << "Ee " << "_e" << summand_index << "." << states.size() << "\t" << (i->action().actions().empty() ? 0 : 1) << "\t" << 1 << "\t" << print_torx_action(i->action()) << "\t\t\t";

                  state next = i->state();

                  /* Print optional first-encounted visited state */
                  if(state_numbers.find(next) == state_numbers.end())
                  {
                    state_numbers[next] = states.size();
                  }
                  else
                  {
                    std::cout << state_numbers[next];
                  }
                  std::cout << std::endl;

                  states.push_back(next);
                }
              }
              std::cout << "EE" << std::endl;

            }
            catch (mcrl2::runtime_error e)
            {
              std::cout << "E0 an error occurred while calculating the transitions from this state;\n" << e.what() << std::endl;
            }
            break;
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
  return torx_tool().execute(argc, argv);
}

