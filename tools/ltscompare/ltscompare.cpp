// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltscompare.cpp

#define NAME "ltscompare"
#define AUTHOR "Muck van Weerdenburg"

#include <string>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/lts/lts_algorithm.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/tool.h"
#include "mcrl2/exception.h"

#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"

using namespace std;
using namespace mcrl2::lts;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;

static const char *equivalent_string(lts_equivalence eq)
{
  switch ( eq )
  {
    case lts_eq_bisim:
      return "strongly bisimilar";
    case lts_eq_branching_bisim:
      return "branching bisimilar";
    case lts_eq_divergence_preserving_branching_bisim:
      return "divergence preserving branching bisimilar";
    case lts_eq_sim:
      return "strongly simulation equivalent";
    case lts_eq_trace:
      return "strongly trace equivalent";
    case lts_eq_weak_trace:
      return "weak trace equivalent";
    default:
      return "equivalent";
  }
}

static const char *preorder_string(lts_preorder pre)
{
  switch ( pre )
  {
    case lts_pre_sim:
      return "strongly simulated by";
    case lts_pre_trace:
      return "strongly trace-included in";
    case lts_pre_weak_trace:
      return "weakly trace-included in";
    default:
      return "included in";
  }
}

static const std::set<lts_equivalence> &initialise_allowed_eqs()
{
  static std::set<lts_equivalence> s;
  s.insert(lts_eq_bisim);
  s.insert(lts_eq_branching_bisim);
  s.insert(lts_eq_divergence_preserving_branching_bisim);
  s.insert(lts_eq_sim);
  s.insert(lts_eq_trace);
  s.insert(lts_eq_weak_trace);
  return s;
}
static const std::set<lts_equivalence> &allowed_eqs()
{
  static const std::set<lts_equivalence> &s = initialise_allowed_eqs();
  return s;
}

struct t_tool_options 
{
  std::string     name_for_first;
  std::string     name_for_second;
  lts_type        format_for_first;
  lts_type        format_for_second;
  lts_equivalence equivalence;
  lts_preorder    preorder;
  // lts_eq_options  eq_opts;
  std::vector<std::string> tau_actions;   // Actions with these labels must be considered equal to tau.
  bool generate_counter_examples;
};

typedef squadt_tool< input_tool > ltscompare_base;
class ltscompare_tool : public ltscompare_base
{
  private:
    t_tool_options tool_options;

  public:
    ltscompare_tool() :
      ltscompare_base(NAME,AUTHOR,
        "compare two LTSs",
        "Determine whether or not the labelled transition systems (LTSs) in INFILE1 and INFILE2 are related by some equivalence or preorder. "
        "If INFILE1 is not supplied, stdin is used.\n"
        "\n"
        "The input formats are determined by the contents of INFILE1 and INFILE2. "
        "Options --in1 and --in2 can be used to force the input format of INFILE1 and INFILE2, respectively. "
        "The supported formats are:\n"
        + mcrl2::lts::detail::supported_lts_formats_text()
      )
    {
    }

    bool run()
    {
      lts l1,l2;

      if ( tool_options.name_for_first.empty() ) 
      {
        gsVerboseMsg("reading first LTS from stdin...\n");
        try
        {
           mcrl2::lts::detail::read_from(l1,std::cin, tool_options.format_for_first);
        }
        catch (mcrl2::runtime_error &e)
        {
          throw mcrl2::runtime_error(std::string("cannot read LTS from stdin\nretry with -v/--verbose for more information.\n")
                                        + e.what());
        }
      } 
      else 
      {
        gsVerboseMsg("reading first LTS from '%s'...\n", tool_options.name_for_first.c_str());

        try
        { 
           mcrl2::lts::detail::read_from(l1,tool_options.name_for_first, tool_options.format_for_first);
        }
        catch (mcrl2::runtime_error &e)
        { 
          throw mcrl2::runtime_error("cannot read LTS from file '" + tool_options.name_for_first + 
                   "'\nretry with -v/--verbose for more information.\n" + e.what());
        }
      }

      gsVerboseMsg("reading second LTS from '%s'...\n", tool_options.name_for_second.c_str());

      try
      {
         mcrl2::lts::detail::read_from(l2,tool_options.name_for_second, tool_options.format_for_second);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error("cannot read LTS from file '" + tool_options.name_for_second + 
                     "'\nretry with -v/--verbose for more information.\n" + e.what());
      }

      if (!l1.hide_actions(tool_options.tau_actions))
      { throw mcrl2::runtime_error("Cannot hide actions in first transition system");
      }

      if (!l2.hide_actions(tool_options.tau_actions))
      { throw mcrl2::runtime_error("Cannot hide actions in second transition system");
      }

      bool result = true;
      if ( tool_options.equivalence != lts_eq_none )
      {
        gsVerboseMsg("comparing LTSs using %s...\n",
            name_of_equivalence(tool_options.equivalence).c_str());

        result = compare(l1,l2,tool_options.equivalence,tool_options.generate_counter_examples);

        gsMessage("LTSs are %s%s\n",
            ((result) ? "" : "not "),
            equivalent_string(tool_options.equivalence));
      }

      if ( tool_options.preorder != lts_pre_none )
      {
        gsVerboseMsg("comparing LTSs using %s...\n",
            name_of_preorder(tool_options.preorder).c_str());

        result = compare(l1,l2,tool_options.preorder);

        gsMessage("LTS in %s is %s%s LTS in %s\n",
            tool_options.name_for_first.c_str(),
            ((result) ? "" : "not "),
            preorder_string(tool_options.preorder),
            tool_options.name_for_second.c_str() );
      }

      return result;
    }

  protected:
    std::string synopsis() const
    {
      return "[OPTION]... [INFILE1] INFILE2";
    }

    void check_positional_options(const command_line_parser &parser)
    {
      if (2 < parser.arguments.size())
      {
        parser.error("too many file arguments");
      } else if (1 > parser.arguments.size())
      {
        parser.error("too few file arguments");
      }
    }

    void set_tau_actions(std::vector <std::string>& tau_actions, std::string const& act_names)
    {
      std::string::size_type lastpos = 0, pos;
      while ( (pos = act_names.find(',',lastpos)) != std::string::npos )
      {
        tau_actions.push_back(act_names.substr(lastpos,pos-lastpos));
        lastpos = pos+1;
      }
      tau_actions.push_back(act_names.substr(lastpos));
    }

    void add_options(interface_description &desc)
    {
      ltscompare_base::add_options(desc);

      desc.
        add_option("in1", make_mandatory_argument("FORMAT"),
          "use FORMAT as the format for INFILE1 (or stdin)", 'i').
        add_option("in2", make_mandatory_argument("FORMAT"),
          "use FORMAT as the format for INFILE2", 'j').
        add_option("equivalence", make_mandatory_argument("NAME"),
          "use equivalence NAME:\n"
          +supported_lts_equivalences_text(allowed_eqs())+"\n"
          "(not allowed in combination with -p/--preorder)"
          , 'e').
        add_option("preorder", make_mandatory_argument("NAME"),
          "use preorder NAME:\n"
          +supported_lts_preorders_text()+"\n"
          "(not allowed in combination with -e/--equivalence)"
          , 'p').
        add_option("tau", make_mandatory_argument("ACTNAMES"),
          "consider actions with a name in the comma separated list ACTNAMES to "
          "be internal (tau) actions in addition to those defined as such by "
          "the input").
        add_option("counter-example",
          "generate counter example traces if the input lts's are not equivalent",'c');
    }

    void parse_options(const command_line_parser &parser)
    {
      ltscompare_base::parse_options(parser);

      if (parser.options.count("equivalence") > 1)
      {
        parser.error("multiple use of option -e/--equivalence; only one occurrence is allowed");
      }
  
      if (parser.options.count("preorder") > 1)
      {
        parser.error("multiple use of option -p/--preorder; only one occurrence is allowed");
      }
  
      if (parser.options.count("equivalence") + parser.options.count("preorder") > 1)
      {
        parser.error("options -e/--equivalence and -p/--preorder cannot be used simultaneously");
      }
  
      if (parser.options.count("equivalence") + parser.options.count("preorder") < 1)
      {
        parser.error("one of the options -e/--equivalence and -p/--preorder must be used");
      }

      if (parser.options.count("counter-example")>0 && parser.options.count("equivalence")==0)
      { 
        parser.error("counter examples can only be used in combination with an equivalence");
      }
  
      tool_options.equivalence = lts_eq_none;
  
      if (parser.options.count("equivalence")) {
  
        tool_options.equivalence = parse_equivalence(
            parser.option_argument("equivalence"));
  
        if ( allowed_eqs().count(tool_options.equivalence) == 0 )
        {
          parser.error("option -e/--equivalence has illegal argument '" +
              parser.option_argument("equivalence") + "'");
        }
      }
  
      tool_options.preorder = lts_pre_none;
  
      if (parser.options.count("preorder")) {
  
        tool_options.preorder = parse_preorder(
            parser.option_argument("preorder"));
  
        if (tool_options.preorder == lts_pre_none)
        {
          parser.error("option -p/--preorder has illegal argument '" +
              parser.option_argument("preorder") + "'");
        }
      }
  
      if (parser.options.count("tau")) 
      { set_tau_actions(tool_options.tau_actions, parser.option_argument("tau"));
      }
 
      tool_options.generate_counter_examples=parser.options.count("counter-example")>0;
  
      if (parser.arguments.size() == 1) {
        tool_options.name_for_second = parser.arguments[0];
      } else { // if (parser.arguments.size() == 2)
        tool_options.name_for_first  = parser.arguments[0];
        tool_options.name_for_second = parser.arguments[1];
      }
  
      if (parser.options.count("in1")) {
        if (1 < parser.options.count("in1")) {
          std::cerr << "warning: multiple input formats specified for first LTS; can only use one\n";
        }
  
        tool_options.format_for_first = mcrl2::lts::detail::parse_format(parser.option_argument("in1"));
  
        if (tool_options.format_for_first == lts_none) {
          std::cerr << "warning: format '" << parser.option_argument("in1") <<
                       "' is not recognised; option ignored" << std::endl;
        }
      }
      else if (!tool_options.name_for_first.empty()) {
        tool_options.format_for_first = mcrl2::lts::detail::guess_format(tool_options.name_for_first);
      } else {
        gsWarningMsg("cannot detect format from stdin and no input format specified; assuming aut format\n");
        tool_options.format_for_first = lts_aut;
      }
      if (parser.options.count("in2")) {
        if (1 < parser.options.count("in2")) {
          std::cerr << "warning: multiple input formats specified for second LTS; can only use one\n";
        }
  
        tool_options.format_for_second = mcrl2::lts::detail::parse_format(parser.option_argument("in2"));
  
        if (tool_options.format_for_second == lts_none) {
          std::cerr << "warning: format '" << parser.option_argument("in2") <<
                       "' is not recognised; option ignored" << std::endl;
        }
      }
      else {
        tool_options.format_for_second = mcrl2::lts::detail::guess_format(tool_options.name_for_second);
      }
    }

#ifdef ENABLE_SQUADT_CONNECTIVITY
  public:
    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration&);
#endif
};

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/mcrl2_squadt_interface.h>

static const char* lts_file_for_input  = "lts_in";  ///< file containing an LTS that can be imported using the LTS library
static const char* lts_file_auxiliary  = "lts_aux"; ///< LTS for comparison

static const char* option_tau_actions  = "tau_actions";                           ///< the actions that should be recognised as tau
static const char* option_equivalence_class = "add_equivalence_class";    ///< adds equivalence class to the state information of a state instead of actually reducing modulo bisimulation [mCRL2 specific]
static const char* option_preorder_class = "add_preorder_class";    ///< adds preorder equivalence class to the state information of a state instead of actually reducing modulo bisimulation [mCRL2 specific]

/* static const char* option_selected_transformation            = "selected_transformation";               ///< the selected transformation method
static const char* option_selected_output_format             = "selected_output_format";                ///< the selected output format
static const char* option_no_reachability_check              = "no_reachability_check";                 ///< do not check reachability of input LTS
static const char* option_no_state_information               = "no_state_information";                  ///< dot format output specific option to not save state information
static const char* option_tau_actions                        = "tau_actions";                           ///< the actions that should be recognised as tau
static const char* option_add_bisimulation_equivalence_class = "add_bisimulation_equivalence_class";    ///< adds bisimulation equivalence class to the state information of a state instead of actually reducing modulo bisimulation [mCRL2 specific]
*/

void ltscompare_tool::set_capabilities(tipi::tool::capabilities& c) const {
  std::set< lts_type > const& input_formats(mcrl2::lts::detail::supported_lts_formats());

  for (std::set< lts_type >::const_iterator i = input_formats.begin(); i != input_formats.end(); ++i)
  {
    c.add_input_configuration(lts_file_for_input, tipi::mime_type(mcrl2::lts::detail::mime_type_for_type(*i)), tipi::tool::category::reporting);
  }
}

void ltscompare_tool::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  /* Create display */
  tipi::tool_display d;

  layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0, 5, 0, 5));

  /* Helper for format selection */
  mcrl2::utilities::squadt::radio_button_helper < mcrl2::lts::lts_type > format_selector(d);

  file_control& lts_file_field        = d.create< file_control >();

  m.append(d.create< label >()).
    append(d.create< horizontal_box >().
                append(d.create< label >().set_text("LTS file name to compare with : ")).
                append(lts_file_field));
  /* Helper for transformation selection */
  mcrl2::utilities::squadt::radio_button_helper < lts_equivalence > equivalence_selector(d);

  m.append(d.create< label >().set_text("LTS equivalence relation (preorder relation needs to be \"no preorder\"):")).
    append(equivalence_selector.associate(lts_eq_none, "no equivalence",true)).
    append(equivalence_selector.associate(lts_eq_bisim, "strong bisimulation equivalence")).
    append(equivalence_selector.associate(lts_eq_branching_bisim, "branching bisimulation equivalence")).
    append(equivalence_selector.associate(lts_eq_divergence_preserving_branching_bisim, "preserving branching bisimulation equivalence")).
    append(equivalence_selector.associate(lts_eq_sim, "strong simulation equivalence")).
    append(equivalence_selector.associate(lts_eq_trace, "trace equivalence")).
    append(equivalence_selector.associate(lts_eq_weak_trace, "weak trace equivalence"));

/*  mcrl2::utilities::squadt::radio_button_helper < lts_preorder > preorder_selector(d);

  m.append(d.create< label >().set_text("LTS preorder relation (equivalence relation needs to be \"no equivalence\"):")).
    append(preorder_selector.associate(lts_pre_none, "no preorder",true)).
    append(preorder_selector.associate(lts_pre_sim, "strong simulation preorder")).
    append(preorder_selector.associate(lts_pre_trace, "trace preorder")).
    append(preorder_selector.associate(lts_pre_weak_trace, "weak trace preorder")); */

  text_field& tau_field                   = d.create< text_field >();
  m.append(d.create< label >()).
    append(d.create< horizontal_box >().
                append(d.create< label >().set_text("Internal (tau) actions : ")).
                append(tau_field.set_text("tau")));


  button& okay_button = d.create< button >().set_label("OK");

  if (c.option_exists(option_equivalence_class)) {
	  equivalence_selector.set_selection(c.get_option_argument< lts_equivalence >(option_equivalence_class, 0));
  }
  if (c.option_exists(option_tau_actions)) {
    tau_field.set_text(c.get_option_argument< std::string >(option_tau_actions));
  }
/*  if (c.option_exists(option_preorder_class)) {
	  preorder_selector.set_selection(c.get_option_argument< lts_preorder >(option_preorder_class, 0));
  }*/

  send_display_layout(d.manager(m.append(okay_button, layout::top)));

  /* Wait until the ok button was pressed */
  okay_button.await_change();

  if (!lts_file_field.get_text().empty()) {
    c.add_option(lts_file_auxiliary).set_argument_value< 0 >(lts_file_field.get_text());
  }

  if (!tau_field.get_text().empty()) {
    c.add_option(option_tau_actions).set_argument_value< 0 >(tau_field.get_text());
  }

  // Set equivalence class
  c.add_option(option_equivalence_class).set_argument_value< 0 >(equivalence_selector.get_selection());

  //c.add_option(option_preorder_class).set_argument_value< 0 >(preorder_selector.get_selection());


  send_clear_display();
}

bool ltscompare_tool::check_configuration(tipi::configuration const& c) const
{
  bool result = true;

  return (result);
}

bool ltscompare_tool::perform_task(tipi::configuration& c) {
    using namespace mcrl2::lts;
    using namespace tipi;
    using namespace tipi::layout;
    using namespace tipi::layout::elements;

  if (c.input_exists(lts_file_for_input)) {
	tool_options.name_for_first = c.get_input(lts_file_for_input).location();
    tool_options.format_for_first = mcrl2::lts::detail::guess_format(tool_options.name_for_first);
  }

  if (c.input_exists(lts_file_auxiliary)) {
    tool_options.name_for_second = c.get_option_argument< std::string >(lts_file_auxiliary);
    tool_options.format_for_second = mcrl2::lts::detail::guess_format(tool_options.name_for_second);
  }

  if (c.option_exists(option_equivalence_class)) {
    tool_options.equivalence = c.get_option_argument< lts_equivalence >(option_equivalence_class);
  }

  if (c.option_exists(option_preorder_class)) {
    tool_options.preorder = c.get_option_argument< lts_preorder >(option_preorder_class);
  }

  tipi::tool_display d;

  lts l1,l2;

   if ( tool_options.name_for_first.empty() )
   {
	   gsVerboseMsg("reading first LTS from stdin...\n");
     try
     {
        mcrl2::lts::detail::read_from(l1,std::cin, tool_options.format_for_first);
     }
     catch (mcrl2::runtime_error &e)
     {
    	 send_error("cannot read LTS from stdin\nretry with -v/--verbose for more information.\n");
    	 return (false);
     }
   }
   else
   {
     gsVerboseMsg("reading first LTS from '%s'...\n", tool_options.name_for_first.c_str());

     try
     {
        mcrl2::lts::detail::read_from(l1,tool_options.name_for_first, tool_options.format_for_first);
     }
     catch (mcrl2::runtime_error &e)
     {
       send_error("cannot read LTS from file '" + tool_options.name_for_first +
                "'\nretry with -v/--verbose for more information.\n" + e.what());
  	 return (false);
     }
   }

   gsVerboseMsg("reading second LTS from '%s'...\n", tool_options.name_for_second.c_str());
   try
   {
      mcrl2::lts::detail::read_from(l2,tool_options.name_for_second, tool_options.format_for_second);
   }
   catch (mcrl2::runtime_error &e)
   {
     send_error("cannot read LTS from file '" + tool_options.name_for_second +
                  "'\nretry with -v/--verbose for more information.\n" + e.what());
     return (false);
   }

   if (!l1.hide_actions(tool_options.tau_actions))
   { send_error("Cannot hide actions in first transition system");
     return (false);
   }

   if (!l2.hide_actions(tool_options.tau_actions))
   { send_error("Cannot hide actions in second transition system");
     return (false);
   }

   bool result = true;
   if ( tool_options.equivalence != lts_eq_none )
   {
     gsVerboseMsg("comparing LTSs using %s...\n",
         name_of_equivalence(tool_options.equivalence).c_str());

     result = compare(l1,l2,tool_options.equivalence,tool_options.generate_counter_examples);

     gsMessage("LTSs are %s%s\n",
         ((result) ? "" : "not "),
         equivalent_string(tool_options.equivalence));

   }

   if ( tool_options.preorder != lts_pre_none )
   {
     gsVerboseMsg("comparing LTSs using %s...\n",
         name_of_preorder(tool_options.preorder).c_str());

     result = compare(l1,l2,tool_options.preorder);

   }

   return (true);
 }

#endif

class ltscompare_gui_tool: public mcrl2_gui_tool<ltscompare_tool> {
public:
	ltscompare_gui_tool() {

		std::vector<std::string> values;

		m_gui_options["counter-example"] = create_checkbox_widget();

		values.clear();
		values.push_back("bisim");
		values.push_back("branching-bisim");
		values.push_back("dpbranching-bisim");
		values.push_back("sim");
		values.push_back("trace");
		values.push_back("weak-trace");
		m_gui_options["equivalence"] = create_radiobox_widget(values);

		values.clear();
		values.push_back("sim");
		values.push_back("trace");
		values.push_back("weak-trace");
		m_gui_options["preorder"] = create_filepicker_widget();
		m_gui_options["tau"] = create_textctrl_widget();

		//-iFORMAT, --in1=FORMAT   use FORMAT as the format for INFILE1 (or stdin)
		//-jFORMAT, --in2=FORMAT   use FORMAT as the format for INFILE2

	}
};

int main(int argc, char **argv) {
  MCRL2_ATERMPP_INIT(argc, argv)

  return ltscompare_tool().execute(argc,argv);
}
