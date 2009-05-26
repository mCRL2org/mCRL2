// Author(s): F.P.M. (Frank) Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./lpsinfo.cpp

/*include "boost.hpp" // precompiled headers*/

#define TOOLNAME "lpsinfo"
#define AUTHOR "Frank Stappers"

//C++
#include <exception>
#include <set>
#include <algorithm>
#include <sstream>

//tool framework
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/squadt_tool.h"

//LPS framework
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/linear_process.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using namespace mcrl2::utilities::tools;


template <class T>
inline std::string to_string (const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

static inline unsigned int get_number_of_tau_summands(const linear_process &lps)
{ unsigned int num_of_tau = 0;
  for(summand_list::iterator current_summand = lps.summands().begin();
            current_summand != lps.summands().end(); ++current_summand)
  { if (current_summand->is_tau())
    { ++num_of_tau;
    }
  }
  return num_of_tau;
}

static inline std::set<action_label > get_used_actions(linear_process lps)
{ std::set<action_label > actionSet;
  for(summand_list::iterator currentSummand = lps.summands().begin(); currentSummand != lps.summands().end(); ++currentSummand)
  { for(action_list::iterator currentAction = currentSummand->actions().begin();
        currentAction != currentSummand->actions().end();
        ++currentAction)
    { actionSet.insert(currentAction->label());
    }
  }
  return actionSet;
}

static inline std::set<action_list > used_multiactions(linear_process lps)
{ std::set<action_list> multiActionSet;
  for(summand_list::iterator currentSummand = lps.summands().begin(); currentSummand != lps.summands().end(); ++currentSummand)
  { if (currentSummand->actions().size() > 1)
    { multiActionSet.insert(currentSummand->actions());
    }
  }
  return multiActionSet;
}

static inline std::set<action_label > get_unused_actions(specification lps_specification ){
  action_label_list action_list =lps_specification.action_labels();
  std::set<action_label > actionSet;
  for(action_label_list::iterator i = action_list.begin(); i != action_list.end(); ++i )
  { actionSet.insert(*i);
  }

  linear_process lps = lps_specification.process();
  for(summand_list::iterator currentSummand = lps.summands().begin(); currentSummand != lps.summands().end(); ++currentSummand){
	for(action_list::iterator currentAction = currentSummand->actions().begin();
        currentAction != currentSummand->actions().end();
        ++currentAction)
    {
		  actionSet.erase(currentAction->label());
	}
  }
  return actionSet;
}

class lpsinfo_tool: public squadt_tool<input_tool>
{
  protected:
    typedef squadt_tool< input_tool> super;

  public:
    lpsinfo_tool()
      : super(
          TOOLNAME,
          AUTHOR,
          "display basic information about an LPS",
          "Print basic information on the linear process specification (LPS) in INFILE.")
     {}

  private:
    void parse_options(const command_line_parser& parser)
    { super::parse_options(parser);
    }

    void add_options(interface_description& desc)
    { super::add_options(desc);
    }

  public:

    bool run()
    {
      specification lps_specification;

      lps_specification.load(m_input_filename);
      linear_process lps = lps_specification.process();

      std::set<action_label > action_labels = get_used_actions(lps);
      std::set<action_label > unused_action_labels = get_unused_actions(lps_specification);

      cout << "Input read from " << ((m_input_filename.empty())?"stdin":("'" + m_input_filename + "'")) << endl;
      cout << endl;
      cout << "Number of summands                : " << lps.summands().size() << endl;
      cout << "Number of tau-summands            : " << get_number_of_tau_summands(lps) << endl;
      cout << "Number of free variables          : " << lps_specification.initial_process().free_variables().size() + lps.free_variables().size() << endl;
      cout << "Number of process parameters      : " << lps.process_parameters().size() << endl;
      cout << "Number of declared actions        : " << lps_specification.action_labels().size() << endl;
      cout << "Number of used actions            : " << action_labels.size() << endl;
      if (unused_action_labels.size()>0)
      { cout << "Labels of unused actions : " ;
        for(std::set<action_label >::iterator i = unused_action_labels.begin();
                                  i != unused_action_labels.end(); ++i)
        { if (i!=unused_action_labels.begin())
          { cout << ", ";
          }
          cout << pp(*i) ;
        }
        cout << endl;
      }
      cout << "Number of used multi-actions      : " << used_multiactions(lps).size() << endl;
      cout << "Number of declared sorts          : " << boost::distance(lps_specification.data().sorts()) << endl;

      return true;
    }

#ifdef ENABLE_SQUADT_CONNECTIVITY
    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& c) const
    { c.add_input_configuration("lps_in", tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::reporting);
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration&)
    {}

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& c) const
    {
      return c.input_exists("lps_in");
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration& c)
    {
      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;

      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(c);

      specification lps_specification;

      lps_specification.load(c.get_input(m_input_filename).location());

      linear_process lps = lps_specification.process();

      std::set<action_label > action_labels = get_used_actions(lps);
      std::set<action_label > unused_action_labels = get_unused_actions(lps_specification);

      /* Create display */
      tipi::tool_display d;

      layout::horizontal_box& m = d.create< horizontal_box >().set_default_margins(margins(0, 5, 0, 5));

      /* First column */
      m.append(d.create< vertical_box >().set_default_alignment(layout::left).
                append(d.create< label >().set_text("Input read from:")).
                append(d.create< label >().set_text("Summands (#):")).
                append(d.create< label >().set_text("Tau-summands (#):")).
                append(d.create< label >().set_text("Free variables (#):")).
                append(d.create< label >().set_text("Process parameters (#):")).
                append(d.create< label >().set_text("Action labels (#):")).
                append(d.create< label >().set_text("Used actions: (#):")).
                append(d.create< label >().set_text("Multi-actions: (#):")).
                append(d.create< label >().set_text("Sorts (#):")));

      /* Second column */
      m.append(d.create< vertical_box >().set_default_alignment(layout::left).
                append(d.create< label >().set_text(c.get_input(m_input_filename).location())).
                append(d.create< label >().set_text(to_string(lps.summands().size()))).
                append(d.create< label >().set_text(to_string(get_number_of_tau_summands(lps)))).
                append(d.create< label >().set_text(to_string((lps_specification.initial_process().free_variables().size() + lps.free_variables().size())))).
                append(d.create< label >().set_text(to_string(lps.process_parameters().size()))).
                append(d.create< label >().set_text(to_string(lps_specification.action_labels().size()))).
                append(d.create< label >().set_text(to_string(get_used_actions(lps).size()))).
                append(d.create< label >().set_text(to_string(used_multiactions(lps).size()))).
                append(d.create< label >().set_text(to_string(boost::distance(lps_specification.data().sorts())))));

      send_display_layout(d.manager(m));

      return true;
    }
#endif
};

int main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)
  return lpsinfo_tool().execute(argc, argv);
}
