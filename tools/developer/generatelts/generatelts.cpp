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
#include <limits>
#include <memory>
#include <string>
#include <sstream>
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/explorer.h"
#include "mcrl2/lps/detail/lps_io.h"
#include "mcrl2/lts/lts_builder.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/state_space_generator.h"
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

  lps::explorer_options options;
  lts::lts_type output_format = lts::lts_none;
  lps::explorer* current_explorer = nullptr;

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
      desc.add_option("global-cache", "use a global cache");
      desc.add_option("no-one-point-rule-rewrite", "do not apply the one point rule rewriter");
      desc.add_option("no-replace-constants-by-variables", "do not move constant expressions to a substitution");
      desc.add_option("no-resolve-summand-variable-name-clashes", "do not resolve summand variable name clashes");

      // copied from lps2lts
      desc.add_option("cached", "use enumeration caching techniques to speed up state space generation. ");
      desc.add_option("max", utilities::make_mandatory_argument("NUM"), "explore at most NUM states", 'l');
      desc.add_option("nondeterminism", "detect nondeterministic states, i.e. states with outgoing transitions with the same label to different states. ", 'n');
      desc.add_option("deadlock", "detect deadlocks (i.e. for every deadlock a message is printed). ", 'D');
      desc.add_option("divergence",
                   "detect divergences (i.e. for every state with a divergence (=tau loop) a message is printed). "
                   "The algorithm to detect the divergences is linear for every state, "
                   "so state space exploration becomes quadratic with this option on, causing a state "
                   "space exploration to become slow when this option is enabled. ", 'F');
      desc.add_option("action", utilities::make_mandatory_argument("NAMES"),
                   "report whether an action from NAMES occurs in the transitions system, "
                   "where NAMES is a comma-separated list. A message "
                   "is printed for every occurrence of one of these action names. "
                   "With the -t flag traces towards these actions are generated. "
                   "When using -tN only N traces are generated after which the generation of the state space stops. ", 'a');
      desc.add_option("multiaction", utilities::make_mandatory_argument("NAMES"),
                 "detect and report multiactions in the transitions system "
                 "from NAMES, a comma-separated list. Works like -a, except that multi-actions "
                 "are matched exactly, including data parameters. ", 'm');
      desc.add_option("error-trace",
                 "if an error occurs during exploration, save a trace to the state that could "
                 "not be explored. ");
      desc.add_option("trace", utilities::make_optional_argument("NUM", std::to_string(std::numeric_limits<std::size_t>::max())),
                 "Write a shortest trace to each state that is reached with an action from NAMES "
                 "with the option --action, is a deadlock with the option --deadlock, is nondeterministic with the option --nondeterminism, or is a "
                 "divergence with the option --divergence to a file. "
                 "No more than NUM traces will be written. If NUM is not supplied the number of "
                 "traces is unbounded. "
                 "For each trace that is to be written a unique file with extension .trc (trace) "
                 "will be created containing a shortest trace from the initial state to the deadlock "
                 "state. The traces can be pretty printed and converted to other formats using tracepp. ", 't');
      desc.add_option("confluence", utilities::make_optional_argument("NAME", "ctau"),
                 "apply prioritization of transitions with the action label NAME. "
                 "(when no NAME is supplied (i.e., '-c') priority is given to the action 'ctau'. To give priority to "
                 "to tau use the flag -ctau. Note that if the linear process is not tau-confluent, the generated "
                 "state space is necessarily branching bisimilar to the state space of the lps. The generation "
                 "algorithm that is used does not require the linear process to be tau convergent. ", 'c');
      desc.add_option("out", utilities::make_mandatory_argument("FORMAT"), "save the output in the specified FORMAT. ", 'o');
      desc.add_option("tau", utilities::make_mandatory_argument("ACTNAMES"),
                 "consider actions with a name in the comma separated list ACTNAMES to be internal. "
                 "This list is only used and allowed when searching for divergencies. ");
      desc.add_option("strategy", utilities::make_enum_argument<lps::exploration_strategy>("NAME")
                   .add_value_short(lps::es_breadth, "b", true)
                   .add_value_short(lps::es_depth, "d")
        , "explore the state space using strategy NAME:"
        , 's');
      desc.add_option("suppress","in verbose mode, do not print progress messages indicating the number of visited states and transitions. "
                            "For large state spaces the number of progress messages can be quite "
                            "horrendous. This feature helps to suppress those. Other verbose messages, "
                            "such as the total number of states explored, just remain visible. ");
      options.rewrite_strategy = rewrite_strategy();
    }

    std::list<std::string> split_actions(const std::string& s)
    {
      std::size_t count = 0;
      std::string a;
      std::list<std::string> result;
      for (char ch: s)
      {
        if (ch == ',' && count == 0)
        {
          result.push_back(a);
          a.clear();
        }
        else
        {
          if (ch == '(')
          {
            ++count;
          }
          else if (ch == ')')
          {
            --count;
          }
          a.push_back(ch);
        }
      }
      if (!a.empty())
      {
        result.push_back(a);
      }
      return result;
    }

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);
      options.cached                                = parser.has_option("cached");
      options.global_cache                          = parser.has_option("global-cache");
      options.confluence                            = parser.has_option("confluence");
      options.one_point_rule_rewrite                = !parser.has_option("no-one-point-rule-rewrite");
      options.replace_constants_by_variables        = !parser.has_option("no-replace-constants-by-variables");
      options.resolve_summand_variable_name_clashes = !parser.has_option("no-resolve-summand-variable-name-clashes");
      options.detect_deadlock                       = parser.has_option("deadlock");
      options.detect_nondeterminism                 = parser.has_option("nondeterminism");
      options.detect_divergence                     = parser.has_option("divergence");
      options.save_error_trace                      = parser.has_option("error-trace");
      options.suppress_progress_messages            = parser.has_option("suppress");
      options.search_strategy = parser.option_argument_as<lps::exploration_strategy>("strategy");

      if (parser.options.count("max"))
      {
        options.max_states = parser.option_argument_as<std::size_t> ("max");
      }

      if (parser.has_option("out"))
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

      if (parser.has_option("action"))
      {
        options.detect_action = true;
        for (const std::string& s: split_actions(parser.option_argument("action")))
        {
          options.trace_actions.insert(core::identifier_string(s));
        }
      }

      if (parser.has_option("multiaction"))
      {
        std::list<std::string> actions = split_actions(parser.option_argument("multiaction"));
        options.trace_multiaction_strings.insert(actions.begin(), actions.end());
      }

      if (parser.has_option("trace"))
      {
        options.generate_traces = true;
        options.max_traces = parser.option_argument_as<std::size_t>("trace");
      }

      if (parser.has_option("tau"))
      {
        if (parser.options.count("divergence")==0)
        {
          parser.error("Option --tau requires the option --divergence.");
        }
        std::list<std::string> actions = split_actions(parser.option_argument("tau"));
        for (const std::string& s: actions)
        {
          options.actions_internal_for_divergencies.insert(core::identifier_string(s));
        }
      }

      if (parser.has_option("confluence"))
      {
        options.priority_action = parser.option_argument("confluence");
      }

      if (2 < parser.arguments.size())
      {
        parser.error("Too many file arguments.");
      }
    }

    std::unique_ptr<lts::lts_builder> create_builder(const lps::specification& lpsspec)
    {
      switch (output_format)
      {
        case lts::lts_aut: return std::unique_ptr<lts::lts_builder>(new lts::lts_aut_builder());
        case lts::lts_dot: return std::unique_ptr<lts::lts_builder>(new lts::lts_dot_builder(lpsspec.data(), lpsspec.action_labels()));
        case lts::lts_fsm: return std::unique_ptr<lts::lts_builder>(new lts::lts_fsm_builder(lpsspec.data(), lpsspec.action_labels()));
        default: return std::unique_ptr<lts::lts_builder>(new lts::lts_lts_builder(lpsspec.data(), lpsspec.action_labels()));
      }
    }

    bool run() override
    {
      mCRL2log(log::verbose) << options << std::endl;
      options.trace_prefix = input_filename();
      lps::specification lpsspec = lps::detail::load_lps(input_filename());
      std::unique_ptr<lts::lts_builder> builder = create_builder(lpsspec);
      lts::state_space_generator generator(lpsspec, options);
      current_explorer = &generator.explorer;
      generator.explore(*builder);
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
