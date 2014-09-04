// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <time.h>

#include "mcrl2/atermpp/aterm_balanced_tree.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lps/resolve_name_clashes.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"
#include "mcrl2/lts/detail/exploration.h"
#include "mcrl2/lts/lts_io.h"
#include <iomanip>

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::lps;
using namespace mcrl2::lts;

bool lps2lts_algorithm::initialise_lts_generation(lts_generation_options *options)
{
  m_options=*options;

  assert(!(m_options.bithashing && m_options.outformat != lts_aut && m_options.outformat != lts_none));

  if (m_options.bithashing)
  {
    m_bit_hash_table = bit_hash_table(m_options.bithashsize);
  }
  else
  {
    m_state_numbers = atermpp::indexed_set<storage_state_t>(m_options.initial_table_size, 50);
  }

  m_num_states = 0;
  m_num_transitions = 0;
  m_level = 1;
  m_traces_saved = 0;

  m_maintain_traces = m_options.trace || m_options.save_error_trace;
  m_value_prioritize = (m_options.expl_strat == es_value_prioritized || m_options.expl_strat == es_value_random_prioritized);

  lps::specification specification(m_options.specification);
  resolve_summand_variable_name_clashes(specification);

  if (m_options.outformat == lts_aut)
  {
    mCRL2log(verbose) << "writing state space in AUT format to '" << m_options.lts << "'." << std::endl;
    m_aut_file.open(m_options.lts.c_str());
    if (!m_aut_file.is_open())
    {
      mCRL2log(error) << "cannot open '" << m_options.lts << "' for writing" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  else if (m_options.outformat == lts_none)
  {
    mCRL2log(verbose) << "not saving state space." << std::endl;
  }
  else
  {
    mCRL2log(verbose) << "writing state space in " << mcrl2::lts::detail::string_for_type(m_options.outformat)
                      << " format to '" << m_options.lts << "'." << std::endl;
    m_output_lts.set_data(specification.data());
    m_output_lts.set_process_parameters(specification.process().process_parameters());
    m_output_lts.set_action_labels(specification.action_labels());
  }


  if (m_options.usedummies)
  {
    lps::detail::instantiate_global_variables(specification);
  }

  data::rewriter rewriter;
  if (m_options.removeunused)
  {
    mCRL2log(verbose) << "removing unused parts of the data specification." << std::endl;
    std::set<data::function_symbol> extra_function_symbols = lps::find_function_symbols(specification);

    if (m_options.expl_strat == es_value_prioritized || m_options.expl_strat == es_value_random_prioritized)
    {
      extra_function_symbols.insert(data::greater(data::sort_nat::nat()));
      extra_function_symbols.insert(data::equal_to(data::sort_nat::nat()));
    }

    rewriter = data::rewriter(specification.data(), data::used_data_equation_selector(specification.data(), extra_function_symbols, specification.global_variables()), m_options.strat);
  }
  else
  {
    rewriter = data::rewriter(specification.data(), m_options.strat);
  }

  action_summand_vector prioritised_summands;
  action_summand_vector nonprioritised_summands;
  if (m_options.priority_action != "")
  {
    mCRL2log(verbose) << "applying confluence reduction with tau action '" << m_options.priority_action << "'..." << std::endl;

    for (action_summand_vector::iterator i = specification.process().action_summands().begin(); i != specification.process().action_summands().end(); i++)
    {
      if ((m_options.priority_action == "tau" && i->is_tau()) ||
          (i->multi_action().actions().size() == 1 && m_options.priority_action == (std::string)i->multi_action().actions().front().label().name()))
      {
        prioritised_summands.push_back(*i);
      }
      else
      {
        nonprioritised_summands.push_back(*i);
      }
    }

    m_use_confluence_reduction = true;
  }
  else
  {
    m_use_confluence_reduction = false;
  }

  action_summand_vector tau_summands;
  if (m_options.detect_divergence)
  {
    mCRL2log(verbose) << "Detect divergences with tau action is `tau'.\n";
    for (size_t i = 0; i < specification.process().action_summands().size(); i++)
    {
      if (specification.process().action_summands()[i].is_tau())
      {
        tau_summands.push_back(specification.process().action_summands()[i]);
      }
    }
  }

  if (m_options.detect_action)
  {
    m_detected_action_summands.reserve(specification.process().action_summands().size());
    for (size_t i = 0; i < specification.process().action_summands().size(); i++)
    {
      bool found = false;
      for (auto j = specification.process().action_summands()[i].multi_action().actions().begin(); j != specification.process().action_summands()[i].multi_action().actions().end(); j++)
      {
        if (m_options.trace_actions.count(j->label().name()) > 0)
        {
          found = true;
          break;
        }
      }
      m_detected_action_summands.push_back(found);
    }
  }

  bool compute_actions = m_options.outformat != lts_none || m_options.detect_action || m_maintain_traces || m_value_prioritize;
  if (!compute_actions)
  {
    for (size_t i = 0; i < specification.process().action_summands().size(); i++)
    {
      specification.process().action_summands()[i].multi_action().actions() = process::action_list();
    }

    if (m_use_confluence_reduction)
    {
      for (size_t i = 0; i < nonprioritised_summands.size(); i++)
      {
        nonprioritised_summands[i].multi_action().actions() = process::action_list();
      }
      for (size_t i = 0; i < prioritised_summands.size(); i++)
      {
        prioritised_summands[i].multi_action().actions() = process::action_list();
      }
    }
  }

  m_generator = new next_state_generator(specification, rewriter, m_options.use_enumeration_caching, m_options.use_summand_pruning);

  if (m_use_confluence_reduction)
  {
    m_nonprioritized_subset = next_state_generator::summand_subset_t(m_generator, nonprioritised_summands, m_options.use_summand_pruning);
    m_prioritized_subset = next_state_generator::summand_subset_t(m_generator, prioritised_summands, m_options.use_summand_pruning);
    m_main_subset = &m_nonprioritized_subset;
  }
  else
  {
    m_main_subset = &m_generator->full_subset();
  }

  if (m_options.detect_divergence)
  {
    m_tau_summands = next_state_generator::summand_subset_t(m_generator, tau_summands, m_options.use_summand_pruning);
  }

  if (m_options.detect_deadlock)
  {
    mCRL2log(verbose) << "Detect deadlocks.\n" ;
  }

  return true;
}

bool lps2lts_algorithm::generate_lts()
{
  storage_state_t initial_state = m_generator->initial_state();
  m_initial_state_number=0;
  if (m_use_confluence_reduction)
  {
    initial_state = get_prioritised_representative(initial_state);
  }

  if (m_options.bithashing)
  {
    const std::pair<size_t, bool> p=m_bit_hash_table.add_state(initial_state);
    m_initial_state_number=p.first;
    assert(p.second); // The initial state is new.
  }
  else
  {
    m_state_numbers.put(initial_state);
  }

  if (m_options.outformat == lts_aut)
  {
    // HACK: this line will be overwritten once generation is finished.
    m_aut_file << "                                                             " << std::endl;
  }
  else if (m_options.outformat != lts_none)
  {
    m_initial_state_number = m_output_lts.add_state(m_generator->initial_state());
    m_output_lts.set_initial_state(m_initial_state_number);
  }
  m_num_states = 1;

  mCRL2log(verbose) << "generating state space with '" << m_options.expl_strat << "' strategy...\n";

  if (m_options.max_states == 0)
  {
    return true;
  }

  if (m_options.expl_strat == es_breadth || m_options.expl_strat == es_value_prioritized)
  {
    if (m_options.bithashing)
    {
      generate_lts_breadth_bithashing(initial_state);
    }
    else
    {
      if (m_options.todo_max==std::string::npos)
      { 
        generate_lts_breadth_todo_max_is_npos();
      }
      else 
      { 
        generate_lts_breadth_todo_max_larger_than_0(initial_state);
      }
    }

    mCRL2log(verbose) << "done with state space generation ("
                      << m_level-1 << " level" << ((m_level==2)?"":"s") << ", "
                      << m_num_states << " state" << ((m_num_states == 1)?"":"s")
                      << " and " << m_num_transitions << " transition" << ((m_num_transitions==1)?"":"s") << ")" << std::endl;
  }
  else if (m_options.expl_strat == es_depth)
  {
    generate_lts_depth(initial_state);

    mCRL2log(verbose) << "done with state space generation ("
                      << m_num_states << " state" << ((m_num_states == 1)?"":"s")
                      << " and " << m_num_transitions << " transition" << ((m_num_transitions==1)?"":"s") << ")" << std::endl;
  }
  else if (m_options.expl_strat == es_random || m_options.expl_strat == es_value_random_prioritized)
  {
    // In order to guarantee that time is different from a previous run to initialise
    // the seed of a previous run and not get the same results, we wait until time gets
    // a fresh value. It is better to replace this by c++11 random generators, as this
    // is an awkward solution.
    unsigned t=time(NULL);
    for( ; t==time(NULL) ; ); // Wait until time changes.
    srand((unsigned)time(NULL));

    generate_lts_random(initial_state);

    mCRL2log(verbose) << "done with random walk of "
                      << m_num_transitions << " transition" << ((m_num_transitions==1)?"":"s")
                      << " (visited " << m_num_states
                      << " unique state" << ((m_num_states == 1)?"":"s") << ")" << std::endl;
  }
  else
  {
    mCRL2log(error) << "unknown exploration strategy" << std::endl;
    return false;
  }

  return true;
}

bool lps2lts_algorithm::finalise_lts_generation()
{
  if (m_options.outformat == lts_aut)
  {
    m_aut_file.flush();
    m_aut_file.seekp(0);
    m_aut_file << "des (" << m_initial_state_number << "," << m_num_transitions << "," << m_num_states << ")";
    m_aut_file.close();
  }
  else if (m_options.outformat != lts_none)
  {
    if (!m_options.outinfo)
    {
      m_output_lts.clear_state_labels();
    }

    switch (m_options.outformat)
    {
      case lts_lts:
      {
        m_output_lts.save(m_options.lts);
        break;
      }
      case lts_fsm:
      {
        lts_fsm_t fsm;
        detail::lts_convert(m_output_lts, fsm);
        fsm.save(m_options.lts);
        break;
      }
#ifdef USE_BCG
      case lts_bcg:
      {
        lts_bcg_t bcg;
        detail::lts_convert(m_output_lts, bcg);
        bcg.save(m_options.lts);
        break;
      }
#endif
      case lts_dot:
      {
        lts_dot_t dot;
        detail::lts_convert(m_output_lts, dot);
        dot.save(m_options.lts);
        break;
      }
      default:
        assert(0);
    }
  }

  return true;
}

// Confluence reduction based on S.C.C. Blom, Partial tau-confluence for
// Efficient State Space Generation, Technical Report SEN-R0123, CWI, Amsterdam, 2001

lps2lts_algorithm::storage_state_t lps2lts_algorithm::get_prioritised_representative(const storage_state_t& state1)
{
  storage_state_t state=state1;
  assert(m_use_confluence_reduction);

  std::map<storage_state_t, size_t> number;
  std::map<storage_state_t, size_t> low;
  std::map<storage_state_t, std::list<storage_state_t> > next;
  std::map<storage_state_t, storage_state_t> back;
  next_state_generator::enumerator_queue_t enumeration_queue;

  size_t count = 0;
  number[state] = 0;

  while (true)
  {
    assert(number.count(state) > 0);
    if (number[state] == 0)
    {
      count++;
      number[state] = count;
      low[state] = count;
      next[state] = std::list<storage_state_t>();

      enumeration_queue.clear();
      for (next_state_generator::iterator i = m_generator->begin(state, m_prioritized_subset, &enumeration_queue); i; i++)
      {
        const storage_state_t s=i->internal_state();
        next[state].push_back(s);
        if (number.count(s) == 0)
        {
          number[s] = 0;
        }
      }
    }
    assert(next.count(state) > 0);
    if (next[state].empty())
    {
      assert(low.count(state) > 0);
      if (number[state] == low[state])
      {
        return state;
      }
      assert(back.count(state) > 0);
      storage_state_t back_state = back[state];
      low[back_state] = low[back_state] < low[state] ? low[back_state] : low[state];
      state = back_state;
    }
    else
    {
      storage_state_t next_state = next[state].front();
      next[state].pop_front();
      if (number[next_state] == 0)
      {
        back[next_state] = state;
        state = next_state;
      }
      else if (number[next_state] < number[state])
      {
        low[state] = low[state] < number[next_state] ? low[state] : number[next_state];
      }
    }
  }
}

void lps2lts_algorithm::value_prioritize(std::vector<next_state_generator::transition_t>& transitions)
{
  data::data_expression lowest_value;

  for (std::vector<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
  {
    if (i->action().actions().size() == 1 && i->action().actions().front().arguments().size() > 0)
    {
      const data::data_expression& argument = i->action().actions().front().arguments().front();
      if (mcrl2::data::sort_nat::is_nat(argument.sort()))
      {
        if (lowest_value==data::data_expression()) // lowest value is undefined.
        {
          lowest_value = argument;
        }
        else
        {
          const data::data_expression result = m_generator->get_rewriter()(data::greater(lowest_value, argument));

          if (data::sort_bool::is_true_function_symbol(result))
          {
            lowest_value = argument;
          }
          else if (!data::sort_bool::is_false_function_symbol(result))
          {
            throw mcrl2::runtime_error("Fail to rewrite term " + data::pp(result) + " to true or false.");
          }
        }
      }
    }
  }

  size_t new_position=0;
  for (std::vector<next_state_generator::transition_t>::const_iterator i = transitions.begin(); i != transitions.end(); ++i)
  {
    if (i->action().actions().size() != 1)
    {
      transitions[new_position++]= *i;
    }
    else if (i->action().actions().front().arguments().size() == 0)
    {
      transitions[new_position++]= *i;
    }
    else if (!mcrl2::data::sort_nat::is_nat(i->action().actions().front().arguments().front().sort()))
    {
      transitions[new_position++]= *i;
    }
    else
    {
      data::data_expression result = m_generator->get_rewriter()(data::equal_to(lowest_value, i->action().actions().front().arguments().front()));

      if (data::sort_bool::is_true_function_symbol(result))
      {
        transitions[new_position++]= *i;
      }
      else if (data::sort_bool::is_false_function_symbol(result))
      {
        // Skip the transition at position i.
      }
      else
      {
        throw mcrl2::runtime_error("Fail to rewrite term " + data::pp(result) + " to true or false.");
      }
    }
  }
  transitions.resize(new_position);
}

void lps2lts_algorithm::construct_trace(const storage_state_t& state1, mcrl2::trace::Trace& trace)
{
  storage_state_t state=state1;
  std::deque<storage_state_t> states;
  std::map<storage_state_t, storage_state_t>::iterator source;
  while ((source = m_backpointers.find(state)) != m_backpointers.end())
  {
    states.push_front(state);
    state = source->second;
  }

  trace.setState(state);
  next_state_generator::enumerator_queue_t enumeration_queue;
  for (std::deque<storage_state_t>::iterator i = states.begin(); i != states.end(); i++)
  {
    for (next_state_generator::iterator j = m_generator->begin(state, &enumeration_queue); j != m_generator->end(); j++)
    {
      storage_state_t destination = j->internal_state();
      if (m_use_confluence_reduction)
      {
        destination = get_prioritised_representative(destination);
      }
      if (destination == *i)
      {
        trace.addAction(j->action());
        break;
      }
    }
    enumeration_queue.clear();
    state = *i;
    trace.setState(state);
  }

}

// Contruct a trace to state1 and store in in filename.
bool lps2lts_algorithm::save_trace(const storage_state_t& state1, const std::string& filename)
{
  mcrl2::trace::Trace trace;
  lps2lts_algorithm::construct_trace(state1, trace);
  m_traces_saved++;

  try
  {
    trace.save(filename);
    return true;
  }
  catch(...)
  {
    return false;
  }
}

// Contruct a trace to state1, then add transition to it and store in in filename.
bool lps2lts_algorithm::save_trace(const storage_state_t& state1, const next_state_generator::transition_t& transition, const std::string& filename)
{
  mcrl2::trace::Trace trace;
  lps2lts_algorithm::construct_trace(state1, trace);
  trace.addAction(transition.action());
  trace.setState(transition.internal_state());
  m_traces_saved++;

  try
  {
    trace.save(filename);
    return true;
  }
  catch(...)
  {
    return false;
  }
}




bool lps2lts_algorithm::search_divergence(
              const storage_state_t& state,
              std::set<storage_state_t>& current_path,
              std::set<storage_state_t>& visited)
{
  current_path.insert(state);

  std::vector<storage_state_t> new_states;
  next_state_generator::enumerator_queue_t enumeration_queue;
  for (next_state_generator::iterator j = m_generator->begin(state, m_tau_summands, &enumeration_queue); j != m_generator->end(); j++)
  {
    assert(j->action().actions().size() == 0);

    if (visited.insert(j->internal_state()).second)
    {
      new_states.push_back(j->internal_state());
    }
    else if (current_path.count(j->internal_state()) != 0)
    {
      return true;
    }
  }

  for (std::vector<storage_state_t>::iterator i = new_states.begin(); i != new_states.end(); i++)
  {
    if (search_divergence(*i, current_path, visited))
    {
      return true;
    }
  }

  assert(current_path.count(state)==1);
  current_path.erase(state);
  return false;
}

void lps2lts_algorithm::check_divergence(const storage_state_t& state)
{
  std::set<storage_state_t> visited;
  std::set<storage_state_t> current_path;
  visited.insert(state);

  if (search_divergence(state, current_path, visited))
  {
    size_t state_number = m_state_numbers.index(state);
    if (m_options.trace && m_traces_saved < m_options.max_traces)
    {
      std::ostringstream filename_stream;
      filename_stream << "divergence_" << m_traces_saved;
      std::string filename = m_options.generate_filename_for_trace(m_options.trace_prefix, filename_stream.str(), "trc");
      if (save_trace(state, filename))
      {
        mCRL2log(info) << "divergence-detect: divergence found and saved to '" << filename
                       << "' (state index: " << state_number << ")." << std::endl;
      }
      else
      {
        mCRL2log(info) << "divergence-detect: divergence found, but could not be saved to '" << filename
                       << "' (state index: " << state_number << ")." << std::endl;
      }
    }
    else
    {
      mCRL2log(info) << "divergence-detect: divergence found (state index: " << state_number << ")." << std::endl;
    }
  }
}

void lps2lts_algorithm::save_actions(const storage_state_t& state, const next_state_generator::transition_t& transition)
{
  size_t state_number = m_state_numbers.index(state);
  mCRL2log(info) << "Detected action '" << pp(transition.action()) << "' (state index " << state_number << ")";
  if (m_options.trace && m_traces_saved < m_options.max_traces)
  {
    std::ostringstream filename_stream;
    filename_stream << "act_" << m_traces_saved;
    if (m_options.trace_multiactions.find(transition.action()) != m_options.trace_multiactions.end())
    {
      filename_stream << "_" << pp(transition.action());
    }
    for (process::action_list::const_iterator i = transition.action().actions().begin(); i != transition.action().actions().end(); i++)
    {
      if (m_options.trace_actions.count(i->label().name()) > 0)
      {
        filename_stream << "_" << i->label().name();
      }
    }
    std::string filename = m_options.generate_filename_for_trace(m_options.trace_prefix, filename_stream.str(), "trc");
    if (save_trace(state,transition, filename))
      mCRL2log(info) << " and saved to '" << filename << "'";
    else
      mCRL2log(info) << " but could not saved to '" << filename << "'";
  }
  mCRL2log(info) << std::endl;
}

void lps2lts_algorithm::save_deadlock(const storage_state_t& state)
{
  size_t state_number = m_state_numbers.index(state);
  if (m_options.trace && m_traces_saved < m_options.max_traces)
  {
    std::ostringstream filename_stream;
    filename_stream << "dlk_" << m_traces_saved;
    std::string filename = m_options.generate_filename_for_trace(m_options.trace_prefix, filename_stream.str(), "trc");
    if (save_trace(state, filename))
    {
      mCRL2log(verbose) << "deadlock-detect: deadlock found and saved to '" << filename
                        << "' (state index: " << state_number << ").\n";
    }
    else
    {
      mCRL2log(verbose) << "deadlock-detect: deadlock found, but could not be saved to '" << filename
                        << "' (state index: " << state_number << ").\n";
    }
  }
  else
  {
    mCRL2log(info) << "deadlock-detect: deadlock found (state index: " << state_number <<  ").\n";
  }
}

void lps2lts_algorithm::save_error(const storage_state_t& state)
{
  if (m_options.save_error_trace)
  {
    std::string filename = m_options.generate_filename_for_trace(m_options.trace_prefix, "error", "trc");
    if (save_trace(state, filename))
    {
      mCRL2log(verbose) << "saved trace to error in '" << filename << "'.\n";
    }
    else
    {
      mCRL2log(verbose) << "trace to error could not be saved in '" << filename << "'.\n";
    }
  }
}

bool lps2lts_algorithm::add_transition(const storage_state_t& source_state, next_state_generator::transition_t& transition)
{
  storage_state_t destination = transition.internal_state();

  size_t source_state_number;
  std::pair<size_t, bool> destination_state_number;
  if (m_options.bithashing)
  {
    source_state_number = m_bit_hash_table.add_state(source_state).first;
    destination_state_number = m_bit_hash_table.add_state(destination);
  }
  else
  {
    source_state_number = m_state_numbers[source_state];
    destination_state_number = m_state_numbers.put(destination);
  }
  if (destination_state_number.second)
  {
    m_num_states++;
    if (m_maintain_traces)
    {
      assert(m_backpointers.count(destination) == 0);
      m_backpointers[destination] = source_state;
    }

    if (m_options.outformat != lts_none && m_options.outformat != lts_aut)
    {
      assert(!m_options.bithashing);
      size_t state_number = m_output_lts.add_state(transition.state());
      assert(state_number == destination_state_number.first);
      static_cast <void>(state_number);
    }
  }

  if (m_options.detect_action && m_detected_action_summands[transition.summand_index()])
  {
    save_actions(source_state, transition);
  }

  if (m_options.outformat == lts_aut)
  {
    m_aut_file << "(" << source_state_number << ",\"" << lps::pp(transition.action()) << "\"," << destination_state_number.first << ")" << std::endl;
  }
  else if (m_options.outformat != lts_none)
  {
    std::pair<size_t, bool> action_label_number = m_action_label_numbers.put(lps::detail::multi_action_to_aterm(transition.action()));
    if (action_label_number.second)
    {
      size_t action_number = m_output_lts.add_action(transition.action(), transition.action().actions().size() == 0);
      assert(action_number == action_label_number.first);
      static_cast <void>(action_number); // Avoid a warning when compiling in non debug mode.
    }

    m_output_lts.add_transition(mcrl2::lts::transition(source_state_number, action_label_number.first, destination_state_number.first));
  }

  m_num_transitions++;

  for (std::set<lps::multi_action>::const_iterator ma = m_options.trace_multiactions.begin(); ma != m_options.trace_multiactions.end(); ++ma)
  {
    if (*ma == transition.action())
    {
      save_actions(source_state, transition);
    }
  }

  return destination_state_number.second;
}

void lps2lts_algorithm::get_transitions(const storage_state_t& state,
                                        std::vector<lps2lts_algorithm::next_state_generator::transition_t>& transitions,
                                        next_state_generator::enumerator_queue_t& enumeration_queue
                                       )
{
  assert(transitions.empty());
  if (m_options.detect_divergence)
  {
    check_divergence(state);
  }

  try
  {
    enumeration_queue.clear();
    next_state_generator::iterator it(m_generator->begin(state, *m_main_subset, &enumeration_queue));
    while (it)
    {
      transitions.push_back(*it++);
    }
  }
  catch (mcrl2::runtime_error& e)
  {
    mCRL2log(error) << "Error while exploring state space: " << e.what() << "\n";
    save_error(state);
    exit(EXIT_FAILURE);
  }

  if (m_value_prioritize)
  {
    value_prioritize(transitions);
  }

  if (transitions.empty() && m_options.detect_deadlock)
  {
    save_deadlock(state);
  }

  if (m_use_confluence_reduction)
  {
    for (std::vector<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
    {
      i->internal_state() = get_prioritised_representative(i->internal_state());
    }
  }
}

void lps2lts_algorithm::generate_lts_breadth_todo_max_is_npos()
{
  assert(m_options.todo_max==std::string::npos);
  size_t current_state = 0;
  size_t start_level_seen = 1;
  size_t start_level_transitions = 0;
  std::vector<next_state_generator::transition_t> transitions;
  time_t last_log_time = time(NULL) - 1, new_log_time;
  next_state_generator::enumerator_queue_t enumeration_queue;

  while (!m_must_abort && (current_state < m_state_numbers.size()) &&
         (current_state < m_options.max_states) && (!m_options.trace || m_traces_saved < m_options.max_traces))
  {
    storage_state_t state=m_state_numbers.get(current_state);
    get_transitions(state,transitions, enumeration_queue);

    for (std::vector<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
    {
      add_transition(state, *i);
    }
    transitions.clear();

    current_state++;
    if (current_state == start_level_seen)
    {
      m_level++;
      start_level_seen = m_num_states;
      start_level_transitions = m_num_transitions;
    }

    if (!m_options.suppress_progress_messages && time(&new_log_time) > last_log_time)
    {
      last_log_time = new_log_time;
      size_t lvl_states = m_num_states - start_level_seen;
      size_t lvl_transitions = m_num_transitions - start_level_transitions;
      mCRL2log(status) << std::fixed << std::setprecision(2)
                       << m_num_states << "st, " << m_num_transitions << "tr"
                       << ", explored " << 100.0 * ((float)current_state / m_num_states)
                       << "%. Last level: " << m_level << ", " << lvl_states << "st, " << lvl_transitions
                       << "tr.\n";
    }
  }

  if (current_state == m_options.max_states)
  {
    mCRL2log(verbose) << "explored the maximum number (" << m_options.max_states << ") of states, terminating." << std::endl;
  }
}

void lps2lts_algorithm::generate_lts_breadth_todo_max_larger_than_0(const storage_state_t& initial_state)
{
  assert(m_options.todo_max!=std::string::npos);
  size_t current_state = 0;
  size_t start_level_seen = 1;
  size_t start_level_explored = 0;
  size_t start_level_transitions = 0;
  time_t last_log_time = time(NULL) - 1, new_log_time;

  queue<storage_state_t> state_queue;
  state_queue.set_max_size(m_options.max_states < m_options.todo_max ? m_options.max_states : m_options.todo_max);
  state_queue.add_to_queue(initial_state);
  state_queue.swap_queues();
  std::vector<next_state_generator::transition_t> transitions;
  next_state_generator::enumerator_queue_t enumeration_queue;

  while (!m_must_abort && (state_queue.remaining() > 0) &&
         (current_state < m_options.max_states) && (!m_options.trace || m_traces_saved < m_options.max_traces))
  {
    const storage_state_t state=state_queue.get_from_queue();
    get_transitions(state,transitions, enumeration_queue);

    for (std::vector<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
    {
      if (add_transition(state, *i))
      {
        storage_state_t removed = state_queue.add_to_queue(i->internal_state());
        if (removed != storage_state_t())
        {
          m_num_states--;
        }
      }
    }
    transitions.clear();

    if (state_queue.remaining() == 0)
    {
      state_queue.swap_queues();
    }

    current_state++;
    if (current_state == start_level_seen)
    {
      if (!m_options.suppress_progress_messages)
      {
        mCRL2log(verbose) << "monitor: level " << m_level << " done."
                          << " (" << (current_state - start_level_explored) << " state"
                          << ((current_state - start_level_explored)==1?"":"s") << ", "
                          << (m_num_transitions - start_level_transitions) << " transition"
                          << ((m_num_transitions - start_level_transitions)==1?")\n":"s)\n");
      }

      m_level++;
      start_level_seen = m_num_states;
      start_level_explored = current_state;
      start_level_transitions = m_num_transitions;
    }

    if (!m_options.suppress_progress_messages && time(&new_log_time) > last_log_time)
    {
      last_log_time = new_log_time;
      size_t lvl_states = m_num_states - start_level_seen;
      size_t lvl_transitions = m_num_transitions - start_level_transitions;
      mCRL2log(status) << std::fixed << std::setprecision(2)
                       << m_num_states << "st, " << m_num_transitions << "tr"
                       << ", explored " << 100.0 * ((float)current_state / m_num_states)
                       << "%. Last level: " << m_level << ", " << lvl_states << "st, " << lvl_transitions
                       << "tr.\n";
    }
  }

  if (current_state == m_options.max_states)
  {
    mCRL2log(verbose) << "explored the maximum number (" << m_options.max_states << ") of states, terminating." << std::endl;
  }
}

void lps2lts_algorithm::generate_lts_breadth_bithashing(const storage_state_t& initial_state)
{
  size_t current_state = 0;

  size_t start_level_seen = 1;
  size_t start_level_explored = 0;
  size_t start_level_transitions = 0;

  queue<storage_state_t> state_queue;
  state_queue.set_max_size(m_options.max_states < m_options.todo_max ? m_options.max_states : m_options.todo_max);
  state_queue.add_to_queue(initial_state);
  state_queue.swap_queues();
  std::vector<next_state_generator::transition_t> transitions;
  next_state_generator::enumerator_queue_t enumeration_queue;

  while (!m_must_abort && (state_queue.remaining() > 0) && (current_state < m_options.max_states) && (!m_options.trace || m_traces_saved < m_options.max_traces))
  {
    const storage_state_t state=state_queue.get_from_queue();
    get_transitions(state,transitions, enumeration_queue);

    for (std::vector<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
    {
      if (add_transition(state, *i))
      {
        storage_state_t removed = state_queue.add_to_queue(i->internal_state());
        if (removed != storage_state_t())
        {
          m_bit_hash_table.remove_state_from_bithash(removed);
          m_num_states--;
        }
      }
    }
    transitions.clear();

    if (state_queue.remaining() == 0)
    {
      state_queue.swap_queues();
    }

    current_state++;
    if (!m_options.suppress_progress_messages && ((current_state % 1000) == 0))
    {
      mCRL2log(status) << "monitor: currently at level " << m_level << " with "
                        << current_state << " state" << ((current_state==1)?"":"s") << " and "
                        << m_num_transitions << " transition" << ((m_num_transitions==1)?"":"s")
                        << " explored and " << m_num_states << " state" << ((m_num_states==1)?"":"s") << " seen." << std::endl;
    }

    if (current_state == start_level_seen)
    {
      if (!m_options.suppress_progress_messages)
      {
        mCRL2log(verbose) << "monitor: level " << m_level << " done."
                          << " (" << (current_state - start_level_explored) << " state"
                          << ((current_state - start_level_explored)==1?"":"s") << ", "
                          << (m_num_transitions - start_level_transitions) << " transition"
                          << ((m_num_transitions - start_level_transitions)==1?")\n":"s)\n");
      }

      m_level++;
      start_level_seen = m_num_states;
      start_level_explored = current_state;
      start_level_transitions = m_num_transitions;
    }
  }

  if (current_state == m_options.max_states)
  {
    mCRL2log(verbose) << "explored the maximum number (" << m_options.max_states << ") of states, terminating." << std::endl;
  }
}

void lps2lts_algorithm::generate_lts_depth(const storage_state_t& initial_state)
{
  std::list<storage_state_t> stack;
  stack.push_back(initial_state);
  std::vector<next_state_generator::transition_t> transitions;

  size_t current_state = 0;
  next_state_generator::enumerator_queue_t enumeration_queue;

  while (!m_must_abort && (!stack.empty()) && (!m_options.trace || m_traces_saved < m_options.max_traces))
  {
    const storage_state_t state=stack.back();
    stack.pop_back();
    get_transitions(state,transitions, enumeration_queue);

    for (std::vector<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
    {
      if (add_transition(state, *i) && (current_state + stack.size() < m_options.max_states) && (stack.size() < m_options.todo_max))
      {
        stack.push_back(i->internal_state());
      }
    }
    transitions.clear();

    current_state++;
    if (!m_options.suppress_progress_messages && ((current_state % 1000) == 0))
    {
      mCRL2log(verbose) << "monitor: currently explored "
                        << current_state << " state" << ((current_state==1)?"":"s")
                        << " and " << m_num_transitions << " transition" << ((m_num_transitions==1)?"":"s")
                        << " (stacksize is " << stack.size() << ")" << std::endl;
    }
  }

  if (current_state == m_options.max_states)
  {
    mCRL2log(verbose) << "explored the maximum number (" << m_options.max_states << ") of states, terminating." << std::endl;
  }
}

void lps2lts_algorithm::generate_lts_random(const storage_state_t& initial_state)
{
  storage_state_t state = initial_state;
  std::vector<next_state_generator::transition_t> transitions;
  size_t current_state = 0;
  next_state_generator::enumerator_queue_t enumeration_queue;

  while (!m_must_abort && current_state < m_options.max_states && (!m_options.trace || m_traces_saved < m_options.max_traces))
  {
    get_transitions(state,transitions, enumeration_queue);

    if (transitions.empty())
    {
      break;
    }

    size_t index = rand() % transitions.size();
    storage_state_t new_state;

    for (std::vector<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
    {
      add_transition(state, *i);

      if (index-- == 0)
      {
        new_state = i->internal_state();
      }
    }
    transitions.clear();

    state = new_state;

    current_state++;
    if (!m_options.suppress_progress_messages && ((current_state % 1000) == 0))
    {
      mCRL2log(verbose) << "monitor: currently explored "
                        << m_num_transitions << " transition" << ((m_num_transitions==1)?"":"s")
                        << " and encountered " << m_num_states << " unique state" << ((m_num_states==1)?"":"s") << std::endl;
    }
  }

  if (current_state == m_options.max_states)
  {
    mCRL2log(verbose) << "explored the maximum number (" << m_options.max_states << ") of states, terminating." << std::endl;
  }
}
