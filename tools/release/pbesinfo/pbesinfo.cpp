// Author(s): Alexander van Dam, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesinfo.cpp
/// \brief Tool that displays information about a PBES.

#include "mcrl2/pbes/detail/pbes_property_map.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbes_input_tool.h"
#include "mcrl2/utilities/input_tool.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities::tools;
using pbes_system::tools::pbes_input_tool;

class pbesinfo_tool: public pbes_input_tool<input_tool>
{
  protected:
    using super = pbes_input_tool<input_tool>;

    bool opt_full = false;

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      opt_full = parser.options.count("full") > 0;
    }

    void add_options(interface_description& desc) override
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
      )
    {}

    /// If PBES can be loaded from file_name, then
    /// - Show if PBES is closed and if it is well formed
    ///       - Show number of equations
    /// - Show number of mu's / nu's.
    /// - Show which predicate variables have mu's and which predicate variables have nu's
    /// - Show predicate variables and their type
    /// else
    /// - Give error
    bool run() override
    {
      pbes p;
      load_pbes(p, input_filename(), pbes_input_format());

      pbes_system::detail::pbes_property_map info(p);

      // Show file from which PBES was read
      std::cout << input_file_message() << "\n\n";

      // Show number of equations
      std::cout << "Number of equations:     " << p.equations().size() << std::endl;

      // Show number of mu's with the predicate variables from the mu's
      std::cout << "Number of mu's:          " << info["mu_equation_count"] << std::endl;

      // Show number of nu's with the predicate variables from the nu's
      std::cout << "Number of nu's:          " << info["nu_equation_count"] << std::endl;

      // Show number of nu's with the predicate variables from the nu's
      std::cout << "Block nesting depth:     " << info["block_nesting_depth"] << std::endl;

      // Show if PBES is closed and well formed
      std::cout << "The PBES is closed:      " << std::flush;
      std::cout << (p.is_closed() ? "yes" : "no ") << std::endl;
      std::cout << "The PBES is well formed: " << std::flush;
      std::cout << (p.is_well_typed() ? "yes" : "no ") << std::endl;

      // Show binding variables with their signature
      if (opt_full)
      {
        std::cout << "Predicate variables:\n";
        for (std::vector<pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          std::cout << core::pp(i->symbol()) << "." << pp(i->variable()) << std::endl;
        }
      }
      return true;
    }
};

int main(int argc, char** argv)
{
  return pbesinfo_tool().execute(argc, argv);
}
