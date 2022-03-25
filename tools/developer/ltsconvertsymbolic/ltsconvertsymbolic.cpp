// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/input_output_tool.h"

#include "mcrl2/lps/symbolic_lts_io.h"
#include "mcrl2/lps/state.h"
#include "mcrl2/lts/lts_builder.h"
#include "mcrl2/lts/state_space_generator.h"
#include "mcrl2/symbolic/print.h"
#include "mcrl2/utilities/logger.h"

using namespace mcrl2;
using namespace mcrl2::lts;
using namespace mcrl2::lts::detail;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::log;
using namespace sylvan::ldds;

using mcrl2::utilities::tools::input_output_tool;

/// \brief Converts a state vector of indices to a vector of the corresponding data expressions. 
lps::state array2state(const std::vector<symbolic::data_expression_index>& data_index, std::uint32_t* x, std::size_t n)
{
  std::vector<data::data_expression> result;
  for (std::size_t i = 0; i < n; i++)
  {
    if (x[i] == symbolic::relprod_ignore)
    {
      result.push_back(data::undefined_data_expression());
    }
    else
    {
      result.push_back(data_index[i][x[i]]);
    }
  }
  
  return lps::state(result.begin(), n);
}

/// \brief Explore all outgoing transitions for one state vector.
template <typename Context>
void explore_transitions_callback(WorkerP*, Task*, std::uint32_t* x, std::size_t n, void* context);

/// \brief Explore all outgoing transitions per state vector.
template <typename Context>
void explore_state_callback(WorkerP*, Task*, std::uint32_t* x, std::size_t n, void* context);

class convert_concrete_lts
{
  public:
    convert_concrete_lts(const lps::symbolic_lts& lts, std::unique_ptr<lts_builder> builder)
      : m_lts(lts), m_builder(std::move(builder)), m_progress_monitor(mcrl2::lps::exploration_strategy::es_none)
    {
      m_number_of_states = satcount(m_lts.states);
    }

    void run()
    {
      for (const auto& group : m_lts.summand_groups)
      {
        if (group.summands.size() > 1)
        {
          throw mcrl2::runtime_error("Cannot convert a symbolic LTS with non-trivial transition groups");
        }
      }

      // Explore all states in the LDD.
      sat_all_nopar(m_lts.states, explore_state_callback<convert_concrete_lts>, this);

      m_progress_monitor.finish_exploration(m_discovered.size());
      m_builder->finalize(m_discovered, false);
    }

    void save(const std::string& filename)
    {
      m_builder->save(filename);
    }

    const lps::symbolic_lts& m_lts;
    std::unique_ptr<lts_builder> m_builder;
    utilities::indexed_set<lps::state> m_discovered;
    mcrl2::lts::detail::progress_monitor m_progress_monitor;
    std::size_t m_number_of_states;
};

/// \brief std::tuple is completely useless due to the cryptic compilation errors.
struct callback_context
{
  convert_concrete_lts& algorithm;
  std::uint32_t* state;
  std::size_t state_size;
  std::size_t state_index;
  const lps::lps_summand_group& group;
};

template <typename Context>
void explore_transitions_callback(WorkerP*, Task*, std::uint32_t* x, std::size_t n, void* context)
{
  auto pointer = reinterpret_cast<Context*>(context);
  auto& p = *pointer;

  // Try to match the read parameters.
  for (std::size_t i = 0; i < p.group.read.size(); ++i)
  {
    if (p.state[p.group.read[i]] != x[p.group.read_pos[i]])
    {
      return;
    }
  }

  // Apply the writes to the state vector.  
  MCRL2_DECLARE_STACK_ARRAY(target, std::uint32_t, p.state_size);
  for (std::size_t i = 0; i < p.state_size; ++i)
  {
    target[i] = p.state[i];
  }

  for (std::size_t i = 0; i < p.group.write.size(); ++i)
  {
    target[p.group.write[i]] = x[p.group.write_pos[i]];
  }
  
  lps::multi_action action_label = p.algorithm.m_lts.action_index[x[n - 1]];
  lps::state target_state(array2state(p.algorithm.m_lts.data_index, target.data(), p.state_size));
  std::size_t target_index = p.algorithm.m_discovered.insert(target_state).first;

  p.algorithm.m_progress_monitor.examine_transition();
  p.algorithm.m_builder->add_transition(p.state_index, action_label, target_index);
}

template <typename Context>
void explore_state_callback(WorkerP*, Task*, std::uint32_t* x, std::size_t n, void* context)
{
  auto p = reinterpret_cast<Context*>(context);
  auto& algorithm = *p;

  lps::state current(array2state(algorithm.m_lts.data_index, x, n));
  auto [current_index, _] = algorithm.m_discovered.insert(current);

  for (const lps::lps_summand_group& group : algorithm.m_lts.summand_groups)
  {
    // Find all outgoing transitions of this state.
    callback_context context { algorithm, x, n, current_index, group };
    sat_all_nopar(group.L, explore_transitions_callback<callback_context>, &context);
  }

  algorithm.m_progress_monitor.finish_state(algorithm.m_discovered.size(), algorithm.m_number_of_states - algorithm.m_discovered.size());
}

class ltsconvert_tool : public input_output_tool
{
    typedef input_output_tool super;

    // Lace options
    std::size_t lace_n_workers = 1;
    std::size_t lace_dqsize = 1024*1024*4; // set large default
    std::size_t lace_stacksize = 0; // use default

    // Sylvan options
    std::size_t memory_limit = 3;
    std::size_t initial_ratio = 16;
    std::size_t table_ratio = 1;

  public:
    ltsconvert_tool() :
      input_output_tool("ltsconvertsymbolic", "Maurice Laveaux",
                      "applies various conversions to symbolic LTSs",
                      "")
    {}
    
    void add_options(utilities::interface_description& desc) override
    {
      super::add_options(desc);

      desc.add_option("lace-workers", utilities::make_optional_argument("NUM", "1"), "set number of Lace workers (threads for parallelization), (0=autodetect, default 1)");
      desc.add_option("lace-dqsize", utilities::make_optional_argument("NUM", "4194304"), "set length of Lace task queue (default 1024*1024*4)");
      desc.add_option("lace-stacksize", utilities::make_optional_argument("NUM", "0"), "set size of program stack in kilobytes (0=default stack size)");
      desc.add_option("memory-limit", utilities::make_optional_argument("NUM", "3"), "Sylvan memory limit in gigabytes (default 3)", 'm');

      desc.add_option("out", utilities::make_mandatory_argument("FORMAT"), "use FORMAT as the output format.", 'o');
    }

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);

      if (parser.has_option("lace-workers"))
      {
        lace_n_workers = parser.option_argument_as<int>("lace-workers");
      }
      if (parser.has_option("lace-dqsize"))
      {
        lace_dqsize = parser.option_argument_as<int>("lace-dqsize");
      }
      if (parser.has_option("lace-stacksize"))
      {
        lace_stacksize = parser.option_argument_as<int>("lace-stacksize");
      }
      if (parser.has_option("memory-limit"))
      {
        memory_limit = parser.option_argument_as<std::size_t>("memory-limit");
      }
      if (parser.has_option("initial-ratio"))
      {
        initial_ratio = parser.option_argument_as<std::size_t>("initial-ratio");
        if (!utilities::is_power_of_two(initial_ratio))
        {
          throw mcrl2::runtime_error("The initial-ratio should be a power of two.");
        }
      }
      if (parser.has_option("table-ratio"))
      {
        table_ratio = parser.option_argument_as<std::size_t>("table-ratio");
        if (!utilities::is_power_of_two(table_ratio))
        {
          throw mcrl2::runtime_error("The table-ratio should be a power of two.");
        }
      }
      if (parser.options.count("out"))
      {
        if (1 < parser.options.count("out"))
        {
          mCRL2log(warning) << "multiple output formats specified; can only use one\n";
        }

        outtype = mcrl2::lts::detail::parse_format(parser.option_argument("out"));

        if (outtype == lts_none)
        {
          mCRL2log(warning) << "format '" << parser.option_argument("out") <<
                    "' is not recognised; option ignored" << std::endl;
        }
      }

      if (2 < parser.arguments.size())
      {
        parser.error("too many file arguments");
      }
    }
    
    bool run() override
    {      
      lace_init(lace_n_workers, lace_dqsize);
      lace_startup(lace_stacksize, nullptr, nullptr);
      sylvan::sylvan_set_limits(memory_limit * 1024 * 1024 * 1024, std::log2(table_ratio), std::log2(initial_ratio));
      sylvan::sylvan_init_package();
      sylvan::sylvan_init_ldd();

      if (input_filename().empty() || input_filename() == "-")
      {
        std::cin >> m_input;
      }
      else
      {
        std::ifstream ifs(input_filename(), std::ifstream::in | std::ios_base::binary);
        if (!ifs.good())
        {
          throw mcrl2::runtime_error("Could not open file " + input_filename() + ".");
        }
        ifs >> m_input;
      }

      lps::specification lpsspec;
      lps::explorer_options options;
      options.save_at_end = false;
      
      if (outtype == lts_none)
      {
        mCRL2log(verbose) << "Trying to detect output format by extension..." << std::endl;

        outtype = mcrl2::lts::detail::guess_format(output_filename(), true);
      }
      
      std::unique_ptr<lts_builder> builder = create_lts_builder(lpsspec, options, outtype, output_filename());

      convert_concrete_lts algorithm(m_input, std::move(builder));
      algorithm.run();
      algorithm.save(output_filename());

      sylvan::sylvan_quit();
      lace_exit();
      return true;
    }

  private:
    lts_type outtype = lts_none;

    lps::symbolic_lts m_input;
};

int main(int argc, char* argv[])
{
  return ltsconvert_tool().execute(argc, argv);
}