// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file alphabet.cpp
/// \brief This tool reads a process specification,
/// and applies alphabet reduction to it.

#define TOOLNAME "alphabet"
#define AUTHOR "Wieger Wesselink"

#include <fstream>
#include <iostream>
#include <string>
#include "mcrl2/lps/tools.h"
#include "mcrl2/process/alphabet.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::process;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

inline
void alphabet(const std::string& input_filename, const std::string& output_filename, bool remove_duplicate_equations)
{
  std::string text;
  if (input_filename.empty())
  {
    text = utilities::read_text(std::cin);
  }
  else
  {
    text = utilities::read_text(input_filename);
  }

  process::process_specification result;
  result = process::parse_process_specification(text, false);
  if (remove_duplicate_equations)
  {
    process::alphabet_reduce(result);
  }
  else
  {
    process::alphabet_reduce(result, 0);
  }

  std::ofstream out(output_filename.c_str());
  out << process::pp(result);
}

class alphabet_tool : public input_output_tool
{
  typedef input_output_tool super;

  bool remove_duplicate_equations;

  protected:
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      remove_duplicate_equations = 0 < parser.options.count("remove-duplicate-equations");
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("remove-duplicate-equations", "remove duplicate equations after alphabet reduction", 'r');
    }

  public:
    alphabet_tool() : super(
        TOOLNAME,
        AUTHOR,
        "applies alphabet reduction to a process specification",
        "Applies alphabet reduction to the mCRL2 specification in INFILE and writes the "
        "result to OUTFILE.")
    {}

    bool run()
    {
      mCRL2log(log::verbose) << "alphabet parameters:" << std::endl;
      mCRL2log(log::verbose) << "  input file:                       " << m_input_filename << std::endl;
      mCRL2log(log::verbose) << "  output file:                      " << m_output_filename << std::endl;
      mCRL2log(log::verbose) << "  remove-duplicate-equations:       " << std::boolalpha << remove_duplicate_equations << std::endl;
      alphabet(input_filename(), output_filename(), remove_duplicate_equations);
      return true;
    }
};

int main(int argc, char** argv)
{
  return alphabet_tool().execute(argc, argv);
}
