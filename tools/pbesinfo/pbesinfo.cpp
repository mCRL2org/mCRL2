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

using namespace std;
using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities::tools;

class pbesinfo_tool: public input_tool
{
  protected:
    typedef input_tool super;

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

      // Get PBES equations. Makes a lot of function calls more readable.
      const atermpp::vector<pbes_equation>& eqsys = p.equations();

      // Show file from which PBES was read
      std::cout << input_file_message() << "\n\n";

      // Show if PBES is closed and well formed
      std::cout << "The PBES is " << (p.is_closed() ? "" : "not ") << "closed and " << (p.is_well_typed() ? "" : "not ") << "well formed" << std::endl;

      // Show number of equations
      std::cout << "Number of equations: " << eqsys.size() << std::endl;

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
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return pbesinfo_tool().execute(argc, argv);
}
