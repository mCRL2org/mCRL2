// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbestransform.cpp

#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <sstream>

#include "mcrl2/bes/io.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/anonymize.h"
#include "mcrl2/pbes/detail/pbes_command.h"
#include "mcrl2/pbes/pbesinst_lazy.h"
#include "mcrl2/pbes/pbesinst_structure_graph.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriters/data2pbes_rewriter.h"
#include "mcrl2/pbes/rewriters/data_rewriter.h"
#include "mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/pbes/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/pbes/rewriters/quantifiers_inside_rewriter.h"
#include "mcrl2/pbes/rewriters/simplify_quantifiers_rewriter.h"
#include "mcrl2/pbes/rewriters/simplify_rewriter.h"
#include "mcrl2/pbes/stategraph.h"
#include "mcrl2/utilities/detail/io.h"
#include "mcrl2/utilities/detail/transform_tool.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using data::tools::rewriter_tool;
using utilities::detail::transform_tool;
using utilities::tools::input_output_tool;

/// \brief Anonimizes the identifiers of a PBES
struct anonymize_pbes_command: public pbes_system::detail::pbes_command
{
  anonymize_pbes_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbes_system::detail::pbes_command("anonymize", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    pbes_system::anonymize(pbesspec);
    pbes_system::save_pbes(pbesspec, output_filename);
  }
};

// PBES rewriters
struct rewrite_pbes_data_rewriter_command: public pbes_system::detail::pbes_rewriter_command
{
  rewrite_pbes_data_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options, data::rewrite_strategy strategy)
    : pbes_system::detail::pbes_rewriter_command("pbes-data-rewriter", input_filename, output_filename, options, strategy)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    data::rewriter r(pbesspec.data(), strategy);
    pbes_system::data_rewriter<data::rewriter> R(r);
    pbes_rewrite(pbesspec, R);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_enumerate_quantifiers_rewriter_command: public pbes_system::detail::pbes_rewriter_command
{
  rewrite_pbes_enumerate_quantifiers_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options, data::rewrite_strategy strategy)
    : pbes_system::detail::pbes_rewriter_command("pbes-enumerate-quantifiers-rewriter", input_filename, output_filename, options, strategy)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    data::rewriter r(pbesspec.data(), strategy);
    pbes_system::enumerate_quantifiers_rewriter R(r, pbesspec.data());
    pbes_rewrite(pbesspec, R);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_simplify_rewriter_command: public pbes_system::detail::pbes_command
{
  rewrite_pbes_simplify_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbes_system::detail::pbes_command("pbes-simplify-rewriter", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    pbes_system::simplify_rewriter R;
    pbes_rewrite(pbesspec, R);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_simplify_data_rewriter_command: public pbes_system::detail::pbes_rewriter_command
{
  rewrite_pbes_simplify_data_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options, data::rewrite_strategy strategy)
    : pbes_system::detail::pbes_rewriter_command("pbes-simplify-data-rewriter", input_filename, output_filename, options, strategy)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    data::rewriter r(pbesspec.data(), strategy);
    pbes_system::simplify_data_rewriter<data::rewriter> R(r);
    pbes_rewrite(pbesspec, R);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_simplify_quantifiers_rewriter_command: public pbes_system::detail::pbes_command
{
  rewrite_pbes_simplify_quantifiers_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbes_system::detail::pbes_command("pbes-simplify-quantifiers-rewriter", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    pbes_system::simplify_quantifiers_rewriter R;
    pbes_rewrite(pbesspec, R);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_simplify_quantifiers_data_rewriter_command: public pbes_system::detail::pbes_rewriter_command
{
  rewrite_pbes_simplify_quantifiers_data_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options, data::rewrite_strategy strategy)
    : pbes_system::detail::pbes_rewriter_command("pbes-simplify-quantifiers-data-rewriter", input_filename, output_filename, options, strategy)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    data::rewriter r(pbesspec.data(), strategy);
    pbes_system::simplify_data_rewriter<data::rewriter> R(r);
    pbes_rewrite(pbesspec, R);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_one_point_rule_rewriter_command: public pbes_system::detail::pbes_command
{
  rewrite_pbes_one_point_rule_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbes_system::detail::pbes_command("pbes-one-point-rule-rewriter", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    pbes_system::one_point_rule_rewriter R;
    pbes_rewrite(pbesspec, R);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_quantifiers_inside_rewriter_command: public pbes_system::detail::pbes_command
{
  rewrite_pbes_quantifiers_inside_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbes_system::detail::pbes_command("pbes-quantifiers-inside-rewriter", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    pbes_system::quantifiers_inside_rewriter R;
    pbes_rewrite(pbesspec, R);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_data2pbes_rewriter_command: public pbes_system::detail::pbes_command
{
  rewrite_pbes_data2pbes_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbes_system::detail::pbes_command("data-to-pbes-rewriter", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    pbes_system::data2pbes_rewriter R;
    pbes_rewrite(pbesspec, R);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct pbesinst_lazy_command: public pbes_system::detail::pbes_rewriter_command
{
  pbesinst_lazy_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options, data::rewrite_strategy strategy)
    : pbes_system::detail::pbes_rewriter_command("pbesinst-lazy", input_filename, output_filename, options, strategy)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    pbesspec = pbesinst_lazy(pbesspec, strategy, breadth_first, lazy);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

/// \brief Computes a structure graph for a PBES and prints the result.
struct pbesinst_structure_graph_command: public pbes_system::detail::pbes_rewriter_command
{
  pbesinst_structure_graph_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options, data::rewrite_strategy strategy)
    : pbes_system::detail::pbes_rewriter_command("pbesinst-structure-graph", input_filename, output_filename, options, strategy)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    structure_graph G;
    pbesinst_structure_graph(pbesspec, G, strategy, breadth_first, lazy);
    std::ostringstream out;
    out << G.vertices();
    utilities::detail::write_text(output_filename, out.str());
  }
};

struct pbesinst_optimize_command: public pbes_system::detail::pbes_rewriter_command
{
  pbesinst_optimize_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options, data::rewrite_strategy strategy)
    : pbes_system::detail::pbes_rewriter_command("pbesinst-optimize", input_filename, output_filename, options, strategy)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    pbesspec = pbesinst_lazy(pbesspec, strategy, breadth_first, optimize);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct pbesinst_on_the_fly_command: public pbes_system::detail::pbes_rewriter_command
{
  pbesinst_on_the_fly_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options, data::rewrite_strategy strategy)
    : pbes_system::detail::pbes_rewriter_command("pbesinst-on-the-fly", input_filename, output_filename, options, strategy)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    pbesspec = pbesinst_lazy(pbesspec, strategy, breadth_first, on_the_fly);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct pbesinst_on_the_fly_with_fixed_points_command: public pbes_system::detail::pbes_rewriter_command
{
  pbesinst_on_the_fly_with_fixed_points_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options, data::rewrite_strategy strategy)
    : pbes_system::detail::pbes_rewriter_command("pbesinst-on-the-fly-with-fixed-points", input_filename, output_filename, options, strategy)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    pbesspec = pbesinst_lazy(pbesspec, strategy, breadth_first, on_the_fly_with_fixed_points);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct stategraph_local_command: public pbes_system::detail::pbes_command
{
  stategraph_local_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbes_system::detail::pbes_command("stategraph-local", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    pbesstategraph_options options;
    options.use_global_variant = false;
    stategraph(pbesspec, options);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct stategraph_global_command: public pbes_system::detail::pbes_command
{
  stategraph_global_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbes_system::detail::pbes_command("stategraph-global", input_filename, output_filename, options)
  {}

  void execute()
  {
    pbes_system::detail::pbes_command::execute();
    pbesstategraph_options options;
    options.use_global_variant = false;
    stategraph(pbesspec, options);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

class pbestransform_tool: public transform_tool<rewriter_tool<input_output_tool>>
{
  typedef transform_tool<rewriter_tool<input_output_tool>> super;

  public:
    pbestransform_tool()
      : super("pbestransform",
              "Wieger Wesselink",
              "applies a transformation to a PBES",
              "Transform the PBES in INFILE and write the result to OUTFILE. If OUTFILE "
              "is not present, stdout is used. If INFILE is not present, stdin is used."
             )
    {}

    void add_commands(const std::vector<std::string>& options) override
    {
      add_command(std::make_shared<anonymize_pbes_command>(input_filename(), output_filename(), options));
      add_command(std::make_shared<pbesinst_lazy_command>(input_filename(), output_filename(), options, rewrite_strategy()));
      add_command(std::make_shared<pbesinst_structure_graph_command>(input_filename(), output_filename(), options, rewrite_strategy()));
      add_command(std::make_shared<pbesinst_on_the_fly_command>(input_filename(), output_filename(), options, rewrite_strategy()));
      add_command(std::make_shared<pbesinst_on_the_fly_with_fixed_points_command>(input_filename(), output_filename(), options, rewrite_strategy()));
      add_command(std::make_shared<pbesinst_optimize_command>(input_filename(), output_filename(), options, rewrite_strategy()));
      add_command(std::make_shared<rewrite_pbes_data2pbes_rewriter_command>(input_filename(), output_filename(), options));
      add_command(std::make_shared<rewrite_pbes_data_rewriter_command>(input_filename(), output_filename(), options, rewrite_strategy()));
      add_command(std::make_shared<rewrite_pbes_enumerate_quantifiers_rewriter_command>(input_filename(), output_filename(), options, rewrite_strategy()));
      add_command(std::make_shared<rewrite_pbes_one_point_rule_rewriter_command>(input_filename(), output_filename(), options));
      add_command(std::make_shared<rewrite_pbes_quantifiers_inside_rewriter_command>(input_filename(), output_filename(), options));
      add_command(std::make_shared<rewrite_pbes_simplify_data_rewriter_command>(input_filename(), output_filename(), options, rewrite_strategy()));
      add_command(std::make_shared<rewrite_pbes_simplify_quantifiers_data_rewriter_command>(input_filename(), output_filename(), options, rewrite_strategy()));
      add_command(std::make_shared<rewrite_pbes_simplify_quantifiers_rewriter_command>(input_filename(), output_filename(), options));
      add_command(std::make_shared<rewrite_pbes_simplify_rewriter_command>(input_filename(), output_filename(), options));
      add_command(std::make_shared<stategraph_global_command>(input_filename(), output_filename(), options));
      add_command(std::make_shared<stategraph_local_command>(input_filename(), output_filename(), options));
    }
};

int main(int argc, char* argv[])
{
  return pbestransform_tool().execute(argc, argv);
}
