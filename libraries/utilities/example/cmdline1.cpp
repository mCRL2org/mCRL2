// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file cmdline1.cpp
/// \brief CLI example

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using utilities::command_line_parser;
using utilities::interface_description;
using utilities::make_optional_argument;
using utilities::tools::tool;

/// The pbesinst tool.
class my_tool: public tool
{
  protected:
    typedef tool super;

    std::string synopsis() const
    {
      return "[OPTION]... [FIRST]\n";
    }

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      std::string s = parser.option_argument("option1");
      std::cout << "option o: " << s << std::endl;

      int i = parser.option_argument_as<int>("option2");
      std::cout << "option i: " << i << std::endl;

      bool b = parser.option_argument_as<bool>("option3");
      std::cout << "option b: " << std::boolalpha << b << std::endl;

      bool a = parser.options.count("option4") > 0;
      std::cout << "option a: " << std::boolalpha << a << std::endl;

      if (0 < parser.arguments.size())
      {
        std::string s = parser.arguments[0];
        std::cout << "positional option 1: " << s << std::endl;
      }

      std::cout << "--- parser arguments ---" << std::endl;
      for (const auto & option : parser.options)
      {
        std::cout << option.first << " -> " << option.second << std::endl;
      }
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc
      /// string option with default value 'name1'
      .add_option("option1",
                  make_optional_argument("NAME", "name1"),
                  "string option NAME:\n"
                  "  'name1' (default),\n"
                  "  'name2', or\n"
                  "  'name3'.",
                  'o')
      /// integer option with default value 1
      .add_option("option2",
                  make_optional_argument("NAME", "1"),
                  "integer option",
                  'i')
      /// boolean option with default value true
      .add_option("option3",
                  make_optional_argument("NAME", "1"),
                  "boolean option",
                  'b')
      /// boolean flag (default off)
      .add_option("option4",
                  "boolean option",
                  'a')
      ;
    }

  public:
    /// Constructor.
    my_tool()
      : super(
        "My tool",
        "John Doe",
        "One line description",
        "First line of the long description. "
        "Second line of the long description."
      )
    {}

    /// Runs the tool.
    bool run()
    {
      return true;
    }
};

int main(int argc, char* argv[])
{
  return my_tool().execute(argc, argv);
}
