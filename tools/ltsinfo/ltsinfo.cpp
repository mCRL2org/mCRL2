// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsinfo.cpp

#define NAME "ltsinfo"
#define AUTHOR "Muck van Weerdenburg"

#include <string>

#include <boost/lexical_cast.hpp>

#include "aterm2.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/exception.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/squadt_tool.h"

using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;

//Temporary workaround for the passing of the determinism

static const std::set<mcrl2::lts::lts_equivalence> &initialise_allowed_eqs()
{
  using namespace mcrl2::lts;
  static std::set<lts_equivalence> s;
  s.insert(lts_eq_bisim);
  s.insert(lts_eq_branching_bisim);
  s.insert(lts_eq_divergence_preserving_branching_bisim);
  s.insert(lts_eq_isomorph);
  return s;
}
static const std::set<mcrl2::lts::lts_equivalence> &allowed_eqs()
{
  static const std::set<mcrl2::lts::lts_equivalence> &s = initialise_allowed_eqs();
  return s;
}

typedef squadt_tool< input_tool > ltsinfo_base;
class ltsinfo_tool : public ltsinfo_base
{

  private:

    std::string                 infilename;
    mcrl2::lts::lts_type        intype;
    mcrl2::lts::lts_equivalence determinism_equivalence;

  public:

    ltsinfo_tool() :
      ltsinfo_base(NAME,AUTHOR,
        "display basic information about an LTS",
        "Print information about the labelled transition system (LTS) in INFILE. "
        "If INFILE is not supplied, stdin is used.\n"
        "\n"
        "The format of INFILE is determined by its contents. "
        "The option --in can be used to force the format for INFILE. "
        "The supported formats are:\n"
        +mcrl2::lts::lts::supported_lts_formats_text()
      ),
      intype(mcrl2::lts::lts_none),
      determinism_equivalence(mcrl2::lts::lts_eq_isomorph)
    {
    }

  private:

    void add_options(interface_description &desc)
    {
      using namespace mcrl2::lts;

      ltsinfo_base::add_options(desc);

      desc.
        add_option("equivalence", make_mandatory_argument("NAME"),
          "use equivalence NAME for deterministic check:\n"
          "  '" + lts::string_for_equivalence(lts_eq_isomorph) + "' for "
                + lts::name_of_equivalence(lts_eq_isomorph) + " (default),\n"
          "  '" + lts::string_for_equivalence(lts_eq_bisim) + "' for "
                + lts::name_of_equivalence(lts_eq_bisim) + ",\n"
          "  '" + lts::string_for_equivalence(lts_eq_branching_bisim) + "' for "
                + lts::name_of_equivalence(lts_eq_branching_bisim) + ", or\n"
          "  '" + lts::string_for_equivalence(lts_eq_divergence_preserving_branching_bisim) + "' for "
                + lts::name_of_equivalence(lts_eq_divergence_preserving_branching_bisim) + ", or\n"
          "  'none' for not performing the check at all",
          'e').
        add_option("in", make_mandatory_argument("FORMAT"),
          "use FORMAT as the input format", 'i');
    }

    void parse_options(const command_line_parser &parser)
    {
      using namespace mcrl2::lts;

      ltsinfo_base::parse_options(parser);

      if (parser.options.count("equivalence")) {
        determinism_equivalence = lts::parse_equivalence(parser.option_argument("equivalence"));
        if (allowed_eqs().count(determinism_equivalence) == 0 &&
            parser.option_argument("equivalence") != "none")
        {
          parser.error("option -e/--equivalence has illegal argument '" +
              parser.option_argument("equivalence") + "'");
        }
      }

      if (0 < parser.arguments.size()) {
        infilename = parser.arguments[0];
      }
      if (1 < parser.arguments.size()) {
        parser.error("too many file arguments");
      }

      if (parser.options.count("in")) {
        if (1 < parser.options.count("in")) {
          parser.error("multiple input formats specified; can only use one");
        }

        intype = lts::parse_format(parser.option_argument("in"));
        if (intype == lts_none || intype == lts_dot)  {
          parser.error("option -i/--in has illegal argument '" +
            parser.option_argument("in") + "'");
        }
      }
    }

  public:

    bool run() {
      using namespace mcrl2::lts;

      mcrl2::lts::lts l;

      if (infilename.empty()) {
        gsVerboseMsg("reading LTS from stdin...\n");

        if ( !l.read_from(std::cin, intype) ) {
          throw mcrl2::runtime_error("cannot read LTS from stdin\nretry with -v/--verbose for more information");
        }
      }
      else {
        gsVerboseMsg("reading LTS from '%s'...\n",infilename.c_str());

        if (!l.read_from(infilename,intype)) {
          throw mcrl2::runtime_error("cannot read LTS from file '" + infilename +
                                             "'\nretry with -v/--verbose for more information");
        }
      }

      std::cout << "LTS format: " << lts::string_for_type(l.get_type()) << std::endl
           << "Number of states: " << l.num_states() << std::endl
           << "Number of labels: " << l.num_labels() << std::endl
           << "Number of transitions: " << l.num_transitions() << std::endl;

      if ( l.has_state_info() )
      {
        std::cout << "Has state information." << std::endl;
      } else {
        std::cout << "Does not have state information." << std::endl;
      }
      if ( l.has_label_info() )
      {
        std::cout << "Has label information." << std::endl;
      } else {
        std::cout << "Does not have label information." << std::endl;
      }
      if ( l.has_creator() )
      {
        std::cout << "Created by: " << l.get_creator() << std::endl;
      }
      gsVerboseMsg("checking reachability...\n");
      if ( !l.reachability_check() )
      {
        std::cout << "Warning: some states are not reachable from the initial state! (This might result in unspecified behaviour of LTS tools.)" << std::endl;
      }
      if ( determinism_equivalence != lts_eq_none )
      {
        gsVerboseMsg("checking whether LTS is deterministic (modulo %s)...\n",lts::name_of_equivalence(determinism_equivalence).c_str());
        gsVerboseMsg("minimisation...\n");

        l.reduce(determinism_equivalence);
        gsVerboseMsg("deterministic check...\n");
        if ( l.is_deterministic() )
        {
          std::cout << "LTS is deterministic (modulo " << lts::name_of_equivalence(determinism_equivalence) << ")" << std::endl;
        } else {
          std::cout << "LTS is not deterministic (modulo " << lts::name_of_equivalence(determinism_equivalence) << ")" << std::endl;
        }
      }

      return true;
    }

#ifdef ENABLE_SQUADT_CONNECTIVITY
    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& c) const {
      std::set< mcrl2::lts::lts_type > const& input_formats(mcrl2::lts::lts::supported_lts_formats());

      for (std::set< mcrl2::lts::lts_type >::const_iterator i = input_formats.begin(); i != input_formats.end(); ++i) {
        c.add_input_configuration("lts_in", tipi::mime_type(mcrl2::lts::lts::mime_type_for_type(*i)), tipi::tool::category::reporting);
      }
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration& c) {
      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;

      std::string infilename = c.get_input("lts_in").location();

      /* Create display */
      tipi::tool_display d;

      // Helper for linearisation method selection
      mcrl2::utilities::squadt::radio_button_helper< mcrl2::lts::lts_equivalence > determinism_selector(d);

      layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0,5,0,5));

      m.append(d.create< label >().set_text("Deterministic check:")).
        append(d.create< horizontal_box >().
            append(determinism_selector.associate(mcrl2::lts::lts_eq_none, "None")).
            append(determinism_selector.associate(mcrl2::lts::lts_eq_isomorph, "Isomorphism", true)).
            append(determinism_selector.associate(mcrl2::lts::lts_eq_bisim, "Strong bisimilarity")).
            append(determinism_selector.associate(mcrl2::lts::lts_eq_branching_bisim, "Branching bisimilarity")).
            append(determinism_selector.associate(mcrl2::lts::lts_eq_divergence_preserving_branching_bisim, "Divergence preserving branching bisimilarity")));

      // Add okay button
      button& okay_button = d.create< button >().set_label("OK");

      m.append(d.create< label >().set_text(" ")).
        append(okay_button, layout::right);

      // Set default values for options if the configuration specifies them
      if (c.option_exists("determinism_equivalence")) {
        determinism_selector.set_selection(
            c.get_option_argument< mcrl2::lts::lts_equivalence >("determinism_equivalence", 0));
      }

      // Display
      send_display_layout(d.manager(m));

      /* Wait for the OK button to be pressed */
      okay_button.await_change();

      c.add_option("determinism_equivalence"). set_argument_value< 0 >(determinism_selector.get_selection());

      send_clear_display();
    }

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& c) const {
      return c.input_exists("lts_in") || c.option_exists("determinism_equivalence");
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration& c) {
      using mcrl2::lts::lts;
      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;

      tipi::configuration::object& input_object = c.get_input("lts_in");

      lts l;
      mcrl2::lts::lts_type t = lts::parse_format(input_object.type().sub_type());

      // Extract configuration
      determinism_equivalence = c.get_option_argument< mcrl2::lts::lts_equivalence >("determinism_equivalence", 0);

      if (l.read_from(input_object.location(), t)) {
        /* Create and add the top layout manager */
        tipi::tool_display d;

        layout::horizontal_box& m = d.create< horizontal_box >();

        std::string deterministic("-");

        if(determinism_equivalence != mcrl2::lts::lts_eq_none) {
          l.reduce(determinism_equivalence);

          deterministic = std::string(l.is_deterministic() ? "yes" : "no") + ", modulo " +
                                 lts::name_of_equivalence(determinism_equivalence);
        }

        m.append(d.create< vertical_box >().set_default_alignment(layout::left).
                append(d.create< label >().set_text("States (#):")).
                append(d.create< label >().set_text("Labels (#):")).
                append(d.create< label >().set_text("Transitions (#):")).
                append(d.create< label >().set_text("")).
                append(d.create< label >().set_text("State information:")).
                append(d.create< label >().set_text("Label information:")).
                append(d.create< label >().set_text("Deterministic:")).
                append(d.create< label >().set_text("")).
                append(d.create< label >().set_text("Created by:")),
             margins(0,5,0,5));

        /* Second column */
        m.append(d.create< vertical_box >().set_default_alignment(layout::right).
                append(d.create< label >().set_text(boost::lexical_cast < std::string > (l.num_states()))).
                append(d.create< label >().set_text(boost::lexical_cast < std::string > (l.num_labels()))).
                append(d.create< label >().set_text(boost::lexical_cast < std::string > (l.num_transitions()))).
                append(d.create< label >().set_text("")).
                append(d.create< label >().set_text(l.has_state_info() ? "present" : "not present")).
                append(d.create< label >().set_text(l.has_label_info() ? "present" : "not present")).
                append(d.create< label >().set_text(deterministic)).
                append(d.create< label >().set_text("")).
                append(d.create< label >().set_text(l.get_creator())),
             margins(0,5,0,5));

        layout::vertical_box& n = d.create< vertical_box >();

        n.append(m).
            append(d.create< label >().
                 set_text("Input read from " + input_object.location() + " (" + lts::string_for_type(t) + " format)"),
                        margins(5,0,5,20));

        gsVerboseMsg("checking reachability...\n");
        if (!l.reachability_check()) {
            n.append(d.create< label >().set_text("Warning: some states are not reachable from the initial state!")).
              append(d.create< label >().set_text("(This might result in unspecified behaviour of LTS tools.)"));
        }

        send_display_layout(d.manager(n));
      }
      else {
        send_error("Could not read `" + c.get_input("lts_in").location() + "', corruption or incorrect format?\n");

        return (false);
      }

      return (true);
    }
#endif
};


int main(int argc, char **argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return ltsinfo_tool().execute(argc, argv);
}
