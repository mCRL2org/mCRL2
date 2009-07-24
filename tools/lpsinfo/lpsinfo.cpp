// Author(s): Frank Stappers, Wieger Wesselink
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
#define AUTHOR "Frank Stappers, Wieger Wesselink"

#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/specification_property_map.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using namespace mcrl2::utilities::tools;

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
      specification spec;
      spec.load(input_filename());
      lps::detail::specification_property_map info(spec);
      std::cout << "Input read from " << ((m_input_filename.empty())?"stdin":("'" + m_input_filename + "'")) << "\n\n";
      std::cout << info.info();
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

      specification spec;
      spec.load(c.get_input(m_input_filename).location());
      lps::detail::specification_property_map info(spec);

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
                append(d.create< label >().set_text("Multi-actions: (#):"))
               );

      /* Second column */
      m.append(d.create< vertical_box >().set_default_alignment(layout::left).
                append(d.create< label >().set_text(c.get_input(m_input_filename).location())).
                append(d.create< label >().set_text(info["summand_count"               ])).
                append(d.create< label >().set_text(info["tau_summand_count"           ])).
                append(d.create< label >().set_text(info["declared_free_variable_count"])).
                append(d.create< label >().set_text(info["process_parameter_count"     ])).
                append(d.create< label >().set_text(info["declared_action_label_count" ])).
                append(d.create< label >().set_text(info["used_action_label_count"     ])).
                append(d.create< label >().set_text(info["used_multi_action_count"     ]))
               );

      send_display_layout(d.manager(m));

      return true;
    }
#endif
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return lpsinfo_tool().execute(argc, argv);
}
