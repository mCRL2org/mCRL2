// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transform.cpp

#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include <boost/lexical_cast.hpp>

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/process/alphabet.h"
#include "mcrl2/process/eliminate_single_usage_equations.h"
#include "mcrl2/process/eliminate_trivial_equations.h"
#include "mcrl2/process/eliminate_unused_equations.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/process/process_info.h"
#include "mcrl2/process/process_variable_strongly_connected_components.h"
#include "mcrl2/process/remove_equations.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;

/// \brief Saves text to the file filename, or to stdout if filename equals "-".
inline
void write_text(const std::string& filename, const std::string& text)
{
  if (filename == "-")
  {
    std::cout << text;
  }
  else
  {
    std::ofstream out(filename);
    out << text;
  }
}

/// \brief Loads a process specification from input_filename, or from stdin if filename equals "".
inline
process::process_specification parse_process_specification(const std::string& input_filename)
{
  process::process_specification result;
  if (input_filename.empty())
  {
    return process::parse_process_specification(std::cin);
  }
  else
  {
    std::ifstream from(input_filename, std::ifstream::in | std::ifstream::binary);
    return process::parse_process_specification(from);
  }
  return result;
}

/// \brief
struct command
{
  std::string name;
  const std::string& input_filename;
  const std::string& output_filename;
  const std::vector<std::string>& options;

  command(const std::string& name_,
          const std::string& input_filename_,
          const std::string& output_filename_,
          const std::vector<std::string>& options_
         )
    : name(name_),
      input_filename(input_filename_),
      output_filename(output_filename_),
      options(options_)
  {}

  virtual void execute() = 0;
};

struct processcommand: public command
{
  process::process_specification procspec;

  processcommand(const std::string& name,
                 const std::string& input_filename,
                 const std::string& output_filename,
                 const std::vector<std::string>& options
                )
    : command(name, input_filename, output_filename, options)
  {}

  void execute()
  {
    procspec = parse_process_specification(input_filename);
  }
};

/// \brief Eliminates trivial process equations of the shape P = Q
struct eliminate_trivial_equations_command: public processcommand
{
  eliminate_trivial_equations_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : processcommand("eliminate-trivial-equations", input_filename, output_filename, options)
  {}

  void execute()
  {
    processcommand::execute();
    process::eliminate_trivial_equations(procspec);
    write_text(output_filename, process::pp(procspec));
  }
};

/// \brief Applies a bisimulation reduction to process variables
struct join_bisimilar_equations_command: public processcommand
{
  join_bisimilar_equations_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : processcommand("join-bisimilar-equations", input_filename, output_filename, options)
  {}

  void execute()
  {
    processcommand::execute();
    process::remove_duplicate_equations(procspec);
    write_text(output_filename, process::pp(procspec));
  }
};

/// \brief Applies alphabet reduction to a process
struct alphabet_reduce_command: public processcommand
{
  alphabet_reduce_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : processcommand("alphabet-reduce", input_filename, output_filename, options)
  {}

  void execute()
  {
    processcommand::execute();
    process::remove_duplicate_equations(procspec);
    write_text(output_filename, process::pp(procspec));
  }
};

/// \brief Computes strongly connected components of processes
struct process_scc_command: public processcommand
{
  process_scc_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : processcommand("process-scc", input_filename, output_filename, options)
  {}

  void execute()
  {
    processcommand::execute();
    for (const std::set<process::process_identifier>& component: process::process_variable_strongly_connected_components(procspec.equations()))
    {
      std::cout << core::detail::print_set(component) << std::endl;
    }
  }
};

/// \brief Eliminates equations that are used in a single place
struct eliminate_single_usage_equations_command: public processcommand
{
  eliminate_single_usage_equations_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : processcommand("eliminate-single-usage-equations", input_filename, output_filename, options)
  {}

  void execute()
  {
    processcommand::execute();
    process::eliminate_single_usage_equations(procspec);
    write_text(output_filename, process::pp(procspec));
  }
};

/// \brief Eliminates unused equations
struct eliminate_unused_equations_command: public processcommand
{
  eliminate_unused_equations_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : processcommand("eliminate-unused-equations", input_filename, output_filename, options)
  {}

  void execute()
  {
    processcommand::execute();
    process::eliminate_unused_equations(procspec);
    write_text(output_filename, process::pp(procspec));
  }
};

/// \brief Prints information about a process specification
struct process_info_command: public processcommand
{
  process_info_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : processcommand("process-info", input_filename, output_filename, options)
  {}

  void execute()
  {
    processcommand::execute();
    process_info(procspec);
  }
};

class transform_tool: public utilities::tools::input_output_tool
{
  protected:
    typedef utilities::tools::input_output_tool super;

    std::string algorithm_and_options;
    bool print_algorithms = false;

    void parse_options(const utilities::command_line_parser& parser)
    {
      super::parse_options(parser);
      algorithm_and_options = parser.option_argument("algorithm");
      print_algorithms = parser.options.count("print-algorithms") > 0;
    }

    void add_options(utilities::interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("algorithm", utilities::make_mandatory_argument("NAME", "novalue"), "the algorithm that is to be applied", 'a');
      desc.add_option("print-algorithms", "print the available algorithms", 'p');
    }

    inline
    void add_command(std::map<std::string, std::shared_ptr<command>>& commands, const std::shared_ptr<command>& command) const
    {
      commands[command->name] = command;
    }

  public:
    transform_tool()
      : super("transform",
              "Wieger Wesselink",
              "applies a transformation to an mCRL2 object",
              "Transform the object in INFILE and write the result to OUTFILE. If OUTFILE "
              "is not present, stdout is used. If INFILE is not present, stdin is used."
             )
    {}

    bool run()
    {
      std::vector<std::string> options = utilities::regex_split(algorithm_and_options, "\\s+");
      std::string algorithm = options[0];
      options.erase(options.begin());

      std::map<std::string, std::shared_ptr<command>> commands;
      add_command(commands, std::make_shared<process_scc_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<eliminate_trivial_equations_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<join_bisimilar_equations_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<alphabet_reduce_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<eliminate_single_usage_equations_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<eliminate_unused_equations_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<process_info_command>(input_filename(), output_filename(), options));

      if (print_algorithms)
      {
        std::set<std::string> algorithms;
        for (auto i = commands.begin(); i != commands.end(); ++i)
        {
          algorithms.insert(i->first);
        }
        std::cout << "The following algorithms are available:" << std::endl;
        for (auto const& algorithm: algorithms)
        {
          std::cout << algorithm << std::endl;
        }
      }
      else
      {
        auto i = commands.find(algorithm);
        if (i == commands.end())
        {
          throw std::runtime_error("Unknown algorithm " + algorithm);
        }
        i->second->execute();
      }
      return true;
    }
};

int main(int argc, char* argv[])
{
  return transform_tool().execute(argc, argv);
}
