// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file next_state_generator.cpp

#include <set>
#include <algorithm>

#include "mcrl2/lps/next_state_generator.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;

// First we provide two classes, that represent lambdas.
class rewriter_class
{
  protected:
    const rewriter& m_r;
    mutable_indexed_substitution<>& m_sigma;

  public:
    rewriter_class(const rewriter& r, mutable_indexed_substitution<>& sigma)
      :  m_r(r),
         m_sigma(sigma)
    {}

    const data_expression operator()(const data_expression& t) const
    {
      return m_r(t,m_sigma);
    }
};

class state_applier
{
  protected:
    const state& m_state;
    const size_t m_size;

  public:
    state_applier(const state& state, const size_t size)
      :  m_state(state),
         m_size(size)
    {}

    const data_expression& operator()(const size_t n) const
    {
      return m_state.element_at(n,m_size);
    }
};

next_state_generator::next_state_generator(
  const stochastic_specification& spec,
  const data::rewriter& rewriter,
  bool use_enumeration_caching,
  bool use_summand_pruning)
  : m_specification(spec),
    m_rewriter(rewriter),
    m_enumerator(m_rewriter, m_specification.data(), m_rewriter,(std::numeric_limits<std::size_t>::max)(),true),  // Generate exceptions.
    m_use_enumeration_caching(use_enumeration_caching)
{
  m_process_parameters = data::variable_vector(m_specification.process().process_parameters().begin(), m_specification.process().process_parameters().end());

  if(m_specification.process().has_time())
  {
    mCRL2log(log::warning) << "specification uses time, which is (currently) not supported; ignoring timing" << std::endl;
  }

  for (stochastic_action_summand_vector::iterator i = m_specification.process().action_summands().begin(); 
                 i != m_specification.process().action_summands().end(); i++)
  {
    summand_t summand;
    summand.summand = &(*i);
    summand.variables =  order_variables_to_optimise_enumeration(i->summation_variables(),spec.data());
    summand.condition = i->condition();
    const data_expression_list& l=i->next_state(m_specification.process().process_parameters());
    summand.distribution = i->distribution();
    summand.result_state = data_expression_vector(l.begin(),l.end());

    for (process::action_list::const_iterator j = i->multi_action().actions().begin(); j != i->multi_action().actions().end(); j++)
    {
      action_internal_t action_label;
      action_label.label = j->label();

      for (data_expression_list::iterator k = j->arguments().begin(); k != j->arguments().end(); k++)
      {
        action_label.arguments.push_back(*k);
      }

      summand.action_label.push_back(action_label);
    }

    for (size_t j = 0; j < m_process_parameters.size(); j++)
    {
      if (data::search_free_variable(i->condition(), m_process_parameters[j]))
      {
        summand.condition_parameters.push_back(j);
      }
    }
    summand.condition_arguments_function = atermpp::function_symbol("condition_arguments", summand.condition_parameters.size());
    std::vector<atermpp::aterm_int> dummy(summand.condition_arguments_function.arity(), atermpp::aterm_int(static_cast<size_t>(0)));
    summand.condition_arguments_function_dummy = atermpp::aterm_appl(summand.condition_arguments_function, dummy.begin(), dummy.end());

    m_summands.push_back(summand);
  }

  data::data_expression_list initial_state_raw = m_specification.initial_process().state(m_specification.process().process_parameters());

  mutable_indexed_substitution<> sigma;
  rewriter_class r(m_rewriter,m_substitution);
  data::data_expression_vector initial_symbolic_state(initial_state_raw.begin(),initial_state_raw.end()); 
  m_initial_states = calculate_distribution(m_specification.initial_process().distribution(),
                                            initial_symbolic_state,
                                            sigma);

  m_all_summands = summand_subset_t(this, use_summand_pruning);
}

next_state_generator::~next_state_generator()
{}

next_state_generator::summand_subset_t::summand_subset_t(next_state_generator *generator, bool use_summand_pruning)
  : m_generator(generator),
    m_use_summand_pruning(use_summand_pruning)
{
  if (m_use_summand_pruning)
  {
    m_pruning_tree.summand_subset = atermpp::shared_subset<summand_t>(generator->m_summands);
    build_pruning_parameters(generator->m_specification.process().action_summands());
  }
  else
  {
    for (size_t i = 0; i < generator->m_summands.size(); i++)
    {
      m_summands.push_back(i);
    }
  }
}

bool next_state_generator::summand_subset_t::summand_set_contains(
            const std::set<stochastic_action_summand>& summand_set, 
            const next_state_generator::summand_t& summand)
{
  return summand_set.count(*summand.summand) > 0;
}

next_state_generator::summand_subset_t::summand_subset_t(
                next_state_generator *generator, 
                const stochastic_action_summand_vector& summands, bool use_summand_pruning)
  : m_generator(generator),
    m_use_summand_pruning(use_summand_pruning)
{
  std::set<stochastic_action_summand> summand_set;
  for (stochastic_action_summand_vector::const_iterator i = summands.begin(); i != summands.end(); i++)
  {
    summand_set.insert(*i);
  }

  if (m_use_summand_pruning)
  {
    atermpp::shared_subset<summand_t> full_set(generator->m_summands);
    m_pruning_tree.summand_subset =  atermpp::shared_subset<summand_t>(full_set, std::bind(next_state_generator::summand_subset_t::summand_set_contains, summand_set, std::placeholders::_1));
    build_pruning_parameters(summands);
  }
  else
  {
    for (size_t i = 0; i < generator->m_summands.size(); i++)
    {
      if (summand_set.count(*generator->m_summands[i].summand) > 0)
      {
        m_summands.push_back(i);
      }
    }
  }
}

static float condition_selectivity(const data_expression& e, const variable& v)
{
  if (sort_bool::is_and_application(e))
  {
    return condition_selectivity(data::binary_left(atermpp::down_cast<data::application>(e)), v)
        +  condition_selectivity(data::binary_right(atermpp::down_cast<data::application>(e)), v);
  }
  else if (sort_bool::is_or_application(e))
  {
    float sum = 0;
    size_t count = 0;
    std::list<data_expression> terms;
    terms.push_back(e);
    while (!terms.empty())
    {
      data_expression expression = terms.front();
      terms.pop_front();
      if (sort_bool::is_or_application(expression))
      {
        terms.push_back(data::binary_left(atermpp::down_cast<data::application>(e)));
        terms.push_back(data::binary_right(atermpp::down_cast<data::application>(e)));
      }
      else
      {
        sum += condition_selectivity(expression, v);
        count++;
      }
    }
    return sum / count;
  }
  else if (is_equal_to_application(e))
  {
    data_expression left = data::binary_left(atermpp::down_cast<data::application>(e));
    data_expression right = data::binary_right(atermpp::down_cast<data::application>(e));

    if (is_variable(left) && variable(left) == v)
    {
      return 1;
    }
    else if(is_variable(right) && variable(right) == v)
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    return 0;
  }
}

struct parameter_score
{
  size_t parameter_id;
  float score;

  parameter_score() {}

  parameter_score(size_t id, float score_)
    : parameter_id(id), score(score_)
  {}
};

static bool parameter_score_compare(const parameter_score& left, const parameter_score& right)
{
  return left.score > right.score;
}

void next_state_generator::summand_subset_t::build_pruning_parameters(const stochastic_action_summand_vector& summands)
{
  std::vector < parameter_score> parameters;

  for (size_t i = 0; i < m_generator->m_process_parameters.size(); i++)
  {
    parameters.push_back(parameter_score(i, 0));
    for (stochastic_action_summand_vector::const_iterator j = summands.begin(); j != summands.end(); j++)
    {
      parameters[i].score += condition_selectivity(j->condition(), m_generator->m_process_parameters[i]);
    }
  }

  std::sort(parameters.begin(), parameters.end(), parameter_score_compare);

  for (size_t i = 0; i < m_generator->m_process_parameters.size(); i++)
  {
    if (parameters[i].score > 0)
    {
      m_pruning_parameters.push_back(parameters[i].parameter_id);
      mCRL2log(log::verbose) << "using pruning parameter " << m_generator->m_process_parameters[parameters[i].parameter_id].name() << std::endl;
    }
  }
}

bool next_state_generator::summand_subset_t::is_not_false(const next_state_generator::summand_t& summand)
{
  return m_generator->m_rewriter(summand.condition, m_pruning_substitution) != data::sort_bool::false_();
}

atermpp::shared_subset<next_state_generator::summand_t>::iterator next_state_generator::summand_subset_t::begin(const state& state)
{
  assert(m_use_summand_pruning);

  for (size_t i = 0; i < m_pruning_parameters.size(); i++)
  {
    const variable& v=m_generator->m_process_parameters[m_pruning_parameters[i]];
    m_pruning_substitution[v] = v;
  }

  pruning_tree_node_t *node = &m_pruning_tree;
  for (size_t i = 0; i < m_pruning_parameters.size(); i++)
  {
    size_t parameter = m_pruning_parameters[i];
    data_expression argument = state.element_at(parameter,m_generator->m_process_parameters.size());
    m_pruning_substitution[m_generator->m_process_parameters[parameter]] = argument;
    std::map<data_expression, pruning_tree_node_t>::iterator position = node->children.find(argument);
    if (position == node->children.end())
    {
      pruning_tree_node_t child;
      child.summand_subset = atermpp::shared_subset<summand_t>(node->summand_subset, std::bind(&next_state_generator::summand_subset_t::is_not_false, this, std::placeholders::_1));
      node->children[argument] = child;
      node = &node->children[argument];
    }
    else
    {
      node = &position->second;
    }
  }

  return node->summand_subset.begin();
}



next_state_generator::iterator::iterator(next_state_generator *generator, const state& state, next_state_generator::substitution_t *substitution, summand_subset_t& summand_subset, enumerator_queue_t* enumeration_queue)
  : m_generator(generator),
    m_state(state),
    m_substitution(substitution),
    m_single_summand(false),
    m_use_summand_pruning(summand_subset.m_use_summand_pruning),
    m_summand(0),
    m_caching(false),
    m_enumeration_queue(enumeration_queue)
{
  if (m_use_summand_pruning)
  {
    m_summand_subset_iterator = summand_subset.begin(state);
  }
  else
  {
    m_summand_iterator = summand_subset.m_summands.begin();
    m_summand_iterator_end = summand_subset.m_summands.end();
  }

  size_t j=0;
  for (state::iterator i = state.begin(); i!=state.end(); ++i, ++j)
  {
    (*m_substitution)[generator->m_process_parameters[j]] = *i;
  }

  increment();
}

next_state_generator::iterator::iterator(next_state_generator *generator, const state& state, next_state_generator::substitution_t *substitution, size_t summand_index, enumerator_queue_t* enumeration_queue)
  : m_generator(generator),
    m_state(state),
    m_substitution(substitution),
    m_single_summand(true),
    m_single_summand_index(summand_index),
    m_use_summand_pruning(false),
    m_summand(0),
    m_caching(false),
    m_enumeration_queue(enumeration_queue)
{
  size_t j=0;
  for (state::iterator i = state.begin(); i!=state.end(); ++i, ++j)
  {
    (*m_substitution)[generator->m_process_parameters[j]] = *i;
  }

  increment();
}

struct is_not_zero
{ 
  // The argument intentionally does not have the type probabilistic_data_expression,
  // as this invokes == on probabilistic data expressions, which expects two fractions.
  // The enumerator can also generate open data expressions, which == on probabilistic_data_expressions
  // cannot handle. 
  bool operator()(const data_expression& x) const
  {
    assert(x.sort()==sort_real::real_());
    return x!=probabilistic_data_expression::zero(); 
  }
};

const next_state_generator::transition_t::state_probability_list next_state_generator::calculate_distribution(
                         const stochastic_distribution& dist,
                         const data::data_expression_vector& state_args,
                         substitution_t& sigma)
{
  rewriter_class r(m_rewriter,sigma);
  transition_t::state_probability_list resulting_state_probability_list;
  if (dist.variables().empty())
  {
    const lps::state target_state(state_args.begin(),state_args.size(),r);
    resulting_state_probability_list.push_front(state_probability_pair(target_state,probabilistic_data_expression::one()));
  }
  else
  {
    // Save the expressions for variables that occur in the distribution, and reset these in sigma.
    std::vector<data_expression> old_values_for_variables;
    old_values_for_variables.reserve(dist.variables().size());
    for(const variable& v: dist.variables())
    {
      old_values_for_variables.push_back(sigma(v));
      sigma[v]=v;
    }

    typedef enumerator_algorithm_with_iterator<rewriter, enumerator_list_element_with_substitution<>, is_not_zero> enumerator_type;
    const bool throw_exceptions=true;
    enumerator_type enumerator(m_rewriter, m_specification.data(), m_rewriter, 
                               data::detail::get_enumerator_variable_limit(), throw_exceptions);
    std::deque<enumerator_list_element_with_substitution<> > enumerator_solution_deque(1,enumerator_list_element_with_substitution<>(dist.variables(), dist.distribution()));
    for(enumerator_type::iterator probabilistic_solution = enumerator.begin(sigma, enumerator_solution_deque);
                                  probabilistic_solution != enumerator.end(); ++probabilistic_solution)
    {
      probabilistic_solution->add_assignments(dist.variables(),sigma,m_rewriter);
      rewriter_class r(m_rewriter,sigma);
      const lps::state target_state(state_args.begin(),state_args.size(),r);
      assert(probabilistic_solution->expression()==m_rewriter(dist.distribution(),sigma));
      if (atermpp::down_cast<probabilistic_data_expression>(probabilistic_solution->expression())>probabilistic_data_expression::zero())
      {
        resulting_state_probability_list.push_front(state_probability_pair(target_state,probabilistic_solution->expression()));
      }
      // Reset substitution
      for(const variable& v: dist.variables())
      {
        sigma[v]=v;
      }
    }
    
    // Set the old values of sigma back again.
    std::vector<data_expression>::const_iterator i=old_values_for_variables.begin();
    for(const variable& v: dist.variables())
    {
      sigma[v]=*i;
      assert(i!=old_values_for_variables.end());
      i++;
    }
  }
  return resulting_state_probability_list;
}



void next_state_generator::iterator::increment()
{
  while (!m_summand ||
         (m_cached && m_enumeration_cache_iterator == m_enumeration_cache_end) ||
         (!m_cached && m_enumeration_iterator == m_generator->m_enumerator.end())
        )
  {
    if (m_caching)
    {
      m_summand->enumeration_cache[m_enumeration_cache_key] = m_enumeration_log;
    }

    if (m_single_summand)
    {
      if (m_summand)
      {
        m_generator = 0;
        return;
      }
      m_summand = &(m_generator->m_summands[m_single_summand_index]);
    }
    else if (m_use_summand_pruning)
    {
      if (!m_summand_subset_iterator)
      {
        m_generator = 0;
        return;
      }
      m_summand = &(*m_summand_subset_iterator++);
    }
    else
    {
      if (m_summand_iterator == m_summand_iterator_end)
      {
        m_generator = 0;
        return;
      }
      m_summand = &(m_generator->m_summands[*m_summand_iterator++]);
    }

    if (m_generator->m_use_enumeration_caching)
    {
      state_applier apply_m_state(m_state,m_generator->m_process_parameters.size());
      m_enumeration_cache_key = condition_arguments_t(m_summand->condition_arguments_function,
                                                      m_summand->condition_parameters.begin(),
                                                      m_summand->condition_parameters.end(),
                                                      apply_m_state);

      std::map<condition_arguments_t, summand_enumeration_t>::iterator position = m_summand->enumeration_cache.find(m_enumeration_cache_key);
      if (position == m_summand->enumeration_cache.end())
      {
        m_cached = false;
        m_caching = true;
        m_enumeration_log.clear();
      }
      else
      {
        m_cached = true;
        m_caching = false;
        m_enumeration_cache_iterator = position->second.begin();
        m_enumeration_cache_end = position->second.end();
      }
    }
    else
    {
      m_cached = false;
      m_caching = false;
    }
    if (!m_cached)
    {
      for (data::variable_list::iterator i = m_summand->variables.begin(); i != m_summand->variables.end(); i++)
      {
        (*m_substitution)[*i] = *i;  // Reset the variable.
      }
      enumerate(m_summand->variables, m_summand->condition, *m_substitution);
    }
  }

  data_expression_list valuation;
  if (m_cached)
  {
    valuation = *m_enumeration_cache_iterator;
    m_enumeration_cache_iterator++;
    assert(valuation.size() == m_summand->variables.size());
    data_expression_list::iterator v = valuation.begin();
    for (variable_list::iterator i = m_summand->variables.begin(); i != m_summand->variables.end(); i++, v++)
    {
      (*m_substitution)[*i] = *v;
    }
  }
  else
  {
    m_enumeration_iterator->add_assignments(m_summand->variables,*m_substitution,m_generator->m_rewriter);

    // If we failed to exactly rewrite the condition to true, nextstate generation fails.
    if (m_enumeration_iterator->expression()!=sort_bool::true_())
    {
      assert(m_enumeration_iterator->expression()!=sort_bool::false_());

      // Reduce condition as much as possible, and give a hint of the original condition in the error message.
      data_expression reduced_condition(m_generator->m_rewriter(m_summand->condition, *m_substitution));
      std::string printed_condition(data::pp(m_summand->condition).substr(0, 300));

      throw mcrl2::runtime_error("Expression " + data::pp(reduced_condition) +
                                 " does not rewrite to true or false in the condition "
                                 + printed_condition
                                 + (printed_condition.size() >= 300?"...":""));
    }

    m_enumeration_iterator++;
    if (m_caching)
    {
      valuation=data_expression_list(m_summand->variables.begin(),m_summand->variables.end(),*m_substitution);
      assert(valuation.size() == m_summand->variables.size());
    }
  }

  if (m_caching)
  {
    m_enumeration_log.push_back(valuation);
  }

  // Calculate the effect of the distribution.

  const stochastic_distribution& dist=m_summand->distribution;
  if (dist.variables().empty())
  { 
    // There is no distribution, and therefore only one target state is generated
    const data_expression_vector& state_args=m_summand->result_state;
    rewriter_class r(m_generator->m_rewriter,*m_substitution);
    m_transition.set_target_state(lps::state(state_args.begin(),state_args.size(),r));
    m_transition.set_other_target_states(transition_t::state_probability_list());
  }
  else
  {
    // There is a non trivial distribution. We need to generate states and their probabilities.
    // The current implementation is inefficient, but efficiency is of a later concern.
    
    transition_t::state_probability_list resulting_state_probability_list=
                         m_generator->calculate_distribution(dist,m_summand->result_state,*m_substitution);
    if (resulting_state_probability_list.empty())
    {
      // There are no state probability pairs. But this is wrong. The total probabilities should add up to one.
      // This means there should at least be one probability. 
      rewriter_class r(m_generator->m_rewriter,*m_substitution);
      throw mcrl2::runtime_error("The distribution " + pp(r(dist.distribution())) + " has an empty set of instances.");
    }
    // Set one state as the resulting state, and leave the other states in the resulting_state_probability_list.
    m_transition.set_target_state(resulting_state_probability_list.front().state());
    resulting_state_probability_list.pop_front();
    m_transition.set_other_target_states(resulting_state_probability_list);
  }

  std::vector <process::action> actions;
  actions.resize(m_summand->action_label.size());
  std::vector < data_expression> arguments;
  for (size_t i = 0; i < m_summand->action_label.size(); i++)
  {
    arguments.resize(m_summand->action_label[i].arguments.size());
    for (size_t j = 0; j < m_summand->action_label[i].arguments.size(); j++)
    {
      arguments[j] = m_generator->m_rewriter(m_summand->action_label[i].arguments[j], *m_substitution);
    }
    actions[i] = process::action(m_summand->action_label[i].label, data_expression_list(arguments.begin(), arguments.end()));
  }
  m_transition.set_action(multi_action(process::action_list(actions.begin(), actions.end())));

  m_transition.set_summand_index(m_summand - &m_generator->m_summands[0]);

  for (variable_list::iterator i = m_summand->variables.begin(); i != m_summand->variables.end(); i++)
  {
    (*m_substitution)[*i] = *i;  // Reset the variable.
  }
}
