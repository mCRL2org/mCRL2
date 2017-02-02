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

#include "mcrl2/bes/io.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbesinst_lazy.h"
#include "mcrl2/pbes/remove_equations.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriters/data_rewriter.h"
#include "mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/pbes/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/pbes/rewriters/quantifiers_inside_rewriter.h"
#include "mcrl2/pbes/rewriters/simplify_quantifiers_rewriter.h"
#include "mcrl2/pbes/rewriters/simplify_rewriter.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

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

inline
std::string file_extension(const std::string& filename)
{
  auto pos = filename.find_last_of('.');
  if (pos == std::string::npos)
  {
    return "";
  }
  return filename.substr(pos + 1);
}

inline
void my_save_pbes(const pbes& p, const std::string& filename)
{
  auto ext = file_extension(filename);
  if (ext == "pbes")
  {
    save_pbes(p, filename, pbes_format_internal());
  }
  else if (ext == "bes")
  {
    bes::save_pbes(p, filename, bes::bes_format_internal());
  }
  else if (ext == "pg")
  {
    bes::save_pbes(p, filename, bes::bes_format_pgsolver());
  }
  else
  {
    pbes_system::save_pbes(p, filename);
  }
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

struct pbescommand: public command
{
  pbes_system::pbes pbesspec;

  pbescommand(const std::string& name,
              const std::string& input_filename,
              const std::string& output_filename,
              const std::vector<std::string>& options
             )
    : command(name, input_filename, output_filename, options)
  {}

  void execute()
  {
    pbes_system::load_pbes(pbesspec, input_filename);
  }
};

// PBES rewriters
struct rewrite_pbes_data_rewriter_command: public pbescommand
{
  rewrite_pbes_data_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbescommand("pbes-data-rewriter", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbescommand::execute();
    data::rewriter r(pbesspec.data());
    pbes_system::data_rewriter<data::rewriter> R(r);
    pbes_rewrite(pbesspec, R);
    my_save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_enumerate_quantifiers_rewriter_command: public pbescommand
{
  rewrite_pbes_enumerate_quantifiers_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbescommand("pbes-enumerate-quantifiers-rewriter", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbescommand::execute();
    data::rewriter r(pbesspec.data());
    pbes_system::enumerate_quantifiers_rewriter R(r, pbesspec.data());
    pbes_rewrite(pbesspec, R);
    my_save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_simplify_rewriter_command: public pbescommand
{
  rewrite_pbes_simplify_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbescommand("pbes-simplify-rewriter", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbescommand::execute();
    pbes_system::simplify_rewriter R;
    pbes_rewrite(pbesspec, R);
    my_save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_simplify_data_rewriter_command: public pbescommand
{
  rewrite_pbes_simplify_data_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbescommand("pbes-simplify-data-rewriter", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbescommand::execute();
    data::rewriter r(pbesspec.data());
    pbes_system::simplify_data_rewriter<data::rewriter> R(r);
    pbes_rewrite(pbesspec, R);
    my_save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_simplify_quantifiers_rewriter_command: public pbescommand
{
  rewrite_pbes_simplify_quantifiers_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbescommand("pbes-simplify-quantifiers-rewriter", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbescommand::execute();
    pbes_system::simplify_quantifiers_rewriter R;
    pbes_rewrite(pbesspec, R);
    my_save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_simplify_quantifiers_data_rewriter_command: public pbescommand
{
  rewrite_pbes_simplify_quantifiers_data_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbescommand("pbes-simplify-quantifiers-data-rewriter", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbescommand::execute();
    data::rewriter r(pbesspec.data());
    pbes_system::simplify_data_rewriter<data::rewriter> R(r);
    pbes_rewrite(pbesspec, R);
    my_save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_one_point_rule_rewriter_command: public pbescommand
{
  rewrite_pbes_one_point_rule_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbescommand("pbes-one-point-rule-rewriter", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbescommand::execute();
    pbes_system::one_point_rule_rewriter R;
    pbes_rewrite(pbesspec, R);
    my_save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_quantifiers_inside_rewriter_command: public pbescommand
{
  rewrite_pbes_quantifiers_inside_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbescommand("pbes-quantifiers-inside-rewriter", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbescommand::execute();
    pbes_system::quantifiers_inside_rewriter R;
    pbes_rewrite(pbesspec, R);
    my_save_pbes(pbesspec, output_filename);
  }
};

struct remove_unused_pbes_equations_command: public pbescommand
{
  remove_unused_pbes_equations_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbescommand("remove-unused-pbes-equations", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbescommand::execute();
    pbes_system::remove_unreachable_variables(pbesspec);
    my_save_pbes(pbesspec, output_filename);
  }
};

struct pbesinst_lazy_command: public pbescommand
{
  pbesinst_lazy_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbescommand("pbesinst-lazy", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbescommand::execute();
    pbesspec = pbesinst_lazy(pbesspec, data::jitty, breadth_first, lazy);
    my_save_pbes(pbesspec, output_filename);
  }
};

struct pbesinst_optimize_command: public pbescommand
{
  pbesinst_optimize_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbescommand("pbesinst-optimize", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbescommand::execute();
    pbesspec = pbesinst_lazy(pbesspec, data::jitty, breadth_first, optimize);
    my_save_pbes(pbesspec, output_filename);
  }
};

struct pbesinst_on_the_fly_command: public pbescommand
{
  pbesinst_on_the_fly_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbescommand("pbesinst-on-the-fly", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbescommand::execute();
    pbesspec = pbesinst_lazy(pbesspec, data::jitty, breadth_first, on_the_fly);
    my_save_pbes(pbesspec, output_filename);
  }
};

struct pbesinst_on_the_fly_with_fixed_points_command: public pbescommand
{
  pbesinst_on_the_fly_with_fixed_points_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbescommand("pbesinst-on-the-fly-with-fixed-points", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbescommand::execute();
    pbesspec = pbesinst_lazy(pbesspec, data::jitty, breadth_first, on_the_fly_with_fixed_points);
    my_save_pbes(pbesspec, output_filename);
  }
};

class transform_tool: public utilities::tools::input_output_tool
{
  protected:
    typedef utilities::tools::input_output_tool super;

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
              "applies a transformation to a PBES",
              "Transform the PBES in INFILE and write the result to OUTFILE. If OUTFILE "
              "is not present, stdout is used. If INFILE is not present, stdin is used."
             )
    {}

    bool run()
    {
      std::vector<std::string> options;
      std::set<std::string> algorithms;
      std::string algorithm;
      std::map<std::string, std::shared_ptr<command>> commands;

      // PBES algorithms
      add_command(commands, std::make_shared<rewrite_pbes_data_rewriter_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<rewrite_pbes_enumerate_quantifiers_rewriter_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<rewrite_pbes_simplify_rewriter_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<rewrite_pbes_simplify_data_rewriter_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<rewrite_pbes_simplify_quantifiers_rewriter_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<rewrite_pbes_simplify_quantifiers_data_rewriter_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<rewrite_pbes_one_point_rule_rewriter_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<rewrite_pbes_quantifiers_inside_rewriter_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<remove_unused_pbes_equations_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<pbesinst_lazy_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<pbesinst_optimize_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<pbesinst_on_the_fly_command>(input_filename(), output_filename(), options));
      add_command(commands, std::make_shared<pbesinst_on_the_fly_with_fixed_points_command>(input_filename(), output_filename(), options));

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