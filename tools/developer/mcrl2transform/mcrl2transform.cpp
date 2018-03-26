// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2transform.cpp

#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/process/alphabet_reduce.h"
#include "mcrl2/process/anonymize.h"
#include "mcrl2/process/detail/process_command.h"
#include "mcrl2/process/eliminate_single_usage_equations.h"
#include "mcrl2/process/eliminate_trivial_equations.h"
#include "mcrl2/process/eliminate_unused_equations.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/utilities/detail/io.h"
#include "mcrl2/utilities/detail/separate_keyword_section.h"
#include "mcrl2/utilities/detail/transform_tool.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using data::tools::rewriter_tool;
using utilities::detail::transform_tool;
using utilities::tools::input_output_tool;

/// \brief Eliminates trivial process equations of the shape P = Q
struct eliminate_trivial_equations_command: public process::detail::process_command
{
  eliminate_trivial_equations_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process::detail::process_command("eliminate-trivial-equations", input_filename, output_filename, options)
  {}

  void execute()
  {
    process::detail::process_command::execute();
    process::eliminate_trivial_equations(procspec);
    utilities::detail::write_text(output_filename, process::pp(procspec));
  }
};

/// \brief Applies alphabet reduction to a process
struct alphabet_reduce_command: public process::detail::process_command
{
  alphabet_reduce_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process::detail::process_command("alphabet-reduce", input_filename, output_filename, options)
  {}

  void execute()
  {
    process::detail::process_command::execute();
    process::alphabet_reduce(procspec, 0);
    utilities::detail::write_text(output_filename, process::pp(procspec));
  }
};

/// \brief Eliminates equations that are used in a single place
struct eliminate_single_usage_equations_command: public process::detail::process_command
{
  eliminate_single_usage_equations_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process::detail::process_command("eliminate-single-usage-equations", input_filename, output_filename, options)
  {}

  void execute()
  {
    process::detail::process_command::execute();
    process::eliminate_single_usage_equations(procspec);
    utilities::detail::write_text(output_filename, process::pp(procspec));
  }
};

/// \brief Eliminates unused equations
struct eliminate_unused_equations_command: public process::detail::process_command
{
  eliminate_unused_equations_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process::detail::process_command("eliminate-unused-equations", input_filename, output_filename, options)
  {}

  void execute()
  {
    process::detail::process_command::execute();
    process::eliminate_unused_equations(procspec.equations(), procspec.init());
    utilities::detail::write_text(output_filename, process::pp(procspec));
  }
};

/// \brief Anonimizes the identifiers of a process specification
struct anonymize_process_command: public process::detail::process_command
{
  anonymize_process_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process::detail::process_command("anonymize", input_filename, output_filename, options)
  {}

  void execute()
  {
    process::detail::process_command::execute();
    process::anonymize(procspec);
    utilities::detail::write_text(output_filename, process::pp(procspec));
  }
};

struct separate_equations_command: public process::detail::process_command
{
  separate_equations_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process::detail::process_command("separate-equations", input_filename, output_filename, options)
  {}

  void execute()
  {
    std::string text = utilities::detail::read_text(input_filename);
    std::vector<std::string> all_keywords = { "sort", "var", "map", "cons", "proc", "init", "act" };
    bool repeat_keyword = true;
    std::pair<std::string, std::string> q;

    q = utilities::detail::separate_keyword_section(text, "var", all_keywords, repeat_keyword);
    std::string var_text = q.first;
    text = q.second;

    // N.B. Don't forget isolated 'eqn' sections without a preceding 'var'.
    all_keywords.push_back("eqn");
    q = utilities::detail::separate_keyword_section(text, "eqn", all_keywords, repeat_keyword);
    var_text = var_text + '\n' + q.first;
    text = q.second;

    q = utilities::detail::separate_keyword_section(text, "proc", all_keywords, repeat_keyword);
    std::string proc_text = q.first;
    text = q.second;

    q = utilities::detail::separate_keyword_section(text, "init", all_keywords, repeat_keyword);
    std::string init_text = q.first;
    text = q.second;

    q = utilities::detail::separate_keyword_section(text, "act", all_keywords, repeat_keyword);
    std::string act_text = q.first;
    text = q.second;

    all_keywords.push_back("eqn");
    q = utilities::detail::separate_keyword_section(text, "eqn", all_keywords, repeat_keyword);
    std::string eqn_text = q.first;
    std::string sort_text = q.second;

    text = "%%%--- dataspec ---%%%\n" + sort_text + "\n"
         + "%%%--- eqnspec ---%%%\n" + var_text + "\n" + eqn_text + "\n"
         + "%%%--- actspec ---%%%\n" + act_text + "\n"
         + "%%%--- procspec ---%%%\n" + proc_text + "\n"
         + "%%%--- initspec ---%%%\n" + init_text;
    utilities::detail::write_text(output_filename, text);
  }
};

class mcrl2transform_tool: public transform_tool<rewriter_tool<input_output_tool>>
{
  typedef transform_tool<rewriter_tool<input_output_tool>> super;

  public:
    mcrl2transform_tool()
      : super("mcrl2transform",
              "Wieger Wesselink",
              "applies a transformation to an mCRL2 process specification",
              "Transform the object in INFILE and write the result to OUTFILE. If OUTFILE "
              "is not present, stdout is used. If INFILE is not present, stdin is used."
             )
    {}

    void add_commands(const std::vector<std::string>& options) override
    {
      add_command(std::make_shared<eliminate_trivial_equations_command>(input_filename(), output_filename(), options));
      add_command(std::make_shared<alphabet_reduce_command>(input_filename(), output_filename(), options));
      add_command(std::make_shared<eliminate_single_usage_equations_command>(input_filename(), output_filename(), options));
      add_command(std::make_shared<eliminate_unused_equations_command>(input_filename(), output_filename(), options));
      add_command(std::make_shared<anonymize_process_command>(input_filename(), output_filename(), options));
      add_command(std::make_shared<separate_equations_command>(input_filename(), output_filename(), options));
    }
};

int main(int argc, char* argv[])
{
  return mcrl2transform_tool().execute(argc, argv);
}
