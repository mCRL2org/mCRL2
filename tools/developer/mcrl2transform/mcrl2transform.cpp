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
#include "mcrl2/process/alphabet.h"
#include "mcrl2/process/alphabet_bounded.h"
#include "mcrl2/process/alphabet_efficient.h"
#include "mcrl2/process/alphabet_new.h"
#include "mcrl2/process/alphabet_reduce.h"
#include "mcrl2/process/anonymize.h"
#include "mcrl2/process/eliminate_single_usage_equations.h"
#include "mcrl2/process/eliminate_trivial_equations.h"
#include "mcrl2/process/eliminate_unused_equations.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/process/process_info.h"
#include "mcrl2/process/process_variable_strongly_connected_components.h"
#include "mcrl2/process/remove_data_parameters.h"
#include "mcrl2/process/remove_equations.h"
#include "mcrl2/process/rewrite.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/detail/separate_keyword_section.h"

using namespace mcrl2;
using namespace mcrl2::utilities::tools;
using data::tools::rewriter_tool;

/// \brief Reads text from the file filename, or from stdin if filename equals "-".
inline
std::string read_text(const std::string& filename)
{
  if (filename.empty())
  {
    return utilities::read_text(std::cin);
  }
  else
  {
    return utilities::read_text(filename);
  }
}

/// \brief Saves text to the file filename, or to stdout if filename equals "-".
inline
void write_text(const std::string& filename, const std::string& text)
{
  if (filename.empty())
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

struct process_command: public command
{
  process::process_specification procspec;

  process_command(const std::string& name,
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

/// \brief Process command that uses a rewrite strategy
struct process_rewriter_command: public process_command
{
  data::rewrite_strategy strategy;

  process_rewriter_command(const std::string& name,
                           const std::string& input_filename,
                           const std::string& output_filename,
                           const std::vector<std::string>& options,
                           data::rewrite_strategy strategy_
                          )
    : process_command(name, input_filename, output_filename, options),
      strategy(strategy_)
  {}
};

/// \brief Eliminates trivial process equations of the shape P = Q
struct eliminate_trivial_equations_command: public process_command
{
  eliminate_trivial_equations_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process_command("eliminate-trivial-equations", input_filename, output_filename, options)
  {}

  void execute()
  {
    process_command::execute();
    process::eliminate_trivial_equations(procspec);
    write_text(output_filename, process::pp(procspec));
  }
};

/// \brief Applies a bisimulation reduction to process variables
struct join_bisimilar_equations_command: public process_command
{
  join_bisimilar_equations_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process_command("join-bisimilar-equations", input_filename, output_filename, options)
  {}

  void execute()
  {
    process_command::execute();
    process::remove_duplicate_equations(procspec);
    write_text(output_filename, process::pp(procspec));
  }
};

/// \brief Applies alphabet reduction to a process
struct alphabet_reduce_command: public process_command
{
  alphabet_reduce_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process_command("alphabet-reduce", input_filename, output_filename, options)
  {}

  void execute()
  {
    process_command::execute();
    process::alphabet_reduce(procspec, 0);
    write_text(output_filename, process::pp(procspec));
  }
};

/// \brief Computes strongly connected components of processes
struct process_scc_command: public process_command
{
  process_scc_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process_command("process-scc", input_filename, output_filename, options)
  {}

  void execute()
  {
    process_command::execute();
    for (const std::set<process::process_identifier>& component: process::process_variable_strongly_connected_components(procspec.equations()))
    {
      std::cout << core::detail::print_set(component) << std::endl;
    }
  }
};

/// \brief Eliminates equations that are used in a single place
struct eliminate_single_usage_equations_command: public process_command
{
  eliminate_single_usage_equations_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process_command("eliminate-single-usage-equations", input_filename, output_filename, options)
  {}

  void execute()
  {
    process_command::execute();
    process::eliminate_single_usage_equations(procspec);
    write_text(output_filename, process::pp(procspec));
  }
};

/// \brief Eliminates unused equations
struct eliminate_unused_equations_command: public process_command
{
  eliminate_unused_equations_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process_command("eliminate-unused-equations", input_filename, output_filename, options)
  {}

  void execute()
  {
    process_command::execute();
    process::eliminate_unused_equations(procspec.equations(), procspec.init());
    write_text(output_filename, process::pp(procspec));
  }
};

/// \brief Prints information about a process specification
struct process_info_command: public process_command
{
  process_info_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process_command("process-info", input_filename, output_filename, options)
  {}

  void execute()
  {
    process_command::execute();
    process_info(procspec);
  }
};

/// \brief Removes data parameters from a process
struct remove_data_parameters_command: public process_command
{
  remove_data_parameters_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process_command("remove-data-parameters", input_filename, output_filename, options)
  {}

  void execute()
  {
    process_command::execute();
    remove_data_parameters(procspec);
    write_text(output_filename, process::pp(procspec));
  }
};

/// \brief Computes the alphabet of the initial state of a process
struct alphabet_command: public process_command
{
  alphabet_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process_command("alphabet", input_filename, output_filename, options)
  {}

  void execute()
  {
    process_command::execute();
    process::multi_action_name_set alpha = process::alphabet(procspec.init(), procspec.equations());
    write_text(output_filename, process::pp(alpha));
  }
};

/// \brief Computes the alphabet of the initial state of a process
struct alphabet_efficient_command: public process_command
{
  alphabet_efficient_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process_command("alphabet-efficient", input_filename, output_filename, options)
  {}

  void execute()
  {
    process_command::execute();
    process::multi_action_name_set alpha = process::alphabet_efficient(procspec.init(), procspec.equations());
    write_text(output_filename, process::pp(alpha));
  }
};

/// \brief Computes the alphabet of the initial state of a process
struct alphabet_new_command: public process_command
{
  alphabet_new_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process_command("alphabet-new", input_filename, output_filename, options)
  {}

  void execute()
  {
    process_command::execute();
    process::multi_action_name_set alpha = process::alphabet_new(procspec.init(), procspec.equations());
    write_text(output_filename, process::pp(alpha));
  }
};

/// \brief Computes the alphabet of the initial state of a process, if it is an allow
struct alphabet_bounded_command: public process_command
{
  alphabet_bounded_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process_command("alphabet-bounded", input_filename, output_filename, options)
  {}

  void execute()
  {
    process_command::execute();
    if (!process::is_allow(procspec.init()))
    {
      std::cout << "Error: expected an allow set!" << std::endl;
      return;
    }
    const process::allow& init = atermpp::down_cast<process::allow>(procspec.init());

    process::multi_action_name_set A;
    for (const process::action_name_multiset& alpha: init.allow_set())
    {
      const auto& names = alpha.names();
      A.insert(process::multi_action_name(names.begin(), names.end()));
    }
    process::multi_action_name_set A1 = process::alphabet_bounded(init.operand(), A, procspec.equations());
    write_text(output_filename, process::pp(A1));
  }
};

/// \brief Anonimizes the identifiers of a process specification
struct anonymize_process_command: public process_command
{
  anonymize_process_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process_command("anonymize", input_filename, output_filename, options)
  {}

  void execute()
  {
    process_command::execute();
    process::anonymize(procspec);
    write_text(output_filename, process::pp(procspec));
  }
};

struct rewrite_process_command: public process_rewriter_command
{
  rewrite_process_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options, data::rewrite_strategy strategy)
    : process_rewriter_command("rewrite-process", input_filename, output_filename, options, strategy)
  {}

  void execute()
  {
    process_rewriter_command::execute();
    data::rewriter r(procspec.data(), strategy);
    process::rewrite(procspec, r);
    write_text(output_filename, process::pp(procspec));
  }
};

struct separate_equations_command: public process_command
{
  separate_equations_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : process_command("separate-equations", input_filename, output_filename, options)
  {}

  void execute()
  {
    std::string text = read_text(input_filename);
    std::vector<std::string> all_keywords = { "sort", "var", "map", "cons", "proc", "init", "act" };
    bool repeat_keyword = true;
    std::pair<std::string, std::string> q;

    q = utilities::detail::separate_keyword_section(text, "var", all_keywords, repeat_keyword);
    std::string var_text = q.first;
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
    write_text(output_filename, text);
  }
};

class transform_tool: public rewriter_tool<input_output_tool>
{
  protected:
    typedef rewriter_tool<input_output_tool> super;

    std::string algorithm_and_options;
    int algorithm_number = -1;
    bool print_algorithms = false;

    void parse_options(const utilities::command_line_parser& parser)
    {
      super::parse_options(parser);
      algorithm_and_options = parser.option_argument("algorithm");
      algorithm_number = parser.option_argument_as<int>("number");
      print_algorithms = parser.options.count("print-algorithms") > 0;
    }

    void add_options(utilities::interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("algorithm", utilities::make_optional_argument<std::string>("NAME", ""), "the algorithm that is to be applied", 'a');
      desc.add_option("number", utilities::make_optional_argument<int>("NAME", "-1"), "the number of the algorithm that is to be applied", 'n');
      desc.add_option("print-algorithms", "print the available algorithms", 'p');
    }

    inline
    void add_command(std::map<std::string, std::shared_ptr<command>>& commands, const std::shared_ptr<command>& command) const
    {
      commands[command->name] = command;
    }

  public:
    transform_tool()
      : super("mcrl2transform",
              "Wieger Wesselink",
              "applies a transformation to an mCRL2 process specification",
              "Transform the object in INFILE and write the result to OUTFILE. If OUTFILE "
              "is not present, stdout is used. If INFILE is not present, stdin is used."
             )
    {}

    bool run()
    {
      std::vector<std::string> options;
      std::set<std::string> algorithms;
      std::string algorithm;
      std::map<std::string, std::shared_ptr<command>> commands;

      // process algorithms
      add_command(commands, std::make_shared<process_scc_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<eliminate_trivial_equations_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<join_bisimilar_equations_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<alphabet_reduce_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<eliminate_single_usage_equations_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<eliminate_unused_equations_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<process_info_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<remove_data_parameters_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<alphabet_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<alphabet_efficient_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<alphabet_new_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<alphabet_bounded_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<anonymize_process_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<rewrite_process_command>(input_filename(), output_filename(), options, rewrite_strategy()));
      add_command(commands, std::make_shared<separate_equations_command>(input_filename(), output_filename(), options));

      for (auto i = commands.begin(); i != commands.end(); ++i)
      {
        algorithms.insert(i->first);
      }

      if (algorithm_number >= 0 && !algorithm_and_options.empty())
      {
        throw mcrl2::runtime_error("It is not allowed to set both number and algorithm!");
      }

      // print the algorithms
      if (print_algorithms || (algorithm_number < 0 && algorithm_and_options.empty()))
      {
        int index = 1;
        std::cout << "The following algorithms are available:" << std::endl;
        for (auto const& algorithm: algorithms)
        {
          std::cout << index++ << ") " << algorithm << std::endl;
        }
        return true;
      }

      // if a number was specified, lookup the corresponding algorithm
      if (algorithm_number >= 0)
      {
        int index = 1;
        for (auto const& algo: algorithms)
        {
          if (index++ == algorithm_number)
          {
            algorithm = algo;
          }
        }
      }
      else
      {
        options = utilities::regex_split(algorithm_and_options, "\\s+");
        algorithm = options[0];
        options.erase(options.begin());
      }

      // run the algorithm
      auto i = commands.find(algorithm);
      if (i == commands.end())
      {
        throw std::runtime_error("Unknown algorithm " + algorithm);
      }
      i->second->execute();

      return true;
    }
};

int main(int argc, char* argv[])
{
  return transform_tool().execute(argc, argv);
}
