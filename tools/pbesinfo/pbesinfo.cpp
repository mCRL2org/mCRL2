// Author(s): Alexander van Dam, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesinfo.cpp
/// \brief Tool that displays information about a PBES.

// ======================================================================
//
// file          : pbesinfo
// date          : 11-04-2007
// version       : 0.1.0
//
// author(s)     : Alexander van Dam <avandam@damdonk.nl>
//
// ======================================================================

//C++
#include <cstdio>
#include <exception>
#include <iostream>
#include <string>

//MCRL2-specific
#include "mcrl2/core/messaging.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/detail/pbes_property_map.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/squadt_tool.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities::tools;

class pbesinfo_tool: public squadt_tool<input_tool>
{
  protected:
    typedef squadt_tool<input_tool> super;

    bool opt_full;

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      opt_full = parser.options.count("full") > 0;
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("full",
                      "display the predicate variables and their signature",
                      'f'
                     )
      ;
    }

  public:
    pbesinfo_tool()
      : super(
          "pbesinfo",
          "Wieger Wesselink; Alexander van Dam",
          "display basic information about a PBES",
          super::make_tool_description("Print basic information about the PBES in INFILE.")
        ),
        opt_full(false)
     {}

    /// If PBES can be loaded from file_name, then
    /// - Show if PBES is closed and if it is well formed
    ///       - Show number of equations
    /// - Show number of mu's / nu's.
    /// - Show which predicate variables have mu's and which predicate variables have nu's
    /// - Show predicate variables and their type
    /// else
    /// - Give error
    bool run()
    {
      pbes<> p;
      p.load(input_filename());

      pbes_system::detail::pbes_property_map info(p);

      // Show file from which PBES was read
      std::cout << input_file_message() << "\n\n";

      // Show if PBES is closed and well formed
      std::cout << "The PBES is " << (p.is_closed() ? "" : "not ") << "closed and " << (p.is_well_typed() ? "" : "not ") << "well formed" << std::endl;

      // Show number of equations
      std::cout << "Number of equations: " << p.equations().size() << std::endl;

      // Show number of mu's with the predicate variables from the mu's
      std::cout << "Number of mu's:      " << info["mu_equation_count"] << std::endl;

      // Show number of nu's with the predicate variables from the nu's
      std::cout << "Number of nu's:      " << info["nu_equation_count"] << std::endl;

      // Show number of nu's with the predicate variables from the nu's
      std::cout << "Block nesting depth: " << info["block_nesting_depth"] << std::endl;

      // Show binding variables with their signature
      if (opt_full)
      {
        std::cout << "Predicate variables:\n";
        for (atermpp::vector<pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          std::cout << core::pp(i->symbol()) << "." << core::pp(i->variable()) << std::endl;
        }
      }
      return true;
    }

#ifdef ENABLE_SQUADT_CONNECTIVITY
    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& c) const
    { c.add_input_configuration("main-input", tipi::mime_type("pbes", tipi::mime_type::application), tipi::tool::category::reporting);
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration&)
    {}

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& c) const
    {
      return c.input_exists("main-input");
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration& c)
    {
      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;

      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(c);

      pbes<> p;
      p.load(c.get_input("main-input").location());
      pbes_system::detail::pbes_property_map info(p);

      /* Create display */
      tipi::tool_display d;

      layout::horizontal_box& m = d.create< horizontal_box >().set_default_margins(margins(0, 5, 0, 5));

      /* First column */
      m.append(d.create< vertical_box >().set_default_alignment(layout::left).
                append(d.create< label >().set_text("Input read from:")).
                append(d.create< label >().set_text("Closed:")).
                append(d.create< label >().set_text("Number of equations:")).
                append(d.create< label >().set_text("Number of mu's:")).
                append(d.create< label >().set_text("Number of nu's:")).
                append(d.create< label >().set_text("Block nesting depth:"))
               );

      /* Second column */
      m.append(d.create< vertical_box >().set_default_alignment(layout::left).
                append(d.create< label >().set_text(c.get_input("main-input").location())).
                append(d.create< label >().set_text(p.is_closed() ? "yes" : "no")).
                append(d.create< label >().set_text(info["equation_count"])).
                append(d.create< label >().set_text(info["equation_count"])).
                append(d.create< label >().set_text(info["mu_equation_count"])).
                append(d.create< label >().set_text(info["nu_equation_count"])).
                append(d.create< label >().set_text(info["block_nesting_depth"]))
               );

      send_display_layout(d.manager(m));

      return true;
    }
#endif
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return pbesinfo_tool().execute(argc, argv);
}
