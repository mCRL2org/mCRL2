// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iomanip>

#include "mcrl2/lps/resolve_name_clashes.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"
#include "mcrl2/lps/one_point_rule_rewrite.h"
#include "mcrl2/lps/detail/move_constants_to_substitution.h"
#include "mcrl2/lts/detail/exploration.h"
#include "mcrl2/lts/detail/counter_example.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::lps;
using namespace mcrl2::lts;

probabilistic_state<std::size_t, probabilistic_data_expression> lps2lts_algorithm::transform_initial_probabilistic_state_list
                 (const next_state_generator::transition_t::state_probability_list& initial_states)
{
  assert(!initial_states.empty());
  if (++initial_states.begin() == initial_states.end()) // Means initial_states.size()==1
  {
    std::size_t state_number=m_state_numbers.insert(initial_states.front().state()).first;
    return probabilistic_state<std::size_t, probabilistic_data_expression>(state_number);
  }
  std::vector <state_probability_pair<std::size_t, lps::probabilistic_data_expression> > result;
  for(lps::next_state_generator::transition_t::state_probability_list::const_iterator i=initial_states.begin();
                    i!=initial_states.end(); ++i)
  {
    std::size_t state_number=m_state_numbers.insert(i->state()).first;
    result.push_back(state_probability_pair<std::size_t, probabilistic_data_expression>(state_number, i->probability()));
  }
  return probabilistic_state<std::size_t, probabilistic_data_expression>(result.begin(),result.end());
}

probabilistic_state<std::size_t, probabilistic_data_expression> lps2lts_algorithm::create_a_probabilistic_state_from_target_distribution(
               const std::size_t base_state_number,
               const next_state_generator::transition_t::state_probability_list& other_probabilities,
               const lps::state& source_state,
               const std::function<void(const lps::state&)> add_state_to_todo_queue_function)
{
  if (other_probabilities.empty())
  {
    return probabilistic_state<std::size_t, probabilistic_data_expression>(base_state_number);
  }

  std::vector <state_probability_pair<std::size_t, probabilistic_data_expression> > result;

  probabilistic_data_expression residual_probability=probabilistic_data_expression::one();

  for(lps::next_state_generator::transition_t::state_probability_list::const_iterator
                         i=other_probabilities.begin();
                         i!=other_probabilities.end(); ++i)
  {
    if (is_application(i->probability()) && atermpp::down_cast<data::application>(i->probability()).head().size()!=3)
    {
      throw mcrl2::runtime_error("The probability " + data::pp(i->probability()) + " is not a proper rational number.");
    }
    residual_probability=data::sort_real::minus(residual_probability,i->probability());
    const std::size_t probability_destination_state_number=add_target_state(source_state,i->state(),add_state_to_todo_queue_function);
    result.push_back(state_probability_pair<std::size_t, probabilistic_data_expression>(probability_destination_state_number, i->probability()));
  }

  residual_probability= (m_generator->get_rewriter())(residual_probability);
  result.push_back(state_probability_pair<std::size_t, probabilistic_data_expression>(base_state_number, residual_probability));
  return  probabilistic_state<std::size_t, probabilistic_data_expression>(result.begin(),result.end());
}

bool is_hidden_summand(const mcrl2::process::action_list& l,
                       const std::set<core::identifier_string>& internal_action_labels)
{
  // Note that if l is empty, true is returned, as desired.
  for(const mcrl2::process::action& a: l)
  {
    if (internal_action_labels.count(a.label().name())==0) // Not found, s has a visible action among its multi-actions.
    {
      return false;
    }
  }
  return true;
}

void lps2lts_algorithm::initialise_lts_generation(const lts_generation_options& options)
{
  m_options = options;

  assert(!(m_options.bithashing && m_options.outformat != lts_aut && m_options.outformat != lts_none));

  if (m_options.bithashing)
  {
    m_bit_hash_table = bit_hash_table(m_options.bithashsize);
  }
  else
  {
    m_state_numbers = utilities::indexed_set<lps::state>(m_options.initial_table_size);
  }

  m_num_states = 0;
  m_num_transitions = 0;
  m_level = 1;
  m_traces_saved = 0;

  m_maintain_traces = m_options.trace || m_options.save_error_trace;
  m_value_prioritize = (m_options.expl_strat == es_value_prioritized || m_options.expl_strat == es_value_random_prioritized);

  lps::stochastic_specification specification(m_options.specification);
  resolve_summand_variable_name_clashes(specification);

  if (m_options.outformat == lts_aut)
  {
    mCRL2log(verbose) << "writing state space in AUT format to '" << m_options.lts << "'." << std::endl;
    m_aut_file.open(m_options.lts.c_str());
    if (!m_aut_file.is_open())
    {
      mCRL2log(error) << "cannot open '" << m_options.lts << "' for writing" << std::endl;
      std::exit(EXIT_FAILURE);
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
    m_output_lts.set_action_label_declarations(specification.action_labels());
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
    extra_function_symbols.insert(data::sort_real::minus(data::sort_real::real_(),data::sort_real::real_()));
    // The symbols below are needed to compare probabilities. Greater implies the inclusion of smaller. 
    extra_function_symbols.insert(data::greater(data::sort_real::real_()));

    if (m_options.expl_strat == es_value_prioritized || m_options.expl_strat == es_value_random_prioritized)
    {
      extra_function_symbols.insert(data::greater(data::sort_nat::nat()));
      extra_function_symbols.insert(data::equal_to(data::sort_nat::nat()));
    }

    rewriter = data::rewriter(specification.data(), 
                              data::used_data_equation_selector(specification.data(), extra_function_symbols, specification.global_variables()), 
                              m_options.strat);
  }
  else
  {
    rewriter = data::rewriter(specification.data(), m_options.strat);
  }

  // Apply the one point rewriter to the linear process specification.
  // This simplifies expressions of the shape exists x:X . (x == e) && phi to phi[x:=e], enabling
  // more lps's to generate lts's. The overhead of this rewriter is limited.
  one_point_rule_rewrite(specification);

  // Replace all constant expressions t in a specification by a variable x, and set base_substitution(x)=u where u is the normal form of t.
  // This avoids having to rewrite the constant expressions each time they are encountered. This typically saves more than 50% of the total
  // time to generate a state space.
  data::mutable_indexed_substitution<> base_substitution;  // This is the substitution used as base in the state space generation.
  move_constants_to_substitution(specification, rewriter, base_substitution);

  stochastic_action_summand_vector prioritised_summands;
  stochastic_action_summand_vector nonprioritised_summands;
  if (m_options.priority_action != "")
  {
    mCRL2log(verbose) << "applying confluence reduction with tau action '" << m_options.priority_action << "'..." << std::endl;

    for(const stochastic_action_summand& s: specification.process().action_summands())
    {
      if ((m_options.priority_action == "tau" && s.is_tau()) ||
          (s.multi_action().actions().size() == 1 && m_options.priority_action == static_cast<std::string>(s.multi_action().actions().front().label().name())))
      {
        prioritised_summands.push_back(s);
      }
      else
      {
        nonprioritised_summands.push_back(s);
      }
    }

    m_use_confluence_reduction = true;
  }
  else
  {
    m_use_confluence_reduction = false;
  }

  stochastic_action_summand_vector tau_summands;
  if (m_options.detect_divergence)
  {
    mCRL2log(verbose) << "Detect divergences where actions with the following labels are hidden: tau";
    for(const core::identifier_string& a: m_options.actions_internal_for_divergencies)
    {
      mCRL2log(verbose) << ", " << a;
    }
    mCRL2log(verbose) << ".\n";

    for (const stochastic_action_summand& s: specification.process().action_summands())
    {
      if (is_hidden_summand(s.multi_action().actions(),m_options.actions_internal_for_divergencies))
      {
        tau_summands.push_back(s);
      }
    }
  }

  if (m_options.detect_action)
  {
    m_detected_action_summands.reserve(specification.process().action_summands().size());
    for (std::size_t i = 0; i < specification.process().action_summands().size(); i++)
    {
      bool found = false;
      for (process::action_list::const_iterator j = specification.process().action_summands()[i].multi_action().actions().begin();
                         j != specification.process().action_summands()[i].multi_action().actions().end(); j++)
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

  bool compute_actions = m_options.outformat != lts_none || m_options.detect_action || !m_options.trace_multiactions.empty() ||
                         m_maintain_traces || m_value_prioritize || !m_options.actions_internal_for_divergencies.empty();
  if (!compute_actions)
  {
    for (std::size_t i = 0; i < specification.process().action_summands().size(); i++)
    {
      specification.process().action_summands()[i].multi_action().actions() = process::action_list();
    }

    if (m_use_confluence_reduction)
    {
      for (std::size_t i = 0; i < nonprioritised_summands.size(); i++)
      {
        nonprioritised_summands[i].multi_action().actions() = process::action_list();
      }
      for (std::size_t i = 0; i < prioritised_summands.size(); i++)
      {
        prioritised_summands[i].multi_action().actions() = process::action_list();
      }
    }
  }
  m_generator = new next_state_generator(specification, rewriter, base_substitution, m_options.use_enumeration_caching, m_options.use_summand_pruning);

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

  if (m_options.detect_nondeterminism)
  {
    mCRL2log(verbose) << "Detect nondeterministic states.\n" ;
  }
}

void set_seed_random_generator()
{
  // In order to guarantee that time is different from a previous run to initialise
  // the seed of a previous run and not get the same results, we wait until time gets
  // a fresh value. It is better to replace this by c++11 random generators, as this
  // is an awkward solution.
  for(time_t t = time(nullptr); time(nullptr) == t; )
  {}  ; // Wait until time changes.
  srand((unsigned)time(nullptr));
}

bool lps2lts_algorithm::generate_lts(const lts_generation_options& options)
{
  initialise_lts_generation(options);
  // First generate a vector of initial states from the initial distribution.
  m_initial_states=m_generator->initial_states();
  assert(!m_initial_states.empty());

  // Count the number of states.
  m_num_states = 0;
  for(lps::next_state_generator::transition_t::state_probability_list::const_iterator i=m_initial_states.begin();
                    i!=m_initial_states.end(); ++i)
  {
    m_num_states++;
  }

  if (m_use_confluence_reduction)
  {
    set_prioritised_representatives(m_initial_states);
  }

  if (m_options.bithashing)
  {
    m_bit_hash_table.add_states(m_initial_states);
  }
  else
  {
    // Store the initial states in the indexed set.
    for(lps::next_state_generator::transition_t::state_probability_list::const_iterator i=m_initial_states.begin();
                    i!=m_initial_states.end(); ++i)
    {
      if (m_state_numbers.insert(i->state()).second && m_options.outformat != lts_aut) // The state is new.
      {
        m_output_lts.add_state(state_label_lts(i->state()));
      }
    }
  }

  if (m_options.outformat == lts_aut)
  {
    m_aut_file << "des(";
    print_target_distribution_in_aut_format(m_initial_states,lps::state(),[](const lps::state&){});
    // HACK: this line will be overwritten once generation is finished.
    m_aut_file << ",0,0)                                          " << std::endl;
  }
  else if (m_options.outformat != lts_none)
  {
    m_output_lts.set_initial_probabilistic_state(transform_initial_probabilistic_state_list(m_initial_states));
  }

  mCRL2log(verbose) << "generating state space with '" << m_options.expl_strat << "' strategy...\n";

  if (m_options.max_states == 0)
  {
    return true;
  }
  if (m_options.expl_strat == es_breadth || m_options.expl_strat == es_value_prioritized)
  {
    if (m_options.bithashing)
    {
      generate_lts_breadth_bithashing(m_initial_states);
    }
    else
    {
      if (m_options.todo_max==std::string::npos)
      {
        generate_lts_breadth_todo_max_is_npos(m_initial_states);
      }
      else
      {
        set_seed_random_generator();
        generate_lts_breadth_todo_max_is_not_npos(m_initial_states);
      }
    }

    mCRL2log(verbose) << "done with state space generation ("
                      << m_level-1 << " level" << ((m_level==2)?"":"s") << ", "
                      << m_num_states << " state" << ((m_num_states == 1)?"":"s")
                      << " and " << m_num_transitions << " transition" << ((m_num_transitions==1)?"":"s") << ")" << std::endl;
  }
  else if (m_options.expl_strat == es_depth)
  {
    generate_lts_depth(m_initial_states);

    mCRL2log(verbose) << "done with state space generation ("
                      << m_num_states << " state" << ((m_num_states == 1)?"":"s")
                      << " and " << m_num_transitions << " transition" << ((m_num_transitions==1)?"":"s") << ")" << std::endl;
  }
  else if (m_options.expl_strat == es_random || m_options.expl_strat == es_value_random_prioritized)
  {
    set_seed_random_generator();

    generate_lts_random(m_initial_states);

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

  finalise_lts_generation();
  return true;
}

void lps2lts_algorithm::finalise_lts_generation()
{
  if (m_options.outformat == lts_aut)
  {
    m_aut_file.flush();
    m_aut_file.seekp(0);
    m_aut_file << "des (";
    print_target_distribution_in_aut_format(m_initial_states,lps::state(),[](const lps::state&){});
    m_aut_file << "," << m_num_transitions << "," << m_num_states << ")";
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
        probabilistic_lts_fsm_t fsm;
        detail::lts_convert(m_output_lts, fsm);
        fsm.save(m_options.lts);
        break;
      }
      case lts_dot:
      {
        probabilistic_lts_dot_t dot;
        detail::lts_convert(m_output_lts, dot);
        dot.save(m_options.lts);
        break;
      }
      default:
        assert(0);
    }
  }
}

void lps2lts_algorithm::set_prioritised_representatives(next_state_generator::transition_t::state_probability_list& states)
{
  for(next_state_generator::transition_t::state_probability_list::iterator i=states.begin(); i!=states.end(); ++i)
  {
    i->state()=get_prioritised_representative(i->state());
  }
}

// Confluence reduction based on S.C.C. Blom, Partial tau-confluence for
// Efficient State Space Generation, Technical Report SEN-R0123, CWI, Amsterdam, 2001

lps::state lps2lts_algorithm::get_prioritised_representative(const lps::state& state1)
{
  lps::state state=state1;
  assert(m_use_confluence_reduction);

  std::map<lps::state, std::size_t> number;
  std::map<lps::state, std::size_t> low;
  std::map<lps::state, std::list<lps::state> > next;
  std::map<lps::state, lps::state> back;
  next_state_generator::enumerator_queue_t enumeration_queue;

  std::size_t count = 0;
  number[state] = 0;

  while (true)
  {
    assert(number.count(state) > 0);
    if (number[state] == 0)
    {
      count++;
      number[state] = count;
      low[state] = count;
      next[state] = std::list<lps::state>();

      enumeration_queue.clear();
      for (next_state_generator::iterator i = m_generator->begin(state, m_prioritized_subset, &enumeration_queue); i; i++)
      {
        const lps::state s=i->target_state();
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
      lps::state back_state = back[state];
      low[back_state] = low[back_state] < low[state] ? low[back_state] : low[state];
      state = back_state;
    }
    else
    {
      lps::state next_state = next[state].front();
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
            if (m_options.outformat == lts_aut)
            {
              m_aut_file.flush();
            }
            throw mcrl2::runtime_error("Fail to rewrite term " + data::pp(result) + " to true or false.");
          }
        }
      }
    }
  }

  std::size_t new_position=0;
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
        if (m_options.outformat == lts_aut)
        {
          m_aut_file.flush();
        }
        throw mcrl2::runtime_error("Fail to rewrite term " + data::pp(result) + " to true or false.");
      }
    }
  }
  transitions.resize(new_position);
}

void lps2lts_algorithm::construct_trace(const lps::state& state1, mcrl2::trace::Trace& trace)
{
  lps::state state=state1;
  std::deque<lps::state> states;
  std::map<lps::state, lps::state>::iterator source;
  while ((source = m_backpointers.find(state)) != m_backpointers.end())
  {
    states.push_front(state);
    state = source->second;
  }

  trace.setState(state);
  next_state_generator::enumerator_queue_t enumeration_queue;
  for (std::deque<lps::state>::iterator i = states.begin(); i != states.end(); i++)
  {
    bool found=false;
    for (next_state_generator::iterator j = m_generator->begin(state, &enumeration_queue); j != m_generator->end(); j++)
    {
      lps::state destination = j->target_state();
      if (m_use_confluence_reduction)
      {
        destination = get_prioritised_representative(destination);
      }
      if (destination == *i)
      {
        trace.addAction(j->action());
        found=true;
        break;
      }
    }
    if (not found)
    {
      throw mcrl2::runtime_error(std::string("Fail to save a trace. Most likely due to the use of binders (forall, exists, lambda, Set, Map) in a state vector.\n") +
                                 "Problematic state (" + pp(*i) + ")");
    }
    enumeration_queue.clear();
    state = *i;
    trace.setState(state);
  }

}

// Contruct a trace to state1 and store in in filename.
bool lps2lts_algorithm::save_trace(const lps::state& state1, const std::string& filename)
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
bool lps2lts_algorithm::save_trace(const lps::state& state1,
                                   const next_state_generator::transition_t& transition,
                                   const std::string& filename)
{
  mcrl2::trace::Trace trace;
  lps2lts_algorithm::construct_trace(state1, trace);
  trace.addAction(transition.action());
  trace.setState(transition.target_state());
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

template <class COUNTER_EXAMPLE_GENERATOR>
bool lps2lts_algorithm::search_divergence(
              const mcrl2::lts::detail::state_index_pair<COUNTER_EXAMPLE_GENERATOR>& state_pair,
              std::set<lps::state>& current_path,
              std::set<lps::state>& visited,
              COUNTER_EXAMPLE_GENERATOR& divergence_loop)
{
  current_path.insert(state_pair.state());
  std::vector<mcrl2::lts::detail::state_index_pair<COUNTER_EXAMPLE_GENERATOR> > new_states;
  next_state_generator::enumerator_queue_t enumeration_queue;
  for (next_state_generator::iterator j = m_generator->begin(state_pair.state(), m_tau_summands, &enumeration_queue); j != m_generator->end(); j++)
  {
    assert(is_hidden_summand(j->action().actions(),m_options.actions_internal_for_divergencies));

    std::pair<std::size_t, bool> action_label_number = m_action_label_numbers.put(j->action());
    if (action_label_number.second)
    {
      assert(j->action().actions().size() != 0);
      std::size_t action_number = m_output_lts.add_action(action_label_lts(j->action()));
      assert(action_number == action_label_number.first);
      static_cast <void>(action_number); // Avoid a warning when compiling in non debug mode.
    }

    if (non_divergent_states.count(j->target_state())==0) // This state is not shown to be non convergent. So, an investigation is in order.
    {
      typename COUNTER_EXAMPLE_GENERATOR::index_type i=divergence_loop.add_transition(action_label_number.first,state_pair.index());
      if (visited.insert(j->target_state()).second)
      {
        new_states.push_back(mcrl2::lts::detail::state_index_pair<COUNTER_EXAMPLE_GENERATOR>(j->target_state(),i));
      }
      else if (current_path.count(j->target_state()) != 0)
      {
        mCRL2log(info) << "divergence-detect: divergence found." << std::endl;
        divergence_loop.save_counter_example(i,m_output_lts);
        return true;
      }
    }
  }

  for (const mcrl2::lts::detail::state_index_pair<COUNTER_EXAMPLE_GENERATOR>& p: new_states)
  {
    if (search_divergence(p, current_path, visited,divergence_loop))
    {
      return true;
    }
  }

  assert(current_path.count(state_pair.state())==1);
  current_path.erase(state_pair.state());
  return false;
}

template <class COUNTER_EXAMPLE_GENERATOR>
void lps2lts_algorithm::check_divergence(
              const mcrl2::lts::detail::state_index_pair<COUNTER_EXAMPLE_GENERATOR>& state_pair,
              COUNTER_EXAMPLE_GENERATOR divergence_loop)
{
  std::set<lps::state> visited;
  std::set<lps::state> current_path;
  visited.insert(state_pair.state());

  if (search_divergence(state_pair, current_path, visited,divergence_loop))
  {
    if (m_options.trace && m_traces_saved < m_options.max_traces)
    {
      std::string filename = m_options.trace_prefix + "_divergence_" + std::to_string(m_traces_saved) + ".trc";
      if (save_trace(state_pair.state(), filename))
      {
        mCRL2log(info) << "Trace to the divergencing state is saved to '" << filename << std::endl;
      }
      else
      {
        mCRL2log(info) << "Failed to save trace to diverging state to the file " << filename << "." << std::endl;
      }
    }
    std::size_t state_number = m_state_numbers.index(state_pair.state());
    mCRL2log(info) << "State index of diverging state is " << state_number << "." << std::endl;
  }
  else
  {
    // No divergence has been found. Register all states as being non divergent.
    for(const lps::state& s: visited)
    {
      assert(non_divergent_states.count(s)==0);
      non_divergent_states.insert(s);
    }
  }
}

void lps2lts_algorithm::save_actions(const lps::state& state, const next_state_generator::transition_t& transition)
{
  std::size_t state_number = m_state_numbers.index(state);
  mCRL2log(info) << "Detected action '" << pp(transition.action()) << "' (state index " << state_number << ")";
  if (m_options.trace && m_traces_saved < m_options.max_traces)
  {
    std::string filename = m_options.trace_prefix + "_act_" + std::to_string(m_traces_saved);
    if (m_options.trace_multiactions.find(transition.action()) != m_options.trace_multiactions.end())
    {
      filename = filename + "_" + pp(transition.action());
    }
    for (const process::action& a: transition.action().actions())
    {
      if (m_options.trace_actions.count(a.label().name()) > 0)
      {
        filename = filename + "_" + core::pp(a.label().name());
      }
    }
    filename = filename + ".trc";
    if (save_trace(state, transition, filename))
    {
      mCRL2log(info) << " and saved to '" << filename << "'";
    }
    else
    {
      mCRL2log(info) << " but it could not saved to '" << filename << "'";
    }
  }
  mCRL2log(info) << std::endl;
}

void lps2lts_algorithm::save_nondeterministic_state(const lps::state& state,
                                                    const next_state_generator::transition_t& nondeterminist_transition)
{
  std::size_t state_number = m_state_numbers.index(state);
  if (m_options.trace && m_traces_saved < m_options.max_traces)
  {
    std::string filename = m_options.trace_prefix + "_nondeterministic_" + std::to_string(m_traces_saved) + ".trc";
    if (save_trace(state, nondeterminist_transition, filename))
    {
      mCRL2log(info) << "Nondeterministic state found and saved to '" << filename
                     << "' (state index: " << state_number << ").\n";
    }
    else
    {
      mCRL2log(info) << "Nondeterministic state found, but its trace could not be saved to '" << filename
                     << "' (state index: " << state_number << ").\n";
    }
  }
  else
  {
    mCRL2log(info) << "Nondeterministic state found (state index: " << state_number <<  ").\n";
  }
}

void lps2lts_algorithm::save_deadlock(const lps::state& state)
{
  std::size_t state_number = m_state_numbers.index(state);
  if (m_options.trace && m_traces_saved < m_options.max_traces)
  {
    std::string filename = m_options.trace_prefix + "_dlk_" + std::to_string(m_traces_saved) + ".trc";
    if (save_trace(state, filename))
    {
      mCRL2log(info) << "deadlock-detect: deadlock found and saved to '" << filename
                     << "' (state index: " << state_number << ").\n";
    }
    else
    {
      mCRL2log(info) << "deadlock-detect: deadlock found, but its trace could not be saved to '" << filename
                     << "' (state index: " << state_number << ").\n";
    }
  }
  else
  {
    mCRL2log(info) << "deadlock-detect: deadlock found (state index: " << state_number <<  ").\n";
  }
}

void lps2lts_algorithm::save_error(const lps::state& state)
{
  if (m_options.save_error_trace)
  {
    std::string filename = m_options.trace_prefix + "_error.trc";
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

std::pair<std::size_t, bool> lps2lts_algorithm::get_state_number(const lps::state& target_state)
{
  if (m_options.bithashing)
  {
    return m_bit_hash_table.add_state(target_state);
  }
  else
  {
    return m_state_numbers.insert(target_state);
  }
}

// Add the target state to the transition system, and if necessary store it to be investigated later.
// Return the number of the target state.
std::size_t lps2lts_algorithm::add_target_state(
                                     const lps::state& source_state, 
                                     const lps::state& target_state,
                                     const std::function<void(const lps::state&)> add_state_to_todo_queue_function)
{
  std::pair<std::size_t, bool> destination_state_number=get_state_number(target_state);;
  if (destination_state_number.second) // The state is new.
  {
    add_state_to_todo_queue_function(target_state);
    m_num_states++;
    if (m_maintain_traces)
    {
      assert(m_backpointers.count(target_state) == 0);
      m_backpointers[target_state] = source_state;
    }

    if (m_options.outformat != lts_none && m_options.outformat != lts_aut)
    {
      assert(!m_options.bithashing);
      m_output_lts.add_state(state_label_lts(target_state));
    }
  }
  return destination_state_number.first;
}

void lps2lts_algorithm::print_target_distribution_in_aut_format(
               const lps::next_state_generator::transition_t::state_probability_list& state_probability_list,
               const std::size_t last_state_number,
               const lps::state& source_state,
               const std::function<void(const lps::state&)> add_state_to_todo_queue_function)
{
  for(lps::next_state_generator::transition_t::state_probability_list::const_iterator
                        i=state_probability_list.begin();
                        i!=state_probability_list.end(); ++i)
  {
    if (m_options.outformat == lts_aut)
    {
      const lps::state probability_destination = i->state();
      const std::size_t probability_destination_state_number=add_target_state(source_state,probability_destination,add_state_to_todo_queue_function);
      if (is_application(i->probability()) && atermpp::down_cast<data::application>(i->probability()).head().size()!=3)
      {
        if (m_options.outformat == lts_aut)
        {
          m_aut_file.flush();
        }
        throw mcrl2::runtime_error("The probability " + data::pp(i->probability()) + " is not a proper rational number.");
      }
      const data::application& prob=atermpp::down_cast<data::application>(i->probability());
      if (prob.head()!=data::sort_real::creal())
      {
        throw mcrl2::runtime_error("Probability is not a closed expression with a proper enumerator and denominator: " + pp(i->probability()) + ".");
      }
      m_aut_file << probability_destination_state_number << " " << (prob[0]) << "/"
                                                                << (prob[1]) << " ";
    }
  }
  m_aut_file << last_state_number;
}

void lps2lts_algorithm::print_target_distribution_in_aut_format(
                const lps::next_state_generator::transition_t::state_probability_list& state_probability_list,
                const lps::state& source_state,
                const std::function<void(const lps::state&)> add_state_to_todo_queue_function)
{
  assert(!state_probability_list.empty());
  const std::size_t a_destination_state_number=
                add_target_state(source_state,state_probability_list.front().state(), add_state_to_todo_queue_function);

  lps::next_state_generator::transition_t::state_probability_list temporary_list=state_probability_list;
  temporary_list.pop_front();
  print_target_distribution_in_aut_format(temporary_list, a_destination_state_number, source_state, add_state_to_todo_queue_function);
}


void lps2lts_algorithm::add_transition(const lps::state& source_state, 
                                       const next_state_generator::transition_t& transition,
                                       const std::function<void(const lps::state&)> add_state_to_todo_queue_function
                                      )
{
  std::size_t source_state_number;
  if (m_options.bithashing)
  {
    source_state_number = m_bit_hash_table.add_state(source_state).first;
  }
  else
  {
    source_state_number = m_state_numbers.index(source_state);
  }

  const lps::state& destination = transition.target_state();
  const std::size_t destination_state_number=add_target_state(source_state,destination,add_state_to_todo_queue_function);

  if (m_options.detect_action && m_detected_action_summands[transition.summand_index()])
  {
    save_actions(source_state, transition);
  }

  if (m_options.outformat == lts_aut || m_options.outformat == lts_none)
  {

    if (m_options.outformat == lts_aut)
    {
      m_aut_file << "(" << source_state_number << ",\"" << lps::pp(transition.action()) << "\",";
    }

    print_target_distribution_in_aut_format(transition.other_target_states(),destination_state_number, source_state, add_state_to_todo_queue_function);

    // Close transition.
    if (m_options.outformat == lts_aut)
    {
      m_aut_file << ")\n"; // Intentionally do not use std::endl to avoid flushing.
    }
  }
  else
  {
    std::pair<std::size_t, bool> action_label_number = m_action_label_numbers.put(transition.action());
    if (action_label_number.second)
    {
      assert(transition.action().actions().size() != 0);
      std::size_t action_number = m_output_lts.add_action(action_label_lts(transition.action()));
      assert(action_number == action_label_number.first);
      static_cast <void>(action_number); // Avoid a warning when compiling in non debug mode.
    }
    std::size_t number_of_a_new_probabilistic_state=m_output_lts.add_probabilistic_state(
                                    create_a_probabilistic_state_from_target_distribution(
                                               destination_state_number,
                                               transition.other_target_states(),
                                               source_state,
                                               add_state_to_todo_queue_function)); // Add a new probabilistic state.
    m_output_lts.add_transition(mcrl2::lts::transition(source_state_number, action_label_number.first, number_of_a_new_probabilistic_state));
  }

  m_num_transitions++;

  for (std::set<lps::multi_action>::const_iterator ma = m_options.trace_multiactions.begin(); ma != m_options.trace_multiactions.end(); ++ma)
  {
    if (*ma == transition.action())
    {
      save_actions(source_state, transition);
    }
  }
}

// The function below checks whether in the set of outgoing transitions,
// there are two transitions with the same label, going to different states.
// If this is the case, true is delivered and one nondeterministic transition
// is returned in the variable nondeterministic_transition.
bool lps2lts_algorithm::is_nondeterministic(std::vector<lps2lts_algorithm::next_state_generator::transition_t>& transitions,
                                            lps2lts_algorithm::next_state_generator::transition_t& nondeterministic_transition)
{
  // Below a mapping from transition labels to target states is made.
  static std::map<lps::multi_action, lps::state> sorted_transitions; // The set is static to avoid repeated construction.
  assert(sorted_transitions.empty());
  for(const lps2lts_algorithm::next_state_generator::transition_t& t: transitions)
  {
    const std::map<lps::multi_action, lps::state>::const_iterator i=sorted_transitions.find(t.action());
    if (i!=sorted_transitions.end())
    {
      if (i->second!=t.target_state())
      {
        // Two transitions with the same label and different targets states have been found. This state is nondeterministic.
        sorted_transitions.clear();
        nondeterministic_transition=t;
        return true;
      }
    }
    else
    {
      sorted_transitions[t.action()]=t.target_state();
    }
  }
  sorted_transitions.clear();
  return false;
}

void lps2lts_algorithm::get_transitions(const lps::state& state,
                                        std::vector<lps2lts_algorithm::next_state_generator::transition_t>& transitions,
                                        next_state_generator::enumerator_queue_t& enumeration_queue
                                       )
{
  bool structured_output = true;
  assert(transitions.empty());
  if (m_options.detect_divergence)
  {
    if (non_divergent_states.count(state)==0)  // This state was not already investigated earlier.
    { if (m_options.trace)
      {
        // Onderstaande string generatie kan duur uitpakken.
        std::string filename_divergence_loop = m_options.trace_prefix + "_divergence_loop" + std::to_string(m_traces_saved) + ".trc";
        check_divergence<detail::counter_example_constructor>(
                detail::state_index_pair<detail::counter_example_constructor>(state,detail::counter_example_constructor::root_index()),
                detail::counter_example_constructor(filename_divergence_loop, structured_output));
      }
      else
      {
        check_divergence(
                detail::state_index_pair<detail::dummy_counter_example_constructor>(state,detail::dummy_counter_example_constructor::root_index()),
                detail::dummy_counter_example_constructor());
      }
    }
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
    if (m_options.outformat == lts_aut)
    {
      m_aut_file.flush();
    }
    exit(EXIT_FAILURE);
  }

  if (m_value_prioritize)
  {
    value_prioritize(transitions);
  }
  if (m_options.detect_deadlock && transitions.empty())
  {
    save_deadlock(state);
  }

  if (m_options.detect_nondeterminism)
  { lps2lts_algorithm::next_state_generator::transition_t nondeterministic_transition;
    if (is_nondeterministic(transitions, nondeterministic_transition))
    {
      // save the trace to the nondeterministic state and one transition to indicate
      // which transition is nondeterministic.
      save_nondeterministic_state(state, nondeterministic_transition);
    }
  }

  if (m_use_confluence_reduction)
  {
    for (std::vector<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
    {
      i->set_target_state(get_prioritised_representative(i->target_state()));
    }
  }
}

void lps2lts_algorithm::generate_lts_breadth_todo_max_is_npos(const next_state_generator::transition_t::state_probability_list& initial_states)
{
  assert(m_options.todo_max==std::string::npos);
  std::size_t current_state = 0;
  std::size_t start_level_seen = 1;
  std::size_t start_level_transitions = 0;
  std::vector<next_state_generator::transition_t> transitions;
  time_t last_log_time = time(nullptr) - 1, new_log_time;
  next_state_generator::enumerator_queue_t enumeration_queue;
  (void)initial_states; // suppress unused variable warning. 

  while (!m_must_abort && (current_state < m_state_numbers.size()) &&
         (current_state < m_options.max_states) && (!m_options.trace || m_traces_saved < m_options.max_traces))
  {
    lps::state state=m_state_numbers[current_state];
    get_transitions(state,transitions,enumeration_queue);
    for (const next_state_generator::transition_t& t: transitions)
    {
      add_transition(state, t, [](const lps::state&){});
    }
    transitions.clear();

    current_state++;
    if (current_state == start_level_seen)
    {
      mCRL2log(debug) << "Number of states at level " << m_level << " is " << m_num_states - start_level_seen << "\n";
      m_level++;
      start_level_seen = m_num_states;
      start_level_transitions = m_num_transitions;
    }

    if (!m_options.suppress_progress_messages && time(&new_log_time) > last_log_time)
    {
      last_log_time = new_log_time;
      std::size_t lvl_states = m_num_states - start_level_seen;
      std::size_t lvl_transitions = m_num_transitions - start_level_transitions;
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

void lps2lts_algorithm::generate_lts_breadth_todo_max_is_not_npos(const next_state_generator::transition_t::state_probability_list& initial_states)
{
  assert(m_options.todo_max!=std::string::npos);
  std::size_t current_state = 0;
  std::size_t start_level_seen = 1;
  std::size_t start_level_explored = 0;
  std::size_t start_level_transitions = 0;
  time_t last_log_time = time(nullptr) - 1, new_log_time;

  queue<lps::state> state_queue;
  state_queue.set_max_size(m_options.max_states < m_options.todo_max ? m_options.max_states : m_options.todo_max);
  for(next_state_generator::transition_t::state_probability_list::const_iterator i=initial_states.begin(); i!=initial_states.end(); ++i)
  {
    state_queue.add_to_queue(i->state());
  }
  state_queue.swap_queues();
  std::vector<next_state_generator::transition_t> transitions;
  next_state_generator::enumerator_queue_t enumeration_queue;

  while (!m_must_abort && (state_queue.remaining() > 0) &&
         (current_state < m_options.max_states) && (!m_options.trace || m_traces_saved < m_options.max_traces))
  {
    const lps::state state=state_queue.get_from_queue();
    get_transitions(state,transitions, enumeration_queue);

    for (std::vector<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
    {
      add_transition(state, *i, [&state_queue](const lps::state& s){ state_queue.add_to_queue(s);});
    }
    transitions.clear();

    current_state++;
    if (state_queue.remaining() == 0)
    {
      state_queue.swap_queues();

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
      std::size_t lvl_states = m_num_states - start_level_seen;
      std::size_t lvl_transitions = m_num_transitions - start_level_transitions;
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

void lps2lts_algorithm::generate_lts_breadth_bithashing(const next_state_generator::transition_t::state_probability_list& initial_states)
{
  std::size_t current_state = 0;

  std::size_t start_level_seen = 1;
  std::size_t start_level_explored = 0;
  std::size_t start_level_transitions = 0;

  queue<lps::state> state_queue;
  state_queue.set_max_size(m_options.max_states < m_options.todo_max ? m_options.max_states : m_options.todo_max);
  for(next_state_generator::transition_t::state_probability_list::const_iterator i=initial_states.begin(); i!=initial_states.end(); ++i)
  {
    state_queue.add_to_queue(i->state());
  }
  state_queue.swap_queues();
  std::vector<next_state_generator::transition_t> transitions;
  next_state_generator::enumerator_queue_t enumeration_queue;

  while (!m_must_abort && (state_queue.remaining() > 0) && (current_state < m_options.max_states) && (!m_options.trace || m_traces_saved < m_options.max_traces))
  {
    const lps::state state=state_queue.get_from_queue();
    get_transitions(state,transitions, enumeration_queue);

    for (std::vector<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
    {
      // It can be that a state is dropped in the queue, as the queue reached its todo-limit.
      // This is ignored in combination with bithashing. So, there might be states with outgoing
      // transitions of which the outgoing transitions are not investigated.
      add_transition(state, *i, [&state_queue](const lps::state& s){ state_queue.add_to_queue(s); });

      // if (add_transition(state, *i))
      // {
      //   // It can be that a state is dropped in the queue, as the queue reached its todo-limit.
      //   // This is ignored in combination with bithashing. So, there might be states with outgoing
      //   // transitions of which the outgoing transitions are not investigated.
      //   state_queue.add_to_queue(i->target_state());
      // }
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

void lps2lts_algorithm::generate_lts_depth(const next_state_generator::transition_t::state_probability_list& initial_states)
{
  std::list<lps::state> stack;
  for(next_state_generator::transition_t::state_probability_list::const_iterator i=initial_states.begin(); i!=initial_states.end(); ++i)
  {
    stack.push_back(i->state());
  }
  std::vector<next_state_generator::transition_t> transitions;

  std::size_t current_state = 0;
  next_state_generator::enumerator_queue_t enumeration_queue;

  while (!m_must_abort && (!stack.empty()) && (!m_options.trace || m_traces_saved < m_options.max_traces))
  {
    const lps::state state=stack.back();
    stack.pop_back();
    get_transitions(state,transitions, enumeration_queue);

    for (std::vector<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
    {
      add_transition(state, *i, 
          [&](const lps::state& s)
             { if ((current_state + stack.size() < m_options.max_states) && (stack.size() < m_options.todo_max))
               {  
                 stack.push_back(s);
               }
             });
      // if (add_transition(state, *i) && (current_state + stack.size() < m_options.max_states) && (stack.size() < m_options.todo_max))
      // {
      //  stack.push_back(i->target_state());
      // }
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

void lps2lts_algorithm::generate_lts_random(const next_state_generator::transition_t::state_probability_list& initial_states)
{
  if (++initial_states.begin()!=initial_states.end())  // There is more than one element in the set of initial states.
  {
    mCRL2log(warning) << "The initial state is not selected at random, conform its distribution. One specific state is chosen.";
  }
  lps::state state = initial_states.front().state();

  std::vector<next_state_generator::transition_t> transitions;
  std::size_t current_state = 0;
  next_state_generator::enumerator_queue_t enumeration_queue;

  while (!m_must_abort && current_state < m_options.max_states && (!m_options.trace || m_traces_saved < m_options.max_traces))
  {
    get_transitions(state,transitions, enumeration_queue);

    if (transitions.empty())
    {
      break;
    }

    std::size_t index = rand() % transitions.size();
    lps::state new_state;

    for (std::vector<next_state_generator::transition_t>::iterator i = transitions.begin(); i != transitions.end(); i++)
    {
      // Only the first state of a probabilistic state is randomly chosen. Not all states are considered. 
      add_transition(state, *i, [](const lps::state& ){});

      if (index-- == 0)
      {
        new_state = i->target_state();
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
