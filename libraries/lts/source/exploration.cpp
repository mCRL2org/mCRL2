// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/lts/detail/exploration.h"

#include "mcrl2/lts/lts_io.h"
#include "mcrl2/trace/trace.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::lps;
using namespace mcrl2::lts;

/*
 * TODO:
 * - make enumeration caching and summand pruning user-configurable
 * - add state tree-storage option
 * - debug and enable used equation selection
 * - optimize action-detect by computing for each summand whether it should be detected or not
 * - optimize divergence-detect by keeping a separate next state generator
 * - clean up options struct
 */

exploration_strategy mcrl2::lts::str_to_expl_strat(std::string s)
{
  if (s=="b" || s=="breadth") return es_breadth;
  if (s=="d" || s=="depth") return es_depth;
  if (s=="r" || s=="random") return es_random;
  if (s=="p" || s=="prioritized") return es_value_prioritized;
  if (s=="q" || s=="rprioritized") return es_value_random_prioritized;
  return es_none;
}

std::string mcrl2::lts::expl_strat_to_str(exploration_strategy es)
{
  switch (es)
  {
    case es_breadth: return "breadth";
    case es_depth: return "depth";
    case es_random: return "random";
    case es_value_prioritized: return "prioritized";
    case es_value_random_prioritized: return "rprioritized";
    default: return "unknown";
  }
}

bool lps2lts_algorithm::initialise_lts_generation(lts_generation_options* options)
{
  m_options = *options;

  assert(!(m_options.bithashing && m_options.outformat != lts_aut && m_options.outformat != lts_none));

  bool use_enumeration_caching = true;
  bool use_summand_pruning = true;

  if (m_options.bithashing)
  {
    m_bit_hash_table = bit_hash_table(m_options.bithashsize);
  }
  else
  {
    m_state_numbers = atermpp::indexed_set(m_options.initial_table_size, 50);
  }

  m_num_states = 0;
  m_num_transitions = 0;
  m_level = 1;
  m_traces_saved = 0;

  m_maintain_traces = m_options.trace || m_options.save_error_trace;
  m_value_prioritize = (m_options.expl_strat == es_value_prioritized || m_options.expl_strat == es_value_random_prioritized);

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
    m_output_lts.set_data(m_options.specification.data());
    m_output_lts.set_process_parameters(m_options.specification.process().process_parameters());
    m_output_lts.set_action_labels(m_options.specification.action_labels());
  }

  lps::specification specification(m_options.specification);

  atermpp::set<data::function_symbol> extra_function_symbols;
  if (m_options.expl_strat == es_value_prioritized || m_options.expl_strat == es_value_random_prioritized)
  {
    extra_function_symbols.insert(data::greater(data::sort_nat::nat()));
    extra_function_symbols.insert(data::equal_to(data::sort_nat::nat()));
  }
  data::rewriter rewriter(specification.data(), /*data::used_data_equation_selector(specification.data(), extra_function_symbols, specification.global_variables()), */m_options.strat);

  if (m_options.priority_action != "")
  {
    mCRL2log(verbose) << "applying confluence reduction with tau action '" << m_options.priority_action << "'..." << std::endl;

    action_summand_vector prioritised_summands;
    action_summand_vector nonprioritised_summands;
    for (action_summand_vector::iterator i = specification.process().action_summands().begin(); i != specification.process().action_summands().end(); i++)
    {
      if ((m_options.priority_action == "tau" && i->is_tau()) ||
          (i->multi_action().actions().size() == 1 && m_options.priority_action == (std::string)i->multi_action().actions().front().label().name()))
      {
        action_summand summand(*i);
        summand.multi_action().actions() = action_list();
        prioritised_summands.push_back(summand);
      }
      else
      {
        nonprioritised_summands.push_back(*i);
      }
    }

    lps::specification prioritised_specification(specification);
    prioritised_specification.process().action_summands() = prioritised_summands;
    specification.process().action_summands() = nonprioritised_summands;

    m_confluence_generator = new next_state_generator(prioritised_specification, rewriter, use_enumeration_caching, use_summand_pruning);
  }
  else
  {
    m_confluence_generator = 0;
  }

  if (m_options.detect_divergence)
  {
    mCRL2log(verbose) << "Detect divergences with tau action is `tau'.\n";
    for (size_t i = 0; i < specification.process().action_summands().size(); i++)
    {
      if (specification.process().action_summands()[i].is_tau())
      {
        m_tau_summands.push_back(i);
      }
    }
  }

  bool compute_actions = m_options.outformat != lts_none || m_options.detect_action || m_maintain_traces || m_value_prioritize;
  if (!compute_actions)
  {
    for (size_t i = 0; i < specification.process().action_summands().size(); i++)
    {
      specification.process().action_summands()[i].multi_action().actions() = action_list();
    }
  }

  m_generator = new next_state_generator(specification, rewriter, use_enumeration_caching, use_summand_pruning);

  if (m_options.detect_deadlock)
  {
    mCRL2log(verbose) << "Detect deadlocks.\n" ;
  }

  return true;
}

bool lps2lts_algorithm::generate_lts()
{
  state_t initial_state = m_generator->internal_initial_state();
  if (m_confluence_generator != 0)
  {
    initial_state = get_prioritised_representative(initial_state);
  }

  if (m_options.bithashing)
  {
    m_bit_hash_table.add_state(initial_state);
  }
  else
  {
    m_state_numbers.put(initial_state);
  }

  if (m_options.outformat == lts_aut)
  {
    // HACK: this line will be overwritten once generation is finished.
    m_aut_file << "                                                 " << std::endl;
  }
  else if (m_options.outformat != lts_none)
  {
    size_t initial_state_number = m_output_lts.add_state(m_generator->initial_state());
    m_output_lts.set_initial_state(initial_state_number);
  }
  m_num_states = 1;

  mCRL2log(verbose) << "generating state space with '" << expl_strat_to_str(m_options.expl_strat) << "' strategy...\n";

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
      generate_lts_breadth(initial_state);
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
    m_aut_file << "des (0," << m_num_transitions << "," << m_num_states << ")";
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
      case lts_svc:
      {
        lts_svc_t svc;
        detail::lts_convert(m_output_lts, svc);
        svc.save(m_options.lts);
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

lps2lts_algorithm::state_t lps2lts_algorithm::get_prioritised_representative(lps2lts_algorithm::state_t state)
{
  assert(m_confluence_generator != 0);

  atermpp::map<state_t, size_t> number;
  atermpp::map<state_t, size_t> low;
  atermpp::map<state_t, atermpp::list<state_t> > next;
  atermpp::map<state_t, state_t> back;
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
      next[state] = atermpp::list<state_t>();

      for (next_state_generator::iterator i = m_confluence_generator->begin(state, &m_substitution); i != m_confluence_generator->end(); i++)
      {
        next[state].push_back(i->internal_state());
        if (number.count(i->internal_state()) == 0)
        {
          number[i->internal_state()] = 0;
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
      state_t back_state = back[state];
      low[back_state] = low[back_state] < low[state] ? low[back_state] : low[state];
      state = back_state;
    }
    else
    {
      state_t next_state = next[state].front();
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

void lps2lts_algorithm::value_prioritize(atermpp::list<next_state_generator::transition_t> &transitions)
{
  data::data_expression lowest_value;

  for (atermpp::list<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
  {
    if (i->action().actions().size() == 1 && i->action().actions().front().arguments().size() > 0)
    {
      const data::data_expression &argument = i->action().actions().front().arguments().front();
      if (mcrl2::data::sort_nat::is_nat(argument.sort()))
      {
        if (lowest_value == data::data_expression())
        {
          lowest_value = argument;
        }
        else
        {
          data::data_expression result = m_generator->get_rewriter()(data::greater(lowest_value, argument));

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

  for (atermpp::list<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end();)
  {
    if (i->action().actions().size() != 1)
    {
      i++;
    }
    else if (i->action().actions().front().arguments().size() == 0)
    {
      i++;
    }
    else if (!mcrl2::data::sort_nat::is_nat(i->action().actions().front().arguments().front().sort()))
    {
      i++;
    }
    else
    {
      data::data_expression result = m_generator->get_rewriter()(data::equal_to(lowest_value, i->action().actions().front().arguments().front()));

      if (data::sort_bool::is_true_function_symbol(result))
      {
        i++;
      }
      else if (data::sort_bool::is_false_function_symbol(result))
      {
        i = transitions.erase(i);
      }
      else
      {
        throw mcrl2::runtime_error("Fail to rewrite term " + data::pp(result) + " to true or false.");
      }
    }
  }
}

bool lps2lts_algorithm::save_trace(lps2lts_algorithm::state_t state, std::string filename)
{
  atermpp::deque<state_t> states;
  atermpp::map<state_t, state_t>::iterator source;
  while ((source = m_backpointers.find(state)) != m_backpointers.end())
  {
    states.push_front(state);
    state = source->second;
  }

  mcrl2::trace::Trace trace;
  trace.setState(m_generator->get_state(state));
  for (atermpp::deque<state_t>::iterator i = states.begin(); i != states.end(); i++)
  {
    bool found = false;
    for (next_state_generator::iterator j = m_generator->begin(state, &m_substitution); j != m_generator->end(); j++)
    {
      if (get_prioritised_representative(j->internal_state()) == *i)
      {
        trace.addAction(lps::detail::multi_action_to_aterm(j->action()));
        found = true;
        break;
      }
    }
    assert(found);
    state = *i;
    trace.setState(m_generator->get_state(state));
  }

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

bool lps2lts_algorithm::search_divergence(lps2lts_algorithm::state_t state, std::set<lps2lts_algorithm::state_t> &current_path, atermpp::set<lps2lts_algorithm::state_t> &visited)
{
  current_path.insert(state);

  std::vector<state_t> new_states;
  for (std::vector<size_t>::iterator i = m_tau_summands.begin(); i != m_tau_summands.end(); i++)
  {
    for (next_state_generator::iterator j = m_generator->begin(state, &m_substitution, *i); j != m_generator->end(); j++)
    {
      assert(j->action().actions().size() == 0);

      if (visited.insert(j->internal_state()).second)
      {
        new_states.push_back(j->internal_state());
      }
      else if (visited.count(j->internal_state()) != 0)
      {
        return true;
      }
    }
  }

  for (std::vector<state_t>::iterator i = new_states.begin(); i != new_states.end(); i++)
  {
    if (search_divergence(*i, current_path, visited))
    {
      return true;
    }
  }

  current_path.erase(state);
  return false;
}

void lps2lts_algorithm::check_divergence(lps2lts_algorithm::state_t state)
{
  atermpp::set<state_t> visited;
  std::set<state_t> current_path;
  visited.insert(state);

  if (search_divergence(state, current_path, visited))
  {
    size_t state_number = m_state_numbers.index(state);
    if (m_options.trace && m_traces_saved < m_options.max_traces)
    {
      std::ostringstream reason;
      reason << "divergence_" << m_traces_saved;
      std::string filename = m_options.generate_filename_for_trace(m_options.trace_prefix, reason.str(), "trc");
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

void lps2lts_algorithm::check_action(lps2lts_algorithm::state_t state, next_state_generator::transition_t &transition)
{
  for (action_list::iterator i = transition.action().actions().begin(); i != transition.action().actions().end(); i++)
  {
    if (m_options.trace_actions.count(i->label().name()) > 0)
    {
      size_t state_number = m_state_numbers.index(state);
      if (m_options.trace && m_traces_saved < m_options.max_traces)
      {
        std::ostringstream reason;
        reason << "act_" << m_traces_saved << "_" << std::string(i->label().name());
        std::string filename = m_options.generate_filename_for_trace(m_options.trace_prefix, reason.str(), "trc");
        if (save_trace(transition.internal_state(), filename))
        {
          mCRL2log(info) << "detect: action '" << lps::pp(transition.action())
                         << "' found and saved to '" << filename
                         << "' (state index: " << state_number << ")." << std::endl;
        }
        else
        {
          mCRL2log(info) << "detect: action '" << lps::pp(transition.action())
                         << "' found, but could not be saved to '" << filename
                         << "' (state index: " << state_number << ")." << std::endl;
        }
      }
      else
      {
        mCRL2log(info) << "detect: action '" << lps::pp(transition.action())
                       << "' found (state index: " << state_number << ")." << std::endl;
      }
    }
  }
}

void lps2lts_algorithm::save_deadlock(lps2lts_algorithm::state_t state)
{
  size_t state_number = m_state_numbers.index(state);
  if (m_options.trace && m_traces_saved < m_options.max_traces)
  {
    std::ostringstream reason;
    reason << "dlk_" << m_traces_saved;
    std::string filename = m_options.generate_filename_for_trace(m_options.trace_prefix, reason.str(), "trc");
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

void lps2lts_algorithm::save_error(lps2lts_algorithm::state_t state)
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

bool lps2lts_algorithm::add_transition(lps2lts_algorithm::state_t state, next_state_generator::transition_t &transition)
{
  size_t from_state_number = m_state_numbers[state];
  std::pair<size_t, bool> to_state_number;
  if (m_options.bithashing)
  {
    to_state_number = m_bit_hash_table.add_state(transition.internal_state());
  }
  else
  {
    to_state_number = m_state_numbers.put(transition.internal_state());
  }
  if (to_state_number.second)
  {
    m_num_states++;
    if (m_maintain_traces)
    {
      assert(m_backpointers.count(transition.internal_state()) == 0);
      m_backpointers[transition.internal_state()] = state;
    }

    if (m_options.outformat != lts_none && m_options.outformat != lts_aut)
    {
      assert(!m_options.bithashing);
      size_t state_number = m_output_lts.add_state(transition.state());
      assert(state_number == to_state_number.first);
      static_cast <void>(state_number);
    }
  }

  if (m_options.outformat == lts_aut)
  {
    m_aut_file << "(" << from_state_number << ",\"" << lps::pp(transition.action()) << "\"," << to_state_number.first << ")" << std::endl;
  }
  else if (m_options.outformat != lts_none)
  {
    atermpp::aterm_appl action_label_term = lps::detail::multi_action_to_aterm(transition.action());
    std::pair<size_t, bool> action_label_number = m_action_label_numbers.put(action_label_term);
    if (action_label_number.second)
    {
      size_t action_number = m_output_lts.add_action(transition.action(), transition.action().actions().size() == 0);
      assert(action_number == action_label_number.first);
      static_cast <void>(action_number); // Avoid a warning when compiling in non debug mode.
    }

    m_output_lts.add_transition(mcrl2::lts::transition(from_state_number, action_label_number.first, to_state_number.first));
  }

  m_num_transitions++;

  if (m_options.detect_action)
  {
    check_action(state, transition);
  }

  return to_state_number.second;
}

atermpp::list<lps2lts_algorithm::next_state_generator::transition_t> lps2lts_algorithm::get_transitions(lps2lts_algorithm::state_t state)
{
  if (m_options.detect_divergence)
  {
    check_divergence(state);
  }

  atermpp::list<next_state_generator::transition_t> transitions;
  try
  {
    next_state_generator::iterator it(m_generator->begin(state, &m_substitution));
    while (it)
    {
      transitions.push_back(*it);
      it++;
    }
    //transitions = atermpp::list<next_state_generator::transition_t>(m_generator->begin(state), m_generator->end());
  }
  catch (mcrl2::runtime_error& e)
  {
    std::cerr << "Error while exploring state space: " << e.what() << "\n";
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

  if (m_confluence_generator != 0)
  {
    for (atermpp::list<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
    {
      i->internal_state() = get_prioritised_representative(i->internal_state());
    }
  }

  return transitions;
}

void lps2lts_algorithm::generate_lts_breadth(state_t initial_state)
{
  size_t current_state = 0;

  size_t start_level_seen = 1;
  size_t start_level_explored = 0;
  size_t start_level_transitions = 0;

  while (!m_must_abort && (current_state < m_state_numbers.size()) && (current_state < m_options.max_states) && (!m_options.trace || m_traces_saved < m_options.max_traces))
  {
    state_t state = m_state_numbers.get(current_state);
    atermpp::list<next_state_generator::transition_t> transitions = get_transitions(state);

    for (atermpp::list<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
    {
      add_transition(state, *i);
    }

    current_state++;
    if (!m_options.suppress_progress_messages && ((current_state % 1000) == 0))
    {
      mCRL2log(verbose) << "monitor: currently at level " << m_level << " with "
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
}

void lps2lts_algorithm::generate_lts_breadth_bithashing(state_t initial_state)
{
  size_t current_state = 0;

  size_t start_level_seen = 1;
  size_t start_level_explored = 0;
  size_t start_level_transitions = 0;

  queue<state_t> state_queue;
  state_queue.set_max_size(m_options.max_states < m_options.todo_max ? m_options.max_states : m_options.todo_max);
  state_queue.add_to_queue(initial_state);
  state_queue.swap_queues();

  while (!m_must_abort && (state_queue.remaining() > 0) && (current_state < m_options.max_states) && (!m_options.trace || m_traces_saved < m_options.max_traces))
  {
    state_t state = state_queue.get_from_queue();
    atermpp::list<next_state_generator::transition_t> transitions = get_transitions(state);

    for (atermpp::list<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
    {
      if (add_transition(state, *i))
      {
        state_t removed = state_queue.add_to_queue(i->internal_state());
        if (removed != state_t())
        {
          m_bit_hash_table.remove_state_from_bithash(removed);
          m_num_states--;
        }
      }
    }

    if (state_queue.remaining() == 0)
    {
      state_queue.swap_queues();
    }

    current_state++;
    if (!m_options.suppress_progress_messages && ((current_state % 1000) == 0))
    {
      mCRL2log(verbose) << "monitor: currently at level " << m_level << " with "
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
}

void lps2lts_algorithm::generate_lts_depth(state_t initial_state)
{
  atermpp::list<state_info> stack;

  state_info initial_state_info;
  initial_state_info.state = initial_state;
  initial_state_info.transitions = get_transitions(initial_state_info.state);
  stack.push_back(initial_state_info);

  size_t current_state = 0;

  while (!m_must_abort && (!stack.empty()) && (!m_options.trace || m_traces_saved < m_options.max_traces))
  {
    if (stack.back().transitions.empty())
    {
      stack.pop_back();
      continue;
    }

    next_state_generator::transition_t transition = stack.back().transitions.front();
    stack.back().transitions.pop_front();

    if (add_transition(stack.back().state, transition) && (current_state < m_options.max_states) && (stack.size() < m_options.todo_max))
    {
      state_info info;
      info.state = transition.internal_state();
      info.transitions = get_transitions(info.state);
      stack.push_back(info);

      current_state++;
      if (!m_options.suppress_progress_messages && ((current_state % 1000) == 0))
      {
        mCRL2log(verbose) << "monitor: currently explored "
                          << current_state << " state" << ((current_state==1)?"":"s")
                          << " and " << m_num_transitions << " transition" << ((m_num_transitions==1)?"":"s")
                          << " (stacksize is " << stack.size() << ")" << std::endl;
      }
    }
  }
}

void lps2lts_algorithm::generate_lts_random(state_t initial_state)
{
  state_t state = initial_state;

  size_t current_state = 0;

  while (!m_must_abort && current_state < m_options.max_states && (!m_options.trace || m_traces_saved < m_options.max_traces))
  {
    atermpp::list<next_state_generator::transition_t> transitions = get_transitions(state);

    if (transitions.empty())
    {
      break;
    }

    size_t index = rand() % transitions.size();
    state_t new_state;

    for (atermpp::list<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
    {
      add_transition(state, *i);

      if (index-- == 0)
      {
        new_state = i->internal_state();
      }
    }

    state = new_state;

    current_state++;
    if (!m_options.suppress_progress_messages && ((current_state % 1000) == 0))
    {
      mCRL2log(verbose) << "monitor: currently explored "
                        << m_num_transitions << " transition" << ((m_num_transitions==1)?"":"s")
                        << " and encountered " << m_num_states << " unique state" << ((m_num_states==1)?"":"s") << std::endl;
    }
  }
}
