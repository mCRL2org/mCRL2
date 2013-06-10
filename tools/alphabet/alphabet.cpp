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

#include "boost.hpp" // precompiled headers

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
void complete_process_specification_old(process_specification& x, bool alpha_reduce = false)
{
  type_check(x);
  if (alpha_reduce)
  {
    alphabet_reduction reduce;
    reduce(x);
  }
  process::translate_user_notation(x);
  process::normalize_sorts(x, x.data());
}

inline
process_specification parse_process_specification_old(std::istream& in, bool alpha_reduce = false)
{
  std::string text = utilities::read_text(in);
  process_specification result = parse_process_specification_new(text);
  complete_process_specification_old(result, alpha_reduce);
  return result;
}

inline
process_specification parse_process_specification_old(const std::string& spec_string, const bool alpha_reduce=false)
{
  std::istringstream in(spec_string);
  return parse_process_specification_old(in, alpha_reduce);
}

inline
void alphabet(const std::string& input_filename, const std::string& output_filename, bool use_new_implementation)
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

  if (use_new_implementation)
  {
    result = process::parse_process_specification(text, false);
    process::alphabet_reduce(result);
  }
  else
  {
    result = parse_process_specification_old(text, true);
  }

  std::ofstream out(output_filename.c_str());
  out << process::pp(result);
}

class alphabet_tool : public input_output_tool
{
  typedef input_output_tool super;

  protected:
    bool m_use_new_implementation;

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      m_use_new_implementation = parser.options.count("use-new-implementation") > 0;
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("use-new-implementation", "use the new implementation", 'n');
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
      alphabet(input_filename(), output_filename(), m_use_new_implementation);
      return true;
    }
};

int main(int argc, char** argv)
{
  return alphabet_tool().execute(argc, argv);
}
