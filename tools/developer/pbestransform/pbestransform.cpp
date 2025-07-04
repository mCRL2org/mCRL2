// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbestransform.cpp

#include "mcrl2/utilities/detail/io.h"
#include "mcrl2/utilities/detail/transform_tool.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/anonymize.h"
#include "mcrl2/pbes/detail/pbes_command.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/pbesinst_structure_graph.h"
#include "mcrl2/pbes/rewriters/data2pbes_rewriter.h"
#include "mcrl2/pbes/rewriters/if_rewriter.h"
#include "mcrl2/pbes/rewriters/quantifiers_inside_rewriter.h"
#include "mcrl2/pbes/quantifier_propagate.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/stategraph.h"
#include "mcrl2/pbes/unify_parameters.h"

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

  void execute() override
  {
    pbes_system::detail::pbes_command::execute();
    pbes_system::anonymize(pbesspec);
    pbes_system::save_pbes(pbesspec, output_filename);
  }
};

struct is_well_typed_command: public pbes_system::detail::pbes_command
{
  is_well_typed_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbes_system::detail::pbes_command("is-well-typed", input_filename, output_filename, options)
  {}

  void execute() override
  {
    pbes_system::detail::pbes_command::execute();
    bool result = pbesspec.is_well_typed();
    utilities::detail::write_text(output_filename, result ? "true\n" : "false\n");
  }
};

// PBES rewriters
struct rewrite_pbes_data_rewriter_command: public pbes_system::detail::pbes_rewriter_command
{
  rewrite_pbes_data_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options, data::rewrite_strategy strategy)
    : pbes_system::detail::pbes_rewriter_command("pbes-data-rewriter", input_filename, output_filename, options, strategy)
  {}

  void execute() override
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

  void execute() override
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

  void execute() override
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

  void execute() override
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

  void execute() override
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

  void execute() override
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

  void execute() override
  {
    pbes_system::detail::pbes_command::execute();
    pbes_system::one_point_rule_rewriter R;
    replace_pbes_expressions(pbesspec, R, false);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_quantifiers_inside_rewriter_command: public pbes_system::detail::pbes_command
{
  rewrite_pbes_quantifiers_inside_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbes_system::detail::pbes_command("pbes-quantifiers-inside-rewriter", input_filename, output_filename, options)
  {}

  void execute() override
  {
    pbes_system::detail::pbes_command::execute();
    pbes_system::quantifiers_inside_rewriter R;
    pbes_rewrite(pbesspec, R);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_quantifier_propagate_command: public pbes_system::detail::pbes_command
{
  rewrite_pbes_quantifier_propagate_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbes_system::detail::pbes_command("pbes-quantifier-propagate", input_filename, output_filename, options)
  {}

  void execute() override
  {
    pbes_system::detail::pbes_command::execute();
    quantifier_propagate(pbesspec);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_data2pbes_rewriter_command: public pbes_system::detail::pbes_command
{
  rewrite_pbes_data2pbes_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbes_system::detail::pbes_command("data-to-pbes-rewriter", input_filename, output_filename, options)
  {}

  void execute() override
  {
    pbes_system::detail::pbes_command::execute();
    pbes_system::data2pbes_rewriter R;
    pbes_rewrite(pbesspec, R);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct rewrite_pbes_if_rewriter_command: public pbes_system::detail::pbes_command
{
  rewrite_pbes_if_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
      : pbes_system::detail::pbes_command("if-rewriter", input_filename, output_filename, options)
  {}

  void execute() override
  {
    pbes_system::detail::pbes_command::execute();
    pbes_system::if_rewriter R;
    pbes_rewrite(pbesspec, R);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct stategraph_local_command: public pbes_system::detail::pbes_command
{
  stategraph_local_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbes_system::detail::pbes_command("stategraph-local", input_filename, output_filename, options)
  {}

  void execute() override
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

  void execute() override
  {
    pbes_system::detail::pbes_command::execute();
    pbesstategraph_options options;
    options.use_global_variant = false;
    stategraph(pbesspec, options);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct unify_parameters_command: public pbes_system::detail::pbes_command
{
  unify_parameters_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbes_system::detail::pbes_command("unify-parameters", input_filename, output_filename, options)
  {}

  void execute() override
  {
    pbes_system::detail::pbes_command::execute();
    pbes_system::unify_parameters(pbesspec, false, false);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct standard_recursive_form_command: public pbes_system::detail::pbes_command
{
  standard_recursive_form_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : pbes_system::detail::pbes_command("srf", input_filename, output_filename, options)
  {}

  void execute() override
  {
    pbes_system::detail::pbes_command::execute();
    pbes_system::algorithms::normalize(pbesspec);
    pbes_system::algorithms::instantiate_global_variables(pbesspec);
    pbes_system::srf_pbes p = pbes2srf(pbesspec);
    unify_parameters(p, false, true);
    pbesspec = p.to_pbes();
    if (!pbesspec.is_well_typed())
    {
      throw mcrl2::runtime_error("the PBES is not well typed!");
    }
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

struct normalize_command: public pbes_system::detail::pbes_command
{
  normalize_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
      : pbes_system::detail::pbes_command("normalize", input_filename, output_filename, options)
  {}

  void execute() override
  {
    pbes_system::detail::pbes_command::execute();
    normalize(pbesspec);
    pbes_system::detail::save_pbes(pbesspec, output_filename);
  }
};

class pbestransform_tool: public transform_tool<rewriter_tool<input_output_tool>>
{
  using super = transform_tool<rewriter_tool<input_output_tool>>;

public:
  pbestransform_tool()
      : super("pbestransform",
            "Wieger Wesselink",
            "applies a transformation to a PBES",
            "Transform the PBES in INFILE and write the result to OUTFILE. If OUTFILE "
            "is not present, stdout is used. If INFILE is not present, stdin is used.")
  {}

  void add_commands(const std::vector<std::string>& options) override
  {
    add_command(std::make_shared<anonymize_pbes_command>(input_filename(), output_filename(), options));
    add_command(std::make_shared<is_well_typed_command>(input_filename(), output_filename(), options));
    add_command(std::make_shared<normalize_command>(input_filename(), output_filename(), options));
    add_command(std::make_shared<standard_recursive_form_command>(input_filename(), output_filename(), options));
    add_command(std::make_shared<rewrite_pbes_if_rewriter_command>(input_filename(), output_filename(), options));
    add_command(
        std::make_shared<rewrite_pbes_data2pbes_rewriter_command>(input_filename(), output_filename(), options));
    add_command(std::make_shared<rewrite_pbes_data_rewriter_command>(input_filename(),
        output_filename(),
        options,
        rewrite_strategy()));
    add_command(std::make_shared<rewrite_pbes_enumerate_quantifiers_rewriter_command>(input_filename(),
        output_filename(),
        options,
        rewrite_strategy()));
    add_command(
        std::make_shared<rewrite_pbes_one_point_rule_rewriter_command>(input_filename(), output_filename(), options));
    add_command(std::make_shared<rewrite_pbes_quantifiers_inside_rewriter_command>(input_filename(),
        output_filename(),
        options));
    add_command(
        std::make_shared<rewrite_pbes_quantifier_propagate_command>(input_filename(), output_filename(), options));
    add_command(std::make_shared<rewrite_pbes_simplify_data_rewriter_command>(input_filename(),
        output_filename(),
        options,
        rewrite_strategy()));
    add_command(std::make_shared<rewrite_pbes_simplify_quantifiers_data_rewriter_command>(input_filename(),
        output_filename(),
        options,
        rewrite_strategy()));
    add_command(std::make_shared<rewrite_pbes_simplify_quantifiers_rewriter_command>(input_filename(),
        output_filename(),
        options));
    add_command(std::make_shared<rewrite_pbes_simplify_rewriter_command>(input_filename(), output_filename(), options));
    add_command(std::make_shared<stategraph_global_command>(input_filename(), output_filename(), options));
    add_command(std::make_shared<stategraph_local_command>(input_filename(), output_filename(), options));
    add_command(std::make_shared<unify_parameters_command>(input_filename(), output_filename(), options));
    }
};

int main(int argc, char* argv[])
{
  return pbestransform_tool().execute(argc, argv);
}
