// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file realzone.cpp
/// \brief

#include <cmath>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/standard_numbers_utility.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/data/parse.h"

#include "realzone.h"
#include "inequality.h"
#include "bound.h"
#include "dbm.h"

namespace mcrl2
{
namespace data
{

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

  /// \brief Returns a list of all nonreal variables in l
  /// \param l a list of data variables
  /// \return The list of all v in l such that v.sort() != real()
  inline
  variable_list get_nonreal_variables(const variable_list& l)
  {
    variable_list r;
    for (variable_list::const_iterator i = l.begin(); i != l.end(); ++i)
    {
      if (i->sort() != sort_real::real_())
      {
        r.push_front(*i);
      }
    }
    return reverse(r);
  }

  static data_expression negate_inequality(const data_expression& e)
  {
    if (is_equal_to_application(e))
    {
      return not_equal_to(data::binary_left(atermpp::down_cast<application>(e)),data::binary_right(atermpp::down_cast<application>(e)));
    }
    if (is_not_equal_to_application(e))
    {
      return equal_to(data::binary_left(atermpp::down_cast<application>(e)),data::binary_right(atermpp::down_cast<application>(e)));
    }
    else if (is_less_application(e))
    {
      return greater_equal(data::binary_left(atermpp::down_cast<application>(e)),data::binary_right(atermpp::down_cast<application>(e)));
    }
    else if (is_less_equal_application(e))
    {
      return data::greater(data::binary_left(atermpp::down_cast<application>(e)),data::binary_right(atermpp::down_cast<application>(e)));
    }
    else if (is_greater_application(e))
    {
      return less_equal(data::binary_left(atermpp::down_cast<application>(e)),data::binary_right(atermpp::down_cast<application>(e)));
    }
    else if (is_greater_equal_application(e))
    {
      return data::less(data::binary_left(atermpp::down_cast<application>(e)),data::binary_right(atermpp::down_cast<application>(e)));
    }
    else
    {
      throw mcrl2::runtime_error("Expression " + data::pp(e) + " is expected to be an inequality over sort Real");
    }
  }
  /// \brief Determine whether a data expression is an inequality
  /// \param e A data expression
  /// \return true iff e is a data application of ==, <, <=, > or >= to
  ///      two arguments.
  inline
  bool is_inequality(data_expression e)
  {
    return is_equal_to_application(e) || is_less_application(e) ||
           is_less_equal_application(e) || is_greater_application(e) ||
           is_greater_equal_application(e);
  }

  static const assignment* assignment_list_get_var(assignment_list al, variable v)
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
  static int get_dbm_index(variable var, InputIt first, InputIt last)
  {
    // First element in DBM is 0
    for(int i = 1; first != last; first++, i++)
    {
      if(var == *first)
      {
        return i;
      }
    }
    assert(false);
    return -1;
  }

  static function_symbol comp_to_function(detail::comparison_t comp)
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

  static void dbm_add_inequalities(data_expression dbm, data_expression& new_dbm, data_expression& conditions, 
    std::vector< linear_inequality > inequalities, variable_list real_parameters, std::vector< variable > update_variables,
    std::set < variable > global_variables, rewriter r)
  {
    data_expression next_zone = sort_dbm::dbm_empty();
    std::vector< data_expression > new_conditions;

    std::vector< linear_inequality > postponed_inequalities;
    std::set< variable > restricted_vars;
    for(linear_inequality &li: inequalities)
    {
      int global_var_count = 0;
      int par_count = 0;
      int primed_par_count = 0;
      for(detail::lhs_t::const_iterator var = li.lhs_begin(); var != li.lhs_end(); var++)
      {
        if(global_variables.find(var->variable_name()) != global_variables.end())
        {
          global_var_count++;
        }
        else if(std::find(real_parameters.begin(), real_parameters.end(), var->variable_name()) != real_parameters.end())
        {
          par_count++;
        }
        else
        {
          primed_par_count++;
        }
      }
      std::cerr << pp(li) << " par count " << par_count << " primed par count " << primed_par_count << std::endl;

      if(par_count == 0 && primed_par_count > 1)
      {
        assert(primed_par_count <= 2);
        postponed_inequalities.push_back(li);
      }
      else if(primed_par_count == 0)
      {
        // Comparisons between more than two variables are not supported by DBMs
        assert(par_count <= 2);
        detail::variable_with_a_rational_factor first = li.lhs().front();
        int neg_index, pos_index;
        if(first.factor() == real_minus_one())
        {
          assert(li.lhs().tail().empty() || li.lhs().front().factor() == real_one());
          neg_index = get_dbm_index(first.variable_name(), real_parameters.begin(), real_parameters.end());
          pos_index = li.lhs().tail().empty() ? 0 : get_dbm_index(li.lhs().tail().front().variable_name(), real_parameters.begin(), real_parameters.end());
        }
        else
        {
          assert(first.factor() == real_one());
          assert(li.lhs().tail().empty() || li.lhs().front().factor() == real_minus_one());
          pos_index = get_dbm_index(first.variable_name(), real_parameters.begin(), real_parameters.end());
          neg_index = li.lhs().tail().empty() ? 0 : get_dbm_index(li.lhs().tail().front().variable_name(), real_parameters.begin(), real_parameters.end());
        }
        
        data_expression condition = sort_dbm::and_d(dbm, 
                    sort_nat::nat(pos_index), 
                    sort_nat::nat(neg_index), 
                    sort_bound::cbound(li.rhs(), comp_to_function(li.comparison())));
        if(li.comparison() == detail::equal)
        {
          condition = sort_dbm::and_d(condition,
                    sort_nat::nat(neg_index),
                    sort_nat::nat(pos_index),
                    sort_bound::cbound(li.rhs(), comp_to_function(li.comparison())));
        }
        std::cerr << "translation " << sort_bool::not_(sort_dbm::inconsistent(condition)) << std::endl;
        new_conditions.push_back(sort_bool::not_(sort_dbm::inconsistent(condition)));
      }
      else
      {
        assert(par_count <= 2);
        detail::lhs_t lhs = li.lhs();
        detail::variable_with_a_rational_factor v = *std::find_first_of(lhs.begin(), lhs.end(), update_variables.begin(), update_variables.end(),
                      [&](detail::variable_with_a_rational_factor a, variable b) { return a.variable_name() == b; });
        restricted_vars.insert(v.variable_name());
        detail::lhs_t lhs_norm = lhs.erase(v.variable_name());
        // std::cerr << "lhs " << pp(lhs_norm) << " variable name " << v.variable_name() << std::endl;
        lhs_norm = detail::multiply(lhs_norm, li.comparison() == detail::equal ? sort_real::times(v.factor(), real_minus_one()) : real_minus_one(), r);

        int lhs_size = lhs_norm.size();
        data_expression bound = sort_bound::cbound(li.rhs(), comp_to_function(li.comparison()));
        if(lhs_size > 0)
        {
          detail::variable_with_a_rational_factor first = lhs_norm.front();
          int neg_index, pos_index;
          if(first.factor() == real_minus_one())
          {
            assert(lhs_size == 1 || lhs_norm.tail().front().factor() == real_one());
            neg_index = get_dbm_index(first.variable_name(), real_parameters.begin(), real_parameters.end());
            pos_index = lhs_size == 1 ? 0 : get_dbm_index(lhs_norm.tail().front().variable_name(), real_parameters.begin(), real_parameters.end());
          }
          else
          {
            assert(first.factor() == real_one());
            assert(lhs_size == 1 || lhs_norm.tail().front().factor() == real_minus_one());
            pos_index = get_dbm_index(first.variable_name(), real_parameters.begin(), real_parameters.end());
            neg_index = lhs_size == 1 ? 0 : get_dbm_index(lhs_norm.tail().front().variable_name(), real_parameters.begin(), real_parameters.end());
          }
          bound = sort_bound::add(bound, sort_dbm::get(dbm, sort_nat::nat(pos_index), sort_nat::nat(neg_index)));
        }


        if(v.factor() == real_one() || li.comparison() == detail::equal)
        {
          // This linear inequality defines an upper bound for v
          next_zone = sort_dbm::and_d(next_zone, sort_nat::nat(get_dbm_index(v.variable_name(), update_variables.begin(), update_variables.end())), sort_nat::nat(0), bound);
          std::cerr << "translation " << sort_dbm::and_d(sort_dbm::dbm_empty(), sort_nat::nat(get_dbm_index(v.variable_name(), update_variables.begin(), update_variables.end())), sort_nat::nat(0), bound) << std::endl;
        }
        if(v.factor() == real_minus_one() || li.comparison() == detail::equal)
        {
          // This linear inequality defines a lower bound for v
          next_zone = sort_dbm::and_d(next_zone, sort_nat::nat(0), sort_nat::nat(get_dbm_index(v.variable_name(), update_variables.begin(), update_variables.end())), bound);
          std::cerr << "translation " << sort_dbm::and_d(sort_dbm::dbm_empty(), sort_nat::nat(0), sort_nat::nat(get_dbm_index(v.variable_name(), update_variables.begin(), update_variables.end())), bound) << std::endl;
        }
      }
    }

    if(!postponed_inequalities.empty())
    {
      for(variable v: restricted_vars)
      {
        std::cerr << "Restricted var " << v << std::endl;
      }
    }
    for(linear_inequality li: postponed_inequalities)
    {
      std::cerr << "Postponed inequality " << pp(li) << std::endl;
      detail::variable_with_a_rational_factor var = li.lhs().front();
      int restricted_var_count = 0;
      for(detail::variable_with_a_rational_factor v: li.lhs())
      {
        if(restricted_vars.find(v.variable_name()) == restricted_vars.end() && global_variables.find(v.variable_name()) == global_variables.end())
        {
          std::cerr << v << std::endl;
          var = v;
        }
      }
      std::cerr << "found variable " << var << std::endl;
      detail::lhs_t lhs_norm = li.lhs().erase(var.variable_name());
      lhs_norm = detail::multiply(lhs_norm, li.comparison() == detail::equal ? sort_real::times(var.factor(), real_minus_one()) : real_minus_one(), r);
      assert(lhs_norm.size() <= 1);
      if(li.comparison() == detail::equal)
      {
        if(lhs_norm.size() == 1)
        {
          if(global_variables.find(lhs_norm.front().variable_name()) != global_variables.end())
          {
            next_zone = sort_dbm::reset(next_zone, sort_nat::nat(get_dbm_index(var.variable_name(), update_variables.begin(), update_variables.end())), lhs_norm.front().variable_name());
            std::cerr << "translation " << sort_dbm::reset(sort_dbm::dbm_empty(), sort_nat::nat(get_dbm_index(var.variable_name(), update_variables.begin(), update_variables.end())), lhs_norm.front().variable_name()) << std::endl;
          }
          else
          {
            next_zone = sort_dbm::copy(next_zone, sort_nat::nat(get_dbm_index(var.variable_name(), update_variables.begin(), update_variables.end())), sort_nat::nat(get_dbm_index(lhs_norm.front().variable_name(), update_variables.begin(), update_variables.end())));
            std::cerr << "variable " << var.variable_name() << " lhs front " << lhs_norm.front().variable_name() << std::endl;
            std::cerr << "translation " << sort_dbm::copy(sort_dbm::dbm_empty(), sort_nat::nat(get_dbm_index(var.variable_name(), update_variables.begin(), update_variables.end())), sort_nat::nat(get_dbm_index(lhs_norm.front().variable_name(), update_variables.begin(), update_variables.end()))) << std::endl;
          }
        }
        else
        {
          next_zone = sort_dbm::reset(next_zone, sort_nat::nat(get_dbm_index(var.variable_name(), update_variables.begin(), update_variables.end())), li.rhs());
          std::cerr << "translation " << sort_dbm::reset(sort_dbm::dbm_empty(), sort_nat::nat(get_dbm_index(var.variable_name(), update_variables.begin(), update_variables.end())), li.rhs()) << std::endl;
        }
      }
      else
      {
        data_expression bound;
        if(lhs_norm.size() == 1)
        {
          if(global_variables.find(lhs_norm.front().variable_name()) != global_variables.end())
          {
            bound = sort_bound::cbound(lhs_norm.front().variable_name(), comp_to_function(li.comparison()));
          }
          else
          {
            assert(false);
          }
        }
        else
        {
          bound = sort_bound::cbound(li.rhs(), comp_to_function(li.comparison()));
        }
        if(var.factor() == real_one())
        {
          next_zone = sort_dbm::and_d(next_zone, sort_nat::nat(get_dbm_index(var.variable_name(), update_variables.begin(), update_variables.end())), sort_nat::nat(0), bound);
          std::cerr << "translation " << sort_dbm::and_d(sort_dbm::dbm_empty(), sort_nat::nat(get_dbm_index(var.variable_name(), update_variables.begin(), update_variables.end())), sort_nat::nat(0), bound) << std::endl;
        }
        else
        {
          next_zone = sort_dbm::and_d(next_zone, sort_nat::nat(0), sort_nat::nat(get_dbm_index(var.variable_name(), update_variables.begin(), update_variables.end())), bound);
          std::cerr << "translation " << sort_dbm::and_d(sort_dbm::dbm_empty(), sort_nat::nat(0), sort_nat::nat(get_dbm_index(var.variable_name(), update_variables.begin(), update_variables.end())), bound) << std::endl;
        }
      }
    }

    conditions = lazy::join_and(new_conditions.begin(), new_conditions.end());
    new_dbm = next_zone;
  }

  static void abstract_reals(
          stochastic_specification& s,
          const rewriter& r,
          std::vector < summand_information >& summand_info)
  {
    set_identifier_generator var_gen;
    var_gen.add_identifiers(lps::find_identifiers((s)));

    const variable_list real_parameters = get_real_variables(s.process().process_parameters());
    s.data().add_equation(data_equation(variable_list(),sort_dbm::N(),sort_nat::nat(real_parameters.size() + 1)));
    variable dbm_var(var_gen("z"), normalize_sorts(sort_dbm::dbm(), s.data()));
    std::vector< variable > update_variables;
    for(variable_list::iterator real_par = real_parameters.begin(); real_par != real_parameters.end(); real_par++)
    {
      variable par_prime(var_gen(static_cast<std::string>(real_par->name()) + "'"), sort_real::real_());
      update_variables.push_back(par_prime);
    }

    std::vector< summand_information > si = summand_info;
    summand_info.clear();
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

      for(linear_inequality &li: inequalities)
      {
        std::cerr << "orginal system " << pp(li) << std::endl;
      }
      variable_list eliminate_variables = summand.get_real_summation_variables();
      for(variable v: s.global_variables())
      {
        inequalities.push_back(linear_inequality(equal_to(v, real_zero()), r));
        eliminate_variables.push_front(v);
      }
      std::vector< linear_inequality > new_inequalities;
      fourier_motzkin(inequalities, eliminate_variables.begin(), eliminate_variables.end(), new_inequalities, r);

      // std::cerr << &summand << std::endl;
      for(linear_inequality &li: new_inequalities)
      {
        std::cerr << "reduced system " << pp(li) << std::endl;
      }

      data_expression next_zone, new_conditions;
      dbm_add_inequalities(dbm_var, next_zone, new_conditions, new_inequalities, real_parameters, update_variables, s.global_variables(), r);

      data_expression cond = sort_bool::and_(summand.get_summand().condition(), sort_bool::not_(sort_dbm::inconsistent(next_zone)));
      if(new_conditions != sort_bool::true_())
      {
        cond = sort_bool::and_(cond, new_conditions);
      }
      summand_base t(summand.get_non_real_summation_variables(), cond);
      assignment_list al;
      for(assignment_list::const_iterator i = summand.get_assignments().begin(); i != summand.get_assignments().end(); i++)
      {
        if(i->lhs().sort() != sort_real::real_())
        {
          al.push_front(*i);
        }
      }
      al.push_front(assignment(dbm_var, next_zone));
      summand_info.push_back(summand_information(t,
                                    summand.is_delta_summand(),
                                    al,
                                    summand.get_distribution(),
                                    summand.get_multi_action(),
                                    summand.get_deadlock(),
                                    variable_list(),
                                    summand.get_non_real_summation_variables(),
                                    std::vector< linear_inequality >(),
                                    std::map< variable, data_expression >()
                                    ));
    }
    variable_list proc_par = s.process().process_parameters();
    s.process().process_parameters() = variable_list();
    for(variable_list::const_iterator i = proc_par.begin(); i != proc_par.end(); i++)
    {
      if(i->sort() != sort_real::real_())
      {
        s.process().process_parameters().push_front(*i);
      }
    }
    s.process().process_parameters().push_front(dbm_var);

    assignment_list al;
    for(assignment_list::const_iterator i = s.initial_process().assignments().begin();
          i != s.initial_process().assignments().end(); i++)
    {
      if(i->lhs().sort() != sort_real::real_())
      {
        al.push_front(*i);
      }
    }
    al.push_front(assignment(dbm_var, normalize_sorts(sort_dbm::dbm_zero(), s.data())));
    s.initial_process() = stochastic_process_initializer(al, s.initial_process().distribution());
  }

  /// \brief Returns a list of all nonreal assignments in l
  /// \param l a list of data assignments
  /// \return The list of all x := e in l such that x.sort() == e.sort() != real()
  static inline
  assignment_list get_nonreal_assignments(const assignment_list& l)
  {
    assignment_list r;
    for (assignment_list::const_iterator i = l.begin(); i != l.end(); ++i)
    {
      if (i->lhs().sort() != sort_real::real_())
      {
        r.push_front(*i);
      }
    }
    return r;
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

  mcrl2::lps::stochastic_specification realzone(mcrl2::lps::stochastic_specification s,
                                             const rewrite_strategy strat)
  {
    rewriter r(s.data(),strat);
    std::vector < summand_information > summand_info;

    const variable_list real_parameters = get_real_variables(s.process().process_parameters());
    move_real_parameters_out_of_actions(s, real_parameters, r);
    normalize_specification(s, get_real_variables(s.process().process_parameters()), r, summand_info);

    s.data().add_system_defined_sort(sort_nat::nat());
    s.data().add_sort(sort_inequality::inequality());
    add_constructors(s.data(), sort_inequality::inequality_generate_constructors_code());
    add_equations(s.data(), sort_inequality::inequality_generate_equations_code());
    s.data().add_sort(sort_bound::bound());
    add_constructors(s.data(), sort_bound::bound_generate_constructors_code());
    add_mappings(s.data(), sort_bound::bound_generate_functions_code());
    add_equations(s.data(), sort_bound::bound_generate_equations_code());
    s.data().add_alias(alias(sort_dbm::dbm(),sort_list::list(sort_list::list(sort_bound::bound()))));
    add_mappings(s.data(), sort_dbm::dbm_generate_functions_code());
    add_equations(s.data(), sort_dbm::dbm_generate_equations_code());

    // data_specification my_data;
    // my_data.add_system_defined_sort(sort_real::real_());
    // my_data.add_system_defined_sort(sort_nat::nat());
    // my_data.add_sort(sort_inequality::inequality());
    // add_constructors(my_data, sort_inequality::inequality_generate_constructors_code());
    // add_equations(my_data, sort_inequality::inequality_generate_equations_code());
    // my_data.add_sort(sort_bound::bound());
    // add_constructors(my_data, sort_bound::bound_generate_constructors_code());
    // add_mappings(my_data, sort_bound::bound_generate_functions_code());
    // add_equations(my_data, sort_bound::bound_generate_equations_code());
    // my_data.add_alias(alias(sort_dbm::dbm(),sort_list::list(sort_list::list(sort_bound::bound()))));
    // add_mappings(my_data, sort_dbm::dbm_generate_functions_code());
    // add_equations(my_data, sort_dbm::dbm_generate_equations_code());
    // std::cerr << (atermpp::aterm_appl) sort_nat::c0() << std::endl;
    // std::cerr << (atermpp::aterm_appl) sort_dbm::dbm_generate_equations_code()[1] << std::endl;
    // data_type_checker checker(my_data);
    // checker.print_context();
    // std::cerr << "All constructors" << std::endl;
    // for(function_symbol s: my_data.constructors())
    // {
    //   std::cerr << (atermpp::aterm_appl) s << std::endl;
    // }
    // std::cerr << checker.typechecked_data_specification() << std::endl;

    abstract_reals(s, r, summand_info);

    // data_expression term = parse_data_expression("and_d(dbm, 0, 0, bound(0,less))", std::set<variable>(), s.data());
    // rewriter r2(s.data());
    // std::cerr << "term " << term << " rewritten " << r2(term) << std::endl;
    // std::cerr << "term " << (atermpp::aterm_appl) term << " rewritten " << (atermpp::aterm_appl) r2(term) << std::endl;
    // term = normalize_sorts(sort_dbm::and_d(sort_dbm::dbm_empty(), sort_nat::c0(), sort_nat::c0(), sort_bound::cbound(real_zero(), sort_inequality::lt())),s.data());
    // std::cerr << "term " << term << " rewritten " << r2(term) << std::endl;
    // std::cerr << "term " << (atermpp::aterm_appl) term << " rewritten " << (atermpp::aterm_appl) r2(term) << std::endl;
    // for(data_equation d: find_equations(s.data(), parse_data_expression("dbm", std::set<variable>(), s.data())))
    // {
    //   std::cerr << "Equation " << d << " aterm " << (atermpp::aterm_appl) d << std::endl;
    // }
    // std::cerr << "Real definition " << (atermpp::aterm_appl) sort_dbm::dbm_generate_equations_code()[39] << std::endl;
    // std::cerr << r2(sort_dbm::dbm_empty()) << std::endl;
    // std::cerr << r2(sort_dbm::get(sort_dbm::dbm_empty(), sort_nat::nat(0), sort_nat::nat(0)), sort_bound::inf()) << std::endl;

    // std::cerr << (atermpp::aterm_appl) rewriter(s.data(),strat)(sort_dbm::dbm_empty()) << std::endl;

    stochastic_action_summand_vector action_summands;
    deadlock_summand_vector deadlock_summands;
    for(std::vector<summand_information>::iterator i = summand_info.begin(); i != summand_info.end(); i++) {
      if (i->is_delta_summand())
      {
        deadlock_summands.push_back(i->get_represented_deadlock_summand(s.data()));
      }
      else
      {
        action_summands.push_back(i->get_represented_action_summand(s.data()));
      }
    }
    stochastic_linear_process lps(s.process().process_parameters(),
                                  deadlock_summands,
                                  action_summands);
    lps::stochastic_specification spec1(
        s.data(),
        s.action_labels(),
        s.global_variables(),
        lps,
        s.initial_process());

    return spec1;
  }

} // namespace data
} // namespace mcrl2
