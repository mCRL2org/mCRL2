// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transform.cpp

#include <csignal>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <sstream>
#include <mcrl2/lps/generate_lts.h>

#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/generate_lts.h"
#include "mcrl2/lps/detail/lps_io.h"
#include "mcrl2/lts/lts_builder.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/utilities/detail/io.h"
#include "mcrl2/utilities/detail/transform_tool.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using data::tools::rewriter_tool;
using utilities::detail::transform_tool;
using utilities::tools::input_output_tool;

class generatelts_tool: public rewriter_tool<input_output_tool>
{
  typedef rewriter_tool<input_output_tool> super;

  lps::generate_lts_options options;
  lts::lts_type output_format = lts::lts_none;
  lps::lps_explorer* current_explorer = nullptr;

  public:
    generatelts_tool()
      : super("generatelts",
              "Wieger Wesselink",
              "generates an LTS from an LPS",
              "Transforms the LPS in INFILE and writes a corresponding LTS in .aut format "
              " to OUTFILE. If OUTFILE is not present, stdout is used. If INFILE is not "
              " present, stdin is used."
             )
    {}

    void add_options(utilities::interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("cached", "cache enumerator solutions");
      desc.add_option("global-cache", "use a global cache");
      desc.add_option("confluence", "apply confluence reduction", 'c');
      desc.add_option("no-one-point-rule-rewrite", "do not apply the one point rule rewriter");
      desc.add_option("no-replace-constants-by-variables", "do not move constant expressions to a substitution");
      desc.add_option("no-resolve-summand-variable-name-clashes", "do not resolve summand variable name clashes");
      desc.add_option("out", utilities::make_mandatory_argument("FORMAT"), "save the output in the specified FORMAT. ", 'o');
      options.rewrite_strategy = rewrite_strategy();
    }

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);
      options.cached                                = parser.options.find("cached") != parser.options.end();
      options.global_cache                          = parser.options.find("global-cache") != parser.options.end();
      options.confluence                            = parser.options.find("confluence") != parser.options.end();
      options.one_point_rule_rewrite                = parser.options.find("no-one-point-rule-rewrite") == parser.options.end();
      options.replace_constants_by_variables        = parser.options.find("no-replace-constants-by-variables") == parser.options.end();
      options.resolve_summand_variable_name_clashes = parser.options.find("no-resolve-summand-variable-name-clashes") == parser.options.end();
      if (parser.options.find("out") != parser.options.end())
      {
        output_format = lts::detail::parse_format(parser.option_argument("out"));
        if (output_format == lts::lts_none)
        {
          parser.error("Format '" + parser.option_argument("out") + "' is not recognised.");
        }
      }
      if (output_format == lts::lts_none)
      {
        output_format = lts::detail::guess_format(output_filename());
        if (output_format == lts::lts_none)
        {
          mCRL2log(log::warning) << "no output format set or detected; using default (mcrl2)" << std::endl;
          output_format = lts::lts_lts;
        }
      }
    }

    std::unique_ptr<lts::lts_builder> create_builder()
    {
      switch (output_format)
      {
        case lts::lts_aut: return std::unique_ptr<lts::lts_builder>(new lts::lts_aut_builder());
        case lts::lts_dot: return std::unique_ptr<lts::lts_builder>(new lts::lts_dot_builder());
        case lts::lts_fsm: return std::unique_ptr<lts::lts_builder>(new lts::lts_fsm_builder());
        default: return std::unique_ptr<lts::lts_builder>(new lts::lts_lts_builder());
      }
    }

    bool run() override
    {
      mCRL2log(log::verbose) << options << std::endl;
      lps::specification lpsspec = lps::detail::load_lps(input_filename());
      std::unique_ptr<lts::lts_builder> builder = create_builder();
      lps::lps_explorer explorer(lpsspec, options);
      current_explorer = &explorer;
      explorer.generate_state_space(
        lps::skip(),
        [&](std::size_t from, const process::timed_multi_action& a, std::size_t to)
        {
          builder->add_transition(from, a, to);
        }
      );
      builder->finalize(explorer.state_map());
      builder->save(output_filename());
      return true;
    }

    void abort()
    {
      current_explorer->abort();
    }
};

std::unique_ptr<generatelts_tool> tool_instance;

void premature_termination_handler(int)
{
  // Reset signal handlers.
  signal(SIGABRT, nullptr);
  signal(SIGINT, nullptr);
  tool_instance->abort();
}

int main(int argc, char** argv)
{
  tool_instance = std::unique_ptr<generatelts_tool>(new generatelts_tool());
  signal(SIGABRT, premature_termination_handler);
  signal(SIGINT, premature_termination_handler); // At ^C invoke the termination handler.
  return tool_instance->execute(argc, argv);
}
