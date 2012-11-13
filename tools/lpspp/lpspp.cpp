// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpspp.cpp

#include "boost.hpp" // precompiled headers

#define NAME "lpspp"
#define AUTHOR "Aad Mathijssen and Jeroen Keiren"

#include <string>
#include <iostream>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/lps/tools.h"

using namespace mcrl2::log;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2;

class lpspp_tool: public input_output_tool
{
  private:
    typedef input_output_tool super;

  public:
    lpspp_tool()
      : super(NAME, AUTHOR,
              "pretty print an LPS",
              "Print the mCRL2 LPS in INFILE to OUTFILE in a human readable format. If OUTFILE "
              "is not present, stdout is used. If INFILE is not present, stdin is used."
             ),
      m_format(print_default)
    {}

    bool run()
    {
      lps::lpspp(input_filename(),
                 output_filename(),
                 m_print_summand_numbers,
                 m_format
                );
      return true;
    }

  protected:
    print_format_type m_format;
    bool m_print_summand_numbers;

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("format", make_enum_argument<print_format_type>("FORMAT")
                      .add_value_desc(print_default, "for a process specification", true)
                      .add_value_desc(print_internal, "for a textual ATerm representation of the internal format"),
                      "print the LPS in the specified FORMAT:", 'f');
      desc.add_option("print-summand_numbers", "print numbers in front of summands", 'n');
    }

    void parse_options(const command_line_parser& parser)
    {
      input_output_tool::parse_options(parser);
      m_format = parser.option_argument_as<print_format_type>("format");
      m_print_summand_numbers = parser.options.count("print-summand_numbers") > 0;
    }
};

int main(int argc, char* argv[])
{
  return lpspp_tool().execute(argc, argv);
}
