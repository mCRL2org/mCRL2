// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file generatelts.cpp

#include <csignal>
#include <memory>
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/is_stochastic.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/stochastic_lts_builder.h"
#include "mcrl2/lts/state_space_generator.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using data::tools::rewriter_tool;
using utilities::tools::input_output_tool;

class generatelts_tool: public rewriter_tool<input_output_tool>
{
  typedef rewriter_tool<input_output_tool> super;

  lps::explorer_options options;
  lts::lts_type output_format = lts::lts_none;
  lps::abortable* current_explorer = nullptr;
  std::set<std::string> trace_multiaction_strings;

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
      desc.add_hidden_option("global-cache", "use a global cache");
      desc.add_hidden_option("no-remove-unused-rewrite-rules", "do not remove unused rewrite rules. ", 'u');
      desc.add_hidden_option("no-one-point-rule-rewrite", "do not apply the one point rule rewriter");
      desc.add_hidden_option("no-replace-constants-by-variables", "do not move constant expressions to a substitution");
      desc.add_option("no-probability-checking", "do not check if probabilities in stochastic specifications have sensible values");
      desc.add_hidden_option("dfs-recursive", "use recursive depth first search for divergence detection");
      desc.add_option("cached", "use enumeration caching techniques to speed up state space generation. ");
      desc.add_option("todo-max", utilities::make_mandatory_argument("NUM"),
                 "keep at most NUM states in the todo list; this option is only relevant for "
                 "highway search, where NUM is the maximum number of states per level. ");
      desc.add_option("nondeterminism", "report nondeterministic states, i.e. states with outgoing transitions"
                 " with the same label to different states. The flag --trace can be used to generate traces to these nondeterministic states.", 'n');
      desc.add_option("deadlock", "report deadlocks (i.e. states with no outgoing transitions). "
                 "The flag --trace can be used to generate traces to these deadlocks.", 'D');
      desc.add_option("divergence",
                   "report divergences (i.e. states with a tau loop). "
                   "The algorithm to detect divergences is linear for every state, "
                   "so state space exploration becomes quadratic with this option enabled."
                   "The flag --trace can be used to generate traces to these divergences.", 'F');
      desc.add_option("action", utilities::make_mandatory_argument("NAMES"),
                   "report actions in the transition system that occur in the "
                   "comma-separated list of action names NAMES. The flag --trace can be used to generate traces to these actions.",
                   'a');
      desc.add_option("multiaction", utilities::make_mandatory_argument("NAMES"),
                 "report occurrences of multi-actions in the transition system that occur in "
                 "the comma-separated list of actions NAMES. Works like --action, except that multi-actions "
                 "are matched exactly, including data parameters. The flag --trace can be used to generate traces "
                 "to these multi-actions.", 'm');
      desc.add_hidden_option("error-trace",
                 "if an error occurs during exploration, save a trace to the state that could "
                 "not be explored.");
      desc.add_option("trace", utilities::make_optional_argument("NUM", std::to_string(std::numeric_limits<std::size_t>::max())),
                 "write a trace to states that are reported using one of the flags "
                 "--action, --deadlock, --divergence, --multiaction or --nondeterminism. "
                 "No more than NUM traces will be written for each flag. If NUM is not supplied"
                 " the number of traces is unbounded. "
                 "For each trace a unique file with extension .trc (trace) "
                 "will be created containing a shortest trace starting from the initial state. "
                 "The traces can be pretty printed and converted to other formats using tracepp. "
                 "If the maximum amount of traces has been reached, state space generation is aborted. "
                 "There is no guarantee that the traces are fully contained in the partial state space."
                 , 't');
      desc.add_option("confluence", utilities::make_optional_argument("NAME", "ctau"),
                 "apply prioritization of transitions with the action label NAME (default 'ctau'). "
                 "To give priority "
                 "to tau use the flag -ctau. Only if the linear process is tau-confluent, the generated "
                 "state space is branching bisimilar to the state space of the lps. The generation "
                 "algorithm that is used does not require the linear process to be tau convergent. ", 'c');
      desc.add_option("out", utilities::make_mandatory_argument("FORMAT"), "save the output in the specified FORMAT. ", 'o');
      desc.add_option("tau", utilities::make_mandatory_argument("NAMES"),
                 "consider actions that occur in the comma-separated list of action names "
                 "NAMES to be internal. This setting only affects the option --divergence.");
      desc.add_option("strategy", utilities::make_enum_argument<lps::exploration_strategy>("NAME")
                   .add_value_short(lps::es_breadth, "b", true)
                   .add_value_short(lps::es_depth, "d")
                   .add_value_short(lps::es_highway, "h")
        , "explore the state space using strategy NAME:"
        , 's');
      desc.add_option("suppress","in verbose mode, do not print progress messages indicating the number of visited states and transitions.");
      desc.add_option("save-at-end", "delay saving of the generated LTS until the end. "
                 "This option only applies to .aut files, which are by default saved on the fly.");
      desc.add_option("no-info", "do not add state label information to OUTFILE. This option only applies to .lts files.");
    }

    static std::list<std::string> split_actions(const std::string& s)
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

    void parse_trace_multiactions(const data::data_specification& dataspec, const process::action_label_list& action_labels)
    {
      for (const std::string& s: trace_multiaction_strings)
      {
        try
        {
          options.trace_multiactions.insert(mcrl2::lps::parse_multi_action(s, action_labels, dataspec));
        }
        catch (mcrl2::runtime_error& e)
        {
          throw mcrl2::runtime_error(std::string("Multi-action ") + s + " does not exist: " + e.what());
        }
        mCRL2log(log::verbose) << "Checking for action \"" << s << "\"\n";
      }
      if (options.detect_action)
      {
        for (const mcrl2::core::identifier_string& ta: options.trace_actions)
        {
          bool found = (std::string(ta) == "tau");
          for(const process::action_label& al: action_labels)
          {
            if (al.name() == ta)
            {
              found = true;
              break;
            }
          }
          if (!found)
          {
            throw mcrl2::runtime_error(std::string("Action label ") + core::pp(ta) + " is not declared.");
          }
          else
          {
            mCRL2log(log::verbose) << "Checking for action " << ta << "\n";
          }
        }
      }
      for (const mcrl2::core::identifier_string& ta: options.actions_internal_for_divergencies)
      {
        mcrl2::process::action_label_list::iterator it = action_labels.begin();
        bool found = (std::string(ta) == "tau");
        while (!found && it != action_labels.end())
        {
          found = (it++->name() == ta);
        }
        if (!found)
        {
          throw mcrl2::runtime_error(std::string("Action label ") + core::pp(ta) + " is not declared.");
        }
      }
    }

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);
      options.save_at_end                       = parser.has_option("save-at-end");
      options.cached                                = parser.has_option("cached");
      options.global_cache                          = parser.has_option("global-cache");
      options.confluence                            = parser.has_option("confluence");
      options.one_point_rule_rewrite                = !parser.has_option("no-one-point-rule-rewrite");
      options.remove_unused_rewrite_rules           = !parser.has_option("no-remove-unused-rewrite-rules");
      options.replace_constants_by_variables        = !parser.has_option("no-replace-constants-by-variables");
      options.check_probabilities                   = !parser.has_option("no-probability-checking");
      options.detect_deadlock                       = parser.has_option("deadlock");
      options.detect_nondeterminism                 = parser.has_option("nondeterminism");
      options.detect_divergence                     = parser.has_option("divergence");
      options.save_error_trace                      = parser.has_option("error-trace");
      options.suppress_progress_messages            = parser.has_option("suppress");
      options.dfs_recursive                         = parser.has_option("dfs-recursive");
      options.discard_lts_state_labels              = parser.has_option("no-info");
      options.search_strategy = parser.option_argument_as<lps::exploration_strategy>("strategy");

      // highway search
      if (parser.has_option("todo-max"))
      {
        options.highway_todo_max = parser.option_argument_as<std::size_t>("todo-max");
      }
      if (options.search_strategy == lps::es_highway && !parser.has_option("todo-max"))
      {
        parser.error("Search strategy 'highway' requires that the option todo-max is set");
      }
      if (options.search_strategy != lps::es_highway && parser.has_option("todo-max"))
      {
        parser.error("Option 'todo-max' can only be used in combination with highway search");
      }

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
        trace_multiaction_strings.insert(actions.begin(), actions.end());
      }

      if (parser.has_option("trace"))
      {
        options.generate_traces = true;
        options.max_traces = parser.option_argument_as<std::size_t>("trace");
      }

      if (parser.has_option("tau"))
      {
        if (!parser.has_option("divergence"))
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
        options.confluence_action = parser.option_argument("confluence");
      }

      if (2 < parser.arguments.size())
      {
        parser.error("Too many file arguments.");
      }

      options.rewrite_strategy = rewrite_strategy();

      if (options.save_at_end && (output_filename().empty() || output_format != lts::lts_aut))
      {
        parser.error("Option '--save-at-end' requires that the output is in .aut format.");
      }

      if (options.discard_lts_state_labels && (output_filename().empty() || output_format != lts::lts_lts))
      {
        parser.error("Option '--no-info' requires that the output is in .lts format.");
      }
    }

    std::unique_ptr<lts::lts_builder> create_lts_builder(const lps::specification& lpsspec)
    {
      switch (output_format)
      {
        case lts::lts_aut:
          {
            return options.save_at_end ? std::unique_ptr<lts::lts_builder>(new lts::lts_aut_disk_builder(output_filename()))
                                           : std::unique_ptr<lts::lts_builder>(new lts::lts_aut_builder());
          }
        case lts::lts_dot: return std::make_unique<lts::lts_dot_builder>(lpsspec.data(), lpsspec.action_labels(), lpsspec.process().process_parameters());
        case lts::lts_fsm: return std::make_unique<lts::lts_fsm_builder>(lpsspec.data(), lpsspec.action_labels(), lpsspec.process().process_parameters());
        case lts::lts_lts: return std::make_unique<lts::lts_lts_builder>(lpsspec.data(), lpsspec.action_labels(), lpsspec.process().process_parameters(), options.discard_lts_state_labels);
        default: return std::make_unique<lts::lts_none_builder>();
      }
    }

    std::unique_ptr<lts::stochastic_lts_builder> create_stochastic_lts_builder(const lps::stochastic_specification& lpsspec)
    {
      switch (output_format)
      {
        case lts::lts_aut: return std::make_unique<lts::stochastic_lts_aut_builder>();
        case lts::lts_lts: return std::make_unique<lts::stochastic_lts_lts_builder>(lpsspec.data(), lpsspec.action_labels(), lpsspec.process().process_parameters(), options.discard_lts_state_labels);
        case lts::lts_fsm: return std::make_unique<lts::stochastic_lts_fsm_builder>(lpsspec.data(), lpsspec.action_labels(), lpsspec.process().process_parameters());
        default: return std::make_unique<lts::stochastic_lts_none_builder>();
      }
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
      if (!trace_multiaction_strings.empty())
      {
        parse_trace_multiactions(stochastic_lpsspec.data(), stochastic_lpsspec.action_labels());
      }
      bool is_timed = stochastic_lpsspec.process().has_time();

      if (lps::is_stochastic(stochastic_lpsspec))
      {
        auto builder = create_stochastic_lts_builder(stochastic_lpsspec);
        if (is_timed)
        {
          generate_state_space<true, true>(stochastic_lpsspec, *builder);
        }
        else
        {
          generate_state_space<true, false>(stochastic_lpsspec, *builder);
        }
      }
      else
      {
        lps::specification lpsspec = lps::remove_stochastic_operators(stochastic_lpsspec);
        auto builder = create_lts_builder(lpsspec);
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

std::unique_ptr<generatelts_tool> tool_instance;

static void premature_termination_handler(int)
{
  // Reset signal handlers.
  signal(SIGABRT, nullptr);
  signal(SIGINT, nullptr);
  tool_instance->abort();
}

int main(int argc, char** argv)
{
  tool_instance = std::make_unique<generatelts_tool>();
  signal(SIGABRT, premature_termination_handler);
  signal(SIGINT, premature_termination_handler); // At ^C invoke the termination handler.
  return tool_instance->execute(argc, argv);
}
