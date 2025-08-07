// Author(s): Jan Friso Groote. Based on a presinfo.cpp by Alexander van Dam, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file presinfo.cpp
/// \brief Tool that displays information about a PRES.

#include "mcrl2/pres/detail/pres_property_map.h"
#include "mcrl2/pres/io.h"
#include "mcrl2/pres/pres_input_tool.h"
#include "mcrl2/utilities/input_tool.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::pres_system;
using namespace mcrl2::utilities::tools;
using pres_system::tools::pres_input_tool;

class presinfo_tool: public pres_input_tool<input_tool>
{
  protected:
    using super = pres_input_tool<input_tool>;

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
    presinfo_tool()
      : super(
        "presinfo",
        "Jan Friso Groote",
        "display basic information about a PRES",
        super::make_tool_description("Print basic information about the PRES in INFILE.")
      )
    {}

    /// If PRES can be loaded from file_name, then
    /// - Show if PRES is closed and if it is well formed
    ///       - Show number of equations
    /// - Show number of mu's / nu's.
    /// - Show which predicate variables have mu's and which predicate variables have nu's
    /// - Show predicate variables and their type
    /// else
    /// - Give error
    bool run() override
    {
      pres p;
      load_pres(p, input_filename(), pres_input_format());

      pres_system::detail::pres_property_map info(p);

      // Show file from which PRES was read
      std::cout << input_file_message() << "\n\n";

      // Show number of equations
      std::cout << "Number of equations:     " << p.equations().size() << std::endl;

      // Show number of mu's with the predicate variables from the mu's
      std::cout << "Number of mu's:          " << info["mu_equation_count"] << std::endl;

      // Show number of nu's with the predicate variables from the nu's
      std::cout << "Number of nu's:          " << info["nu_equation_count"] << std::endl;

      // Show number of nu's with the predicate variables from the nu's
      std::cout << "Block nesting depth:     " << info["block_nesting_depth"] << std::endl;

      // Show if PRES is closed and well formed
      std::cout << "The PRES is closed:      " << std::flush;
      std::cout << (p.is_closed() ? "yes" : "no ") << std::endl;
      std::cout << "The PRES is well formed: " << std::flush;
      std::cout << (p.is_well_typed() ? "yes" : "no ") << std::endl;

      // Show binding variables with their signature
      if (opt_full)
      {
        std::cout << "Predicate variables:\n";
        for (std::vector<pres_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          std::cout << core::pp(i->symbol()) << "." << pres_system::pp(i->variable()) << std::endl;
        }
      }
      return true;
    }
};

int main(int argc, char** argv)
{
  return presinfo_tool().execute(argc, argv);
}
