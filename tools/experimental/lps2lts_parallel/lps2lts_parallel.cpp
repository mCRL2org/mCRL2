// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2lts_parallel.cpp
/// \brief This is an experimental tool to see whether a state space
///        can be generated in parallel. 

#include <csignal>
#include <memory>
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/is_stochastic.h"
#include "mcrl2/lts/lts_io.h"
#include "lts_builder.h"
#include "state_space_generator.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using data::tools::rewriter_tool;
using utilities::tools::input_output_tool;

class lps2lts_parallel_tool: public rewriter_tool<input_output_tool>
{
  typedef rewriter_tool<input_output_tool> super;

  lps::explorer_options options;
  lts::lts_type output_format = lts::lts_none;
  lps::abortable* current_explorer = nullptr;
  std::set<std::string> trace_multiaction_strings;

  public:
    lps2lts_parallel_tool()
      : super("lps2lts_parallel",
              "Jan Friso Groote",
              "generates an LTS from an LPS",
              "Transforms the LPS in INFILE and writes a corresponding LTS "
              " to OUTFILE. If OUTFILE is not present, stdout is used. If INFILE is not "
              " present, stdin is used."
             )
    {}

    void add_options(utilities::interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("out", utilities::make_mandatory_argument("FORMAT"), "save the output in the specified FORMAT. ", 'o');
      desc.add_option("threads", utilities::make_mandatory_argument("NUM"), "run exploration with NUM threads (default=1). ");
    }

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);
       options.search_strategy = lps::es_breadth;

      if (parser.has_option("out"))
      {
        output_format = lts::detail::parse_format(parser.option_argument("out"));
        if (output_format == lts::lts_none)
        {
          parser.error("Format '" + parser.option_argument("out") + "' is not recognised.");
        }
      }

      if (output_format == lts::lts_none && !output_filename().empty())
      {
        output_format = lts::detail::guess_format(output_filename());
        if (output_format == lts::lts_none)
        {
          mCRL2log(log::warning) << "no output format set or detected; using default (lts)" << std::endl;
          output_format = lts::lts_lts;
        }
      }

      if (parser.has_option("threads"))
      {
        options.number_of_threads = parser.option_argument_as<std::size_t>("threads");
        if (options.number_of_threads<1)
        {
          throw mcrl2::runtime_error("The number of threads should at least be 1.");
        }
      }
      else 
      {
        options.number_of_threads = 1;
      }

      options.rewrite_strategy = rewrite_strategy();
    }

    template <bool Stochastic, bool Timed, typename Specification, typename LTSBuilder>
    void generate_state_space(const Specification& lpsspec, LTSBuilder& builder)
    {
      lts::state_space_generator<Stochastic, Timed, Specification> generator(lpsspec, options);
      current_explorer = &generator.explorer;
      generator.explore(builder);
      builder.save(output_filename());
    }

    bool run() override
    {
      mCRL2log(log::verbose) << options << std::endl;
      options.trace_prefix = input_filename();
      lps::stochastic_specification stochastic_lpsspec;
      lps::load_lps(stochastic_lpsspec, input_filename());
      bool is_timed = stochastic_lpsspec.process().has_time();

      if (lps::is_stochastic(stochastic_lpsspec))
      {
        throw mcrl2::runtime_error("This experimental tool cannot handle stochastic processes.");
      }
      else
      {
        lps::specification lpsspec = lps::remove_stochastic_operators(stochastic_lpsspec);
        auto builder = create_lts_builder(lpsspec, options, output_format, output_filename());
        if (is_timed)
        {
          generate_state_space<false, true>(lpsspec, *builder);
        }
        else
        {
          generate_state_space<false, false>(lpsspec, *builder);
        }
      }
      return true;
    }

    void abort()
    {
      current_explorer->abort();
    }
};

std::unique_ptr<lps2lts_parallel_tool> tool_instance;

static void premature_termination_handler(int)
{
  // Reset signal handlers.
  signal(SIGABRT, nullptr);
  signal(SIGINT, nullptr);
  tool_instance->abort();
}

int main(int argc, char** argv)
{
  tool_instance = std::make_unique<lps2lts_parallel_tool>();
  signal(SIGABRT, premature_termination_handler);
  signal(SIGINT, premature_termination_handler); // At ^C invoke the termination handler.
  return tool_instance->execute(argc, argv);
}
