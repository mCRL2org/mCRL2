// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file realzone.h
/// \brief Implements translation of real parameters to difference bound matrices

#ifndef MCRL2_LPSREALZONE_REALZONE_H
#define MCRL2_LPSREALZONE_REALZONE_H


#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/data/fourier_motzkin.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/lps/stochastic_specification.h"

#include "dbm.h"
#include "summand_information.h"
#include "matrix_struct.h"

namespace mcrl2
{
namespace data
{


mcrl2::lps::stochastic_specification simplify(mcrl2::lps::stochastic_specification s,
                                             const rewrite_strategy strat=jitty);
void normalize_specification(
          const mcrl2::lps::stochastic_specification& s,
          const variable_list& real_parameters,
          const rewriter& r,
          std::vector < summand_information >& summand_info);
void move_real_parameters_out_of_actions(mcrl2::lps::stochastic_specification& s,
                                                  const variable_list& real_parameters,
                                                  const rewriter& r);

template <typename Specification>
class realzone_algorithm: public mcrl2::lps::detail::lps_algorithm<Specification>
{
  typedef typename mcrl2::lps::detail::lps_algorithm<Specification> super;
  typedef typename Specification::process_type process_type;
  typedef typename process_type::action_summand_type action_summand_type;
  using super::m_spec;

protected:
  struct bound_comp_s;

  const rewrite_strategy strat;
  rewriter r;
  std::vector<summand_information> summand_info;
  variable_list real_parameters;
  std::vector< variable > update_variables;
  std::set < variable > global_variables;

  std::map< std::pair< data_expression, data_expression >, std::set< data_expression, bound_comp_s >> split_bounds;
  std::vector< variable > sum_split_nats;

  /// \brief Returns a list of all real variables in l
  /// \param l a list of data variables
  /// \return The list of all v in l such that v.sort() == real()
  inline
  variable_list get_real_variables(const variable_list& l)
  {
    variable_list r;
    for (variable_list::const_iterator i = l.begin(); i != l.end(); ++i)
    {
      if (i->sort() == sort_real::real_())
      {
        r.push_front(*i);
      }
    }
    return reverse(r);
  }

  const assignment* assignment_list_get_var(assignment_list al, variable v)
  {
    for(assignment_list::iterator i = al.begin(); i != al.end(); i++)
    {
      if(i->lhs() == v)
      {
        return &(*i);
      }
    }
    return nullptr;
  }

  template< class InputIt >
  int get_dbm_index(variable var, InputIt first, InputIt last)
  {
    // First element in DBM is 0
    for(int i = 1; first != last; first++, i++)
    {
      if(var == *first)
      {
        return i;
      }
    }
    throw mcrl2::runtime_error("Variable " + pp(var) + " not found.");
    return -1;
  }

  function_symbol comp_to_function(detail::comparison_t comp)
  {
    switch (comp)
    {
      case detail::less:  return sort_inequality::lt();
      case detail::less_eq: return sort_inequality::le();
      case detail::equal: return sort_inequality::le();
    };
    assert(false);
    return sort_inequality::le(); //suppress compiler warning
  }

  template< class InputIt >
  std::pair< data_expression, data_expression > dbm_index_pair(std::vector< detail::variable_with_a_rational_factor > pars,
    InputIt begin, InputIt end, bool invert)
  {
    int neg_index = 0;
    int pos_index = 0;
    for(detail::variable_with_a_rational_factor p: pars)
    {
      if(p.factor() == real_one())
      {
        pos_index = get_dbm_index(p.variable_name(), begin, end);
      }
      else
      {
        neg_index = get_dbm_index(p.variable_name(), begin, end);
      }
    }
    std::pair< data_expression, data_expression > result(sort_nat::nat(pos_index), sort_nat::nat(neg_index));
    if(invert) {
      std::swap(result.first, result.second);
    }
    return result;
  }

  struct bound_comp_s
  {
    rewriter r;
    bool operator()(const data_expression b1, const data_expression b2) const
    {
      return r(sort_bound::lt(b1,b2)) == sort_bool::true_();
    }
    explicit bound_comp_s(rewriter rewr) : r(rewr) {}
  };

  void add_to_split_bounds(std::pair< data_expression, data_expression > dbm_indices, data_expression bound)
  {
    // ignore bounds with a zero
    if(sort_bound::left(bound) == real_zero())
    {
      return;
    }
    // to lower the amount of split bounds, we formulate them positively
    std::pair< data_expression, data_expression > key;
    data_expression value;
    if(r(less(dbm_indices.first, dbm_indices.second)) == sort_bool::true_())
    {
      key = std::make_pair(dbm_indices.second, dbm_indices.first);
      value = r(sort_bound::not_b(bound));
    }
    else
    {
      key = std::make_pair(dbm_indices.first, dbm_indices.second);
      value = bound;
    }
    struct bound_comp_s comparator(r);
    std::set< data_expression, bound_comp_s > empty_set(comparator);
    std::set< data_expression, bound_comp_s >& bounds_set = split_bounds.insert(std::make_pair(key, empty_set)).first->second;
    bounds_set.insert(value);
  }

  void categorise_variables(const linear_inequality li, std::vector< detail::variable_with_a_rational_factor >& pars,
    std::vector< detail::variable_with_a_rational_factor >& primed_pars)
  {
    for(detail::lhs_t::const_iterator var = li.lhs_begin(); var != li.lhs_end(); var++)
    {
      if(std::find(real_parameters.begin(), real_parameters.end(), var->variable_name()) != real_parameters.end())
      {
        pars.push_back(*var);
      }
      else if(std::find(update_variables.begin(), update_variables.end(), var->variable_name()) != update_variables.end())
      {
        primed_pars.push_back(*var);
      }
      else if(global_variables.find(var->variable_name()) == global_variables.end())
      {
        throw mcrl2::runtime_error("Variable " + pp(var->variable_name()) + " not found.");
      }
    }
  }

  data_expression li_to_bound(linear_inequality li, bool negate)
  {
    return sort_bound::cbound(negate ? r(sort_real::negate(li.rhs())) : li.rhs(), comp_to_function(li.comparison()));
  }

  data_expression li_vector_to_bound(std::vector< linear_inequality >& lis, const data_expression dbm,
    const std::map< std::pair< data_expression, data_expression >, data_expression > current_state_conditions)
  {
    data_expression bound;
    for(linear_inequality li: lis)
    {
      std::vector< detail::variable_with_a_rational_factor > pars;
      std::vector< detail::variable_with_a_rational_factor > primed_pars;
      categorise_variables(li, pars, primed_pars);

      std::cerr << pp(li) << " par count " << pars.size() << " primed par count " << primed_pars.size() << std::endl;
      std::pair< data_expression, data_expression > dbm_primed_indices = dbm_index_pair(primed_pars, update_variables.begin(), update_variables.end(), false);
      std::pair< data_expression, data_expression > dbm_indices = dbm_index_pair(pars, real_parameters.begin(), real_parameters.end(), true);

      data_expression bound2;
      if(pars.size() == 0)
      {
        bound2 = li_to_bound(li,false);
      }
      else
      {
        std::map< std::pair< data_expression, data_expression >, data_expression >::const_iterator extra_cond = current_state_conditions.find(dbm_indices);
        bound2 = sort_dbm::get(dbm, dbm_indices.first, dbm_indices.second);
        if(extra_cond != current_state_conditions.end())
        {
          bound2 = sort_bound::min_b(bound2, extra_cond->second);
        }
        bound2 = sort_bound::add(li_to_bound(li,false),bound2);
      }
      if(bound == data_expression())
      {
        bound = bound2;
      }
      else
      {
        bound = sort_bound::min_b(bound, bound2);
      }
    }
    return r(bound);
  }

  void dbm_add_translated_inequalities(const data_expression dbm, data_expression& new_dbm, data_expression& conditions, 
    std::vector< linear_inequality > inequalities)
  {
    data_expression next_zone = sort_dbm::dbm_empty();
    std::vector< data_expression > new_conditions;

    std::map< std::pair< data_expression, data_expression >, data_expression > current_state_conditions;
    std::map< std::pair< data_expression, data_expression > , std::vector< linear_inequality >> next_state_conditions;
    for(linear_inequality &li: inequalities)
    {
      std::vector< detail::variable_with_a_rational_factor > pars;
      std::vector< detail::variable_with_a_rational_factor > primed_pars;
      categorise_variables(li, pars, primed_pars);

      if(pars.size() > 2 || primed_pars.size() > 2)
      {
        throw mcrl2::runtime_error("Comparisons between more than two variables are not supported by DBMs: " + pp(li));
      }
      if(primed_pars.size() == 0)
      {
        std::cerr << pp(li) << " par count " << pars.size() << " primed par count " << primed_pars.size() << std::endl;
        std::pair< data_expression, data_expression > dbm_indices = dbm_index_pair(pars, real_parameters.begin(), real_parameters.end(), false);
        data_expression bound = li_to_bound(li,false);
        data_expression condition = sort_dbm::and_d(dbm, 
                    dbm_indices.first,
                    dbm_indices.second,
                    bound);
        current_state_conditions.insert(std::make_pair(dbm_indices, bound));
        if(li.comparison() == detail::equal)
        {
          data_expression bound = li_to_bound(li,true);
          condition = sort_dbm::and_d(condition,
                    dbm_indices.second,
                    dbm_indices.first,
                    bound);
          current_state_conditions.insert(std::make_pair(std::make_pair(dbm_indices.second, dbm_indices.first), bound));
        }
        std::cerr << "translation " << sort_bool::not_(sort_dbm::inconsistent(condition)) << std::endl;
        new_conditions.push_back(sort_bool::not_(sort_dbm::inconsistent(condition)));
      }
      else
      {
        std::pair< data_expression, data_expression > dbm_primed_indices = dbm_index_pair(primed_pars, update_variables.begin(), update_variables.end(), false);
        std::pair< std::map< std::pair< data_expression, data_expression > , std::vector< linear_inequality >>::iterator, bool >
          ins_result = next_state_conditions.insert(std::make_pair(dbm_primed_indices, std::vector< linear_inequality >()));
        ins_result.first->second.push_back(li);
        if(li.comparison() == detail::equal)
        {
          std::pair< std::map< std::pair< data_expression, data_expression > , std::vector< linear_inequality >>::iterator, bool >
            ins_result = next_state_conditions.insert(std::make_pair(std::make_pair(dbm_primed_indices.second, dbm_primed_indices.first), std::vector< linear_inequality >()));
          ins_result.first->second.push_back(linear_inequality(multiply(li.lhs(), real_minus_one(), r), r(sort_real::times(li.rhs(), real_minus_one())), detail::equal));
        }
      }
    }
    for(std::map< std::pair< data_expression, data_expression > , std::vector< linear_inequality >>::iterator ind_pair = next_state_conditions.begin();
      ind_pair != next_state_conditions.end(); ind_pair++)
    {
      if(!ind_pair->second.empty())
      {
        data_expression bound = li_vector_to_bound(ind_pair->second, dbm, current_state_conditions);
        std::map< std::pair< data_expression, data_expression > , std::vector< linear_inequality >>::iterator neg_conditions =
          next_state_conditions.find(std::make_pair(ind_pair->first.second, ind_pair->first.first));
        if(neg_conditions != next_state_conditions.end())
        {
          if(neg_conditions->second.empty())
          {
            throw mcrl2::runtime_error("Unexpected empty list of conditions for variables " + pp(ind_pair->first.second) + " and " + pp(ind_pair->first.first));
          }
          // There is also an opposite bound, so we can combine the expressions in one double_and_d
          data_expression bound2 = li_vector_to_bound(neg_conditions->second, dbm, current_state_conditions);
          next_zone = sort_dbm::double_and_d(next_zone, ind_pair->first.first, ind_pair->first.second, bound, bound2);
          std::cerr << "translation " << sort_dbm::double_and_d(sort_dbm::dbm_empty(), ind_pair->first.first, ind_pair->first.second, bound, bound2) << std::endl;
          // Empty the other list, so it will not get translated again
          neg_conditions->second.clear();
        }
        else
        {
          next_zone = sort_dbm::and_d(next_zone, ind_pair->first.first, ind_pair->first.second, bound);
          std::cerr << "translation " << sort_dbm::and_d(sort_dbm::dbm_empty(), ind_pair->first.first, ind_pair->first.second, bound) << std::endl;
        }
      }
    }

    conditions = lazy::join_and(new_conditions.begin(), new_conditions.end());
    new_dbm = next_zone;
  }

  data_expression dbm_add_splits(const data_expression post_z, data_expression& condition)
  {
    std::vector< variable >::const_iterator bool_var = sum_split_nats.begin();
    data_expression result = post_z;
    for(std::pair< std::pair< data_expression, data_expression >, std::set< data_expression, bound_comp_s >> sb: split_bounds)
    {
      std::pair< data_expression, data_expression > key = std::get<0>(sb);
      std::set< data_expression, bound_comp_s > value = std::get<1>(sb);
      data_expression bounds_list = sort_list::empty(sort_bound::bound());
      for(std::set< data_expression >::reverse_iterator bound = value.rbegin(); bound != value.rend(); bound++)
      {
        bounds_list = sort_list::cons_(sort_bound::bound())(*bound, bounds_list);
      }
      bounds_list = sort_list::cons_(sort_bound::bound())(sort_bound::inf(), bounds_list);
      result = sort_dbm::split_list(result, std::get<0>(key), std::get<1>(key), bounds_list, *bool_var);
      condition = lazy::and_(condition, less_equal(*bool_var, sort_nat::nat(value.size())));
      bool_var++;
    }
    return result;
  }

  void abstract_reals()
  {
    set_identifier_generator var_gen;
    var_gen.add_identifiers(lps::find_identifiers(m_spec));

    real_parameters = get_real_variables(m_spec.process().process_parameters());
    global_variables = m_spec.global_variables();
    m_spec.data().add_equation(data_equation(variable_list(),sort_dbm::N(),sort_nat::nat(real_parameters.size() + 1)));
    variable dbm_var(var_gen("z"), normalize_sorts(sort_dbm::dbm(), m_spec.data()));
    for(variable_list::iterator real_par = real_parameters.begin(); real_par != real_parameters.end(); real_par++)
    {
      variable par_prime(var_gen(static_cast<std::string>(real_par->name()) + "'"), sort_real::real_());
      update_variables.push_back(par_prime);
    }

    std::vector< summand_information > si = summand_info;
    summand_info.clear();
    std::vector< data_expression > dbm_exprs, new_conditions;
    for(summand_information &summand: si)
    {
      std::vector< linear_inequality > inequalities;
      std::vector< linear_inequality > real_conditions = summand.get_summand_real_conditions();
      inequalities.insert(inequalities.begin(), real_conditions.begin(), real_conditions.end());

      std::vector< variable >::iterator par_prime = update_variables.begin();
      for(variable_list::iterator real_par = real_parameters.begin(); real_par != real_parameters.end(); real_par++, par_prime++)
      {
        const assignment* a = assignment_list_get_var(summand.get_assignments(), *real_par);
        inequalities.push_back(linear_inequality(equal_to(*par_prime, a != nullptr ? a->rhs() : *real_par), r));
      }

      // for(linear_inequality &li: inequalities)
      // {
      //   std::cerr << "original system " << pp(li) << std::endl;
      // }
      variable_list eliminate_variables = summand.get_real_summation_variables();
      for(variable v: global_variables)
      {
        inequalities.push_back(linear_inequality(equal_to(v, real_zero()), r));
        eliminate_variables.push_front(v);
      }
      std::vector< linear_inequality > elim_inequalities;
      std::vector< linear_inequality > new_inequalities;
      fourier_motzkin(inequalities, eliminate_variables.begin(), eliminate_variables.end(), elim_inequalities, r);
      remove_redundant_inequalities(elim_inequalities, new_inequalities,r);

      // for(linear_inequality &li: new_inequalities)
      // {
      //   std::cerr << "reduced system " << pp(li) << std::endl;
      // }

      data_expression next_zone, new_condition;
      dbm_add_translated_inequalities(dbm_var, next_zone, new_condition, new_inequalities);
      dbm_exprs.push_back(next_zone);
      new_conditions.push_back(new_condition);

      // Now also eliminate next state variables to obtain splitting bounds
      elim_inequalities.clear();
      new_inequalities.clear();
      eliminate_variables = summand.get_real_summation_variables();
      for(variable v: update_variables)
      {
        eliminate_variables.push_front(v);
      }
      fourier_motzkin(real_conditions, eliminate_variables.begin(), eliminate_variables.end(), elim_inequalities, r);
      remove_redundant_inequalities(elim_inequalities, new_inequalities,r);
      for(linear_inequality i: new_inequalities)
      {
        std::vector< detail::variable_with_a_rational_factor > lhs_vars;
        for(detail::variable_with_a_rational_factor v: i.lhs())
        {
          lhs_vars.push_back(v);
        }
        // std::cerr << "bounds system " << pp(i) << std::endl;
        add_to_split_bounds(dbm_index_pair(lhs_vars, real_parameters.begin(), real_parameters.end(), false),
           sort_bound::cbound(i.rhs(), comp_to_function(i.comparison())));
      }
    }

    std::cerr << "List of split bounds" << std::endl;
    data_expression max_bound = real_zero();
    for(std::pair< std::pair< data_expression, data_expression >, std::set< data_expression, bound_comp_s >> sb: split_bounds)
    {
      std::pair< data_expression, data_expression > key = std::get<0>(sb);
      std::set< data_expression, bound_comp_s > value = std::get<1>(sb);
      data_expression bounds_list = sort_list::empty(sort_bound::bound());
      for(std::set< data_expression >::reverse_iterator bound = value.rbegin(); bound != value.rend(); bound++)
      {
        std::cerr << pp(std::get<0>(key)) << " - " << pp(std::get<1>(key)) << " " 
            << pp(sort_bound::right(*bound)) << " " << pp(sort_bound::left(*bound)) << std::endl;
        max_bound = sort_real::maximum(max_bound, sort_real::abs(sort_bound::left(*bound)));
      }
      sum_split_nats.push_back(variable(var_gen("split"), sort_nat::nat()));
    }
    max_bound = r(max_bound);
    std::cerr << "max bound " << pp(max_bound) << std::endl;

    std::vector< data_expression>::const_iterator next_zone = dbm_exprs.begin();
    std::vector< data_expression>::const_iterator new_condition = new_conditions.begin();
    for(summand_information &summand: si)
    {
      data_expression cond = summand.get_summand().condition();
      data_expression split_zone = dbm_add_splits(*next_zone, cond);
      cond = lazy::and_(cond, sort_bool::not_(sort_dbm::inconsistent(split_zone)));
      cond = lazy::and_(cond, *new_condition);
      variable_list nonreal_summation_variables = summand.get_non_real_summation_variables();
      for(variable v: sum_split_nats)
      {
        nonreal_summation_variables.push_front(v);
      }
      summand_base t(nonreal_summation_variables, cond);
      assignment_list al;
      for(assignment_list::const_iterator i = summand.get_assignments().begin(); i != summand.get_assignments().end(); i++)
      {
        if(i->lhs().sort() != sort_real::real_())
        {
          al.push_front(*i);
        }
      }
      al.push_front(assignment(dbm_var, sort_dbm::norm(split_zone, max_bound)));
      summand_info.push_back(summand_information(t,
                                    summand.is_delta_summand(),
                                    al,
                                    summand.get_distribution(),
                                    summand.get_multi_action(),
                                    summand.get_deadlock(),
                                    variable_list(),
                                    nonreal_summation_variables,
                                    std::vector< linear_inequality >(),
                                    std::map< variable, data_expression >()
                                    ));

      next_zone++;
      new_condition++;
    }
    variable_list proc_par = m_spec.process().process_parameters();
    m_spec.process().process_parameters() = variable_list();
    for(variable_list::const_iterator i = proc_par.begin(); i != proc_par.end(); i++)
    {
      if(i->sort() != sort_real::real_())
      {
        m_spec.process().process_parameters().push_front(*i);
      }
    }
    m_spec.process().process_parameters().push_front(dbm_var);

    assignment_list al;
    for(assignment_list::const_iterator i = m_spec.initial_process().assignments().begin();
          i != m_spec.initial_process().assignments().end(); i++)
    {
      if(i->lhs().sort() != sort_real::real_())
      {
        al.push_front(*i);
      }
    }
    al.push_front(assignment(dbm_var, normalize_sorts(sort_dbm::dbm_zero(), m_spec.data())));
    m_spec.initial_process() = stochastic_process_initializer(al, m_spec.initial_process().distribution());
  }

  void add_constructors(data_specification& d, function_symbol_vector cons) {
    for(function_symbol_vector::const_iterator con = cons.begin(); con != cons.end(); con++)
    {
      d.add_constructor(*con);
    }
  }

  void add_mappings(data_specification& d, function_symbol_vector maps) {
    for(function_symbol_vector::const_iterator map = maps.begin(); map != maps.end(); map++)
    {
      d.add_mapping(*map);
    }
  }

  void add_equations(data_specification& d, data_equation_vector eqns) {
    for(data_equation_vector::const_iterator eqn = eqns.begin(); eqn != eqns.end(); eqn++)
    {
      d.add_equation(*eqn);
    }
  }

  data_expression_vector make_dbm_elems_vector(int size, bool is_zero)
  {
    data_expression_vector elems;
    for(int i = 0; i < size; i++)
    {
      for(int j = 0; j < size; j++)
      {
        if(i == j || is_zero)
        {
          elems.push_back(sort_bound::cbound(real_zero(), sort_inequality::le()));
        }
        else
        {
          elems.push_back(sort_bound::inf());
        }
      }
    }
    return elems;
  }

  void add_dbm_definitions(int size)
  {

    m_spec.data().add_system_defined_sort(sort_nat::nat());
    m_spec.data().add_sort(sort_inequality::inequality());
    add_constructors(m_spec.data(), sort_inequality::inequality_generate_constructors_code());
    add_equations(m_spec.data(), sort_inequality::inequality_generate_equations_code());
    m_spec.data().add_sort(sort_bound::bound());
    add_constructors(m_spec.data(), sort_bound::bound_generate_constructors_code());
    add_mappings(m_spec.data(), sort_bound::bound_generate_functions_code());
    add_equations(m_spec.data(), sort_bound::bound_generate_equations_code());
    // m_spec.data().add_alias(alias(sort_dbm::dbm(),sort_list::list(sort_list::list(sort_bound::bound()))));
    matrix_struct ms(size, sort_bound::bound());
    // add_constructors(m_spec.data(),ms.get_definition().constructor_functions());
    // add_mappings(m_spec.data(),ms.get_definition().projection_functions());
    // add_equations(m_spec.data(),ms.get_definition().projection_equations());
    m_spec.data().add_alias(alias(sort_dbm::dbm(), ms.get_definition()));
    add_mappings(m_spec.data(), sort_dbm::dbm_generate_functions_code());
    add_equations(m_spec.data(), sort_dbm::dbm_generate_equations_code());

    m_spec.data().add_equation(data_equation(variable_list(), sort_dbm::dbm_empty(), application(ms.get_constructor(), make_dbm_elems_vector(size,false))));
    m_spec.data().add_equation(data_equation(variable_list(), sort_dbm::dbm_zero(), application(ms.get_constructor(), make_dbm_elems_vector(size,true))));

    variable vd("d",sort_dbm::dbm());
    for(int i = 0; i < size; i++)
    {
      for(int j = 0; j < size; j++)
      {
        m_spec.data().add_equation(data_equation(variable_list({vd}), sort_dbm::get(vd, sort_nat::nat(i), sort_nat::nat(j)), ms.get_projection_function(i,j)(vd)));
      }
    }

    variable vb("b", sort_bound::bound());
    variable_vector vv, vv_prime;
    for(int i = 0; i < size*size; i++)
    {
      variable var("b"+ std::to_string(i), sort_bound::bound());
      vv.push_back(var);
      vv_prime.push_back(var);
    }
    variable_vector vvb = vv;
    vvb.push_back(vb);
    std::cerr << "match dbm " << pp(ms.get_constructor()) << std::endl << std::flush;
    std::cerr << "match dbm " << pp(ms.get_constructor()(vv[0],vv[1])) << std::endl << std::flush;
    std::cerr << "match dbm " << pp(vv) << " " << pp(vvb) << " " << pp(vv_prime) << std::endl << std::flush;
    for(int i = 0; i < size; i++)
    {
      for(int j = 0; j < size; j++)
      {
        variable temp = vb;
        swap(vv_prime[i*size+j], temp);
        m_spec.data().add_equation(data_equation(vvb, 
          sort_dbm::set(application(ms.get_constructor(), vv), sort_nat::nat(i), sort_nat::nat(j), vb),
          application(ms.get_constructor(), vv_prime)));
        swap(vv_prime[i*size+j], temp);
      }
    }
  }

public:
  realzone_algorithm(Specification& spec, const rewrite_strategy st = jitty)
    : mcrl2::lps::detail::lps_algorithm<Specification>(spec),
    strat(st),
    r(spec.data(),strat)
  {
  }

  void run()
  {
    const variable_list real_parameters = get_real_variables(m_spec.process().process_parameters());
    move_real_parameters_out_of_actions(m_spec, real_parameters, r);
    normalize_specification(m_spec, get_real_variables(m_spec.process().process_parameters()), r, summand_info);
    add_dbm_definitions(real_parameters.size() + 1);

    r = rewriter(m_spec.data(), strat);
    abstract_reals();

    stochastic_action_summand_vector action_summands;
    deadlock_summand_vector deadlock_summands;
    for(std::vector<summand_information>::iterator i = summand_info.begin(); i != summand_info.end(); i++) {
      if (i->is_delta_summand())
      {
        deadlock_summands.push_back(i->get_represented_deadlock_summand(m_spec.data()));
      }
      else
      {
        action_summands.push_back(i->get_represented_action_summand(m_spec.data()));
      }
    }
    stochastic_linear_process lps(m_spec.process().process_parameters(),
                                  deadlock_summands,
                                  action_summands);
    m_spec.process() = lps;
  }
};

} // namespace data
} // namsepace mcrl2


#endif // MCRL2_LPSREALZONE_REALZONE_H
