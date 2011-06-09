// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sim.cpp

#include "boost.hpp" // precompiled headers

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

#include "mcrl2/lps/multi_action.h"

#include "simulator.h"

using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::lps;

class torx_tool : public rewriter_tool< input_tool >
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

    atermpp::set<ATerm>     visited_states;
    atermpp::map<int,ATerm> int_to_seenstate_mapping;
    atermpp::map<ATerm,int> seenstate_to_int_mapping;

    std::string print_torx_transtion( multi_action ma )
    {
      if( ma.actions().empty() )
      {
        return "tau";
      }

      std::string result;
      for (action_list::const_iterator i=ma.actions().begin(); i!=ma.actions().end(); ++i)
      {
        result = result+pp(i->label());
        for (mcrl2::data::data_expression_list::const_iterator arg=i->arguments().begin(); arg!=i->arguments().end(); ++arg)
        {
          result = result+"!"+pp( *arg );
        }

        action_list::const_iterator i_next=i;
        i_next++;
        if (i_next!=ma.actions().end())
        {
          result=result+"!";
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
      mcrl2::lps::specification lps_specification;

      lps_specification.load(m_input_filename);

      StandardSimulator simulator;
      simulator.rewr_strat = rewrite_strategy();
      simulator.use_dummies = m_use_dummies;
      simulator.LoadSpec(lps_specification);

      visited_states.clear();
      int_to_seenstate_mapping.clear();
      seenstate_to_int_mapping.clear();

      int max_state = 0;

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
            visited_states.clear();
            visited_states.insert( simulator.GetState() );

            int_to_seenstate_mapping.clear();
            int_to_seenstate_mapping[ 0 ] = simulator.GetState();

            max_state=0;

            break;
          }
          else if (s[0] == 'e')
          {

            if(s[1] != ' ' || s.size() <= 2)
            {
              std::cout << "incorrect syntax (try 'h' for help)" << std::endl;
              break;
            }

            int index = atoi(s.substr(1, s.size()-1 ).c_str());
            atermpp::map<int,ATerm>::iterator it = int_to_seenstate_mapping.find( index );

            if (it == int_to_seenstate_mapping.end())
            {
              std::cout << "E0 value " << index << " not valid" << std::endl;
              break;
            }

            try
            {
              simulator.Reset( it->second );
              ATermList next_states=ATempty;

              visited_states.insert( simulator.GetState() );
              next_states = simulator.GetNextStates();

              std::cout << "EB" << std::endl;
              for (ATermList l=next_states; !ATisEmpty(l); l=ATgetNext(l))
              {
                max_state++;

                ATermAppl Transition = ATAgetFirst(ATLgetFirst(l));
                ATerm NewState = ATgetFirst(ATgetNext(ATLgetFirst(l)));

                /* Interpret Transition as multi-action*/
                multi_action ma = multi_action(Transition);
                int is_tau = ma.actions().empty()?0:1;

                /* Rebuild transition string in Torx format*/
                std::cout << "Ee " << max_state << "\t" << is_tau << "\t" << 1 << "\t" << print_torx_transtion(ma) << "\t\t\t";
                int_to_seenstate_mapping[max_state] = NewState;

                /* Print optional first-encounted visited state */
                if(seenstate_to_int_mapping.find(NewState) == seenstate_to_int_mapping.end())
                {
                  seenstate_to_int_mapping[ NewState ] = max_state;
                }
                else
                {
                  std::cout << seenstate_to_int_mapping.find(NewState)->second;
                }
                std::cout << std::endl;
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
  MCRL2_ATERMPP_INIT(argc, argv)

  return torx_tool().execute(argc, argv);
}

