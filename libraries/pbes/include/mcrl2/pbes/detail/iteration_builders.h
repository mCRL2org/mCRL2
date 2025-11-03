// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/iteration_builders.h
/// \brief This file provides builders used by pbeschain
///        and pbesfixpointsolve.

#ifndef MCRL2_PBES_DETAIL_ITERATION_BUILDERS_H
#define MCRL2_PBES_DETAIL_ITERATION_BUILDERS_H

#include "mcrl2/data/bool.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"





namespace mcrl2::pbes_system::detail
{

// Substitutor to replace predicate variables OTHER than the self-referencing onces to functions from params to booleans
template <template <class> class Builder>
struct replace_other_propositional_variables_with_functions_builder
    : public Builder<replace_other_propositional_variables_with_functions_builder<Builder>>
{
  using super = Builder<replace_other_propositional_variables_with_functions_builder<Builder>>;
  using super::apply;

  core::identifier_string name;
  simplify_data_rewriter<data::rewriter> m_pbes_rewriter;
  data::variable_list var_list;
  std::map<data::data_expression, propositional_variable_instantiation> m_instantiations;
  bool forward = true;

  explicit replace_other_propositional_variables_with_functions_builder(simplify_data_rewriter<data::rewriter>& r)
      : m_pbes_rewriter(r)
  {}

  void set_name(const core::identifier_string& s) { name = s; }

  void set_forward(bool b) { forward = b; }

  void reset_variable_list() { var_list = data::variable_list({}); }
  data::variable_list get_variable_list() { return var_list; }

  template <class T>
  void apply(T& result, const pbes_expression& d)
  {
    if (forward)
    {
      super::apply(result, d);
      return;
    }
    if (!(data::is_variable(d) || data::is_application(d)))
    {
      super::apply(result, d);
      return;
    }

    if (data::is_variable(d))
    {
      data::variable da = atermpp::down_cast<data::variable>(d);

      for (const auto& var: var_list)
      {
        if (var.name() == da.name())
        {
          propositional_variable_instantiation pvi(da.name());
          result = pvi;
          return;
        }
      }
    }
    if (data::is_application(d))
    {
      data::application da = atermpp::down_cast<data::application>(d);

      da.head();

      if (data::is_function_symbol(da.head()))
      {
        data::function_symbol fname = atermpp::down_cast<data::function_symbol>(da.head());
        for (const auto& var: var_list)
        {
          if (var.name() == fname.name())
          {
            data::data_expression_list params;
            for (const auto& param: da)
            {
              params.push_front(param);
            }
            params = reverse(params);

            propositional_variable_instantiation pvi(fname.name(), params);
            result = pvi;
            return;
          }
        }

      }
    }
    super::apply(result, d);
  }

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    if (forward)
    {
      // This could be unsound, replacing a PVI instance with a function symbol + parameters is not proven.
      // Based on uninterpreted functions theory (for BDDs for instance)
      data::data_expression_list params = x.parameters();
      atermpp::aterm_list term_list;
      for (const auto& x : params)
      {
        atermpp::aterm sort(x.sort());
        term_list.push_front(sort);
      }
      term_list = reverse(term_list); 
      data::sort_expression_list sort_list(term_list);

      if (sort_list.size() > 0)
      {
        data::sort_expression sort_expr = data::function_sort(sort_list, data::sort_bool::bool_());
        data::function_symbol fs = data::function_symbol(x.name(), sort_expr);
        result = data::application(fs, x.parameters());
        data::variable var = data::variable(x.name(), sort_expr);
        var_list.push_front(var);
      }
      else
      {
        data::variable var = data::variable(x.name(), data::sort_bool::bool_());
        var_list.push_front(var);
        result = var;
      }
      return;
    }
    result = x;
  }
};

// Substitutor to target specific path, replace our specific pvi with a substituted CC
template <template <class> class Builder>
struct substitute_propositional_variables_for_bools_builder
    : public Builder<substitute_propositional_variables_for_bools_builder<Builder>>
{
  using super = Builder<substitute_propositional_variables_for_bools_builder<Builder>>;
  using super::apply;

  simplify_data_rewriter<data::rewriter> m_pbes_rewriter;
  propositional_variable_instantiation m_pvi;
  pbes_expression m_replacement;

  explicit substitute_propositional_variables_for_bools_builder(simplify_data_rewriter<data::rewriter>& r)
      : m_pbes_rewriter(r)
  {}

  void set_pvi(const propositional_variable_instantiation x) { m_pvi = x; }
  void set_replacement(const pbes_expression x) { m_replacement = x; }

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    if (x == m_pvi)
    {
      result = m_replacement;
    }
    else
    {
      result = true_();
    }
  }
};

// Substitutor for the self_substitution / iterations
template <template <class> class Builder>
struct substitute_propositional_variables_builder : public Builder<substitute_propositional_variables_builder<Builder>>
{
  using super = Builder<substitute_propositional_variables_builder<Builder>>;
  using super::apply;

  pbes_equation m_eq;
  core::identifier_string name;
  simplify_data_rewriter<data::rewriter> m_pbes_rewriter;
  data::data_specification data_spec;
  bool m_stable = false;

  explicit substitute_propositional_variables_builder(simplify_data_rewriter<data::rewriter>& r)
      : m_pbes_rewriter(r)
  {}

  void set_stable(bool b) { m_stable = b; }

  bool stable() const { return m_stable; }

  void set_equation(const pbes_equation& eq) { m_eq = eq; }

  void set_name(const core::identifier_string& s) { name = s; }

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    if (x.name() == m_eq.variable().name())
    {
      data::mutable_indexed_substitution sigma;
      data::data_expression_list pars = x.parameters();
      for (const data::variable& v : m_eq.variable().parameters())
      {
        data::data_expression par = pars.front();
        pars.pop_front();
        if (par.sort() != v.sort())
        {
          // Parameters do not match with variables. Ignore this substitution.
          result = x;
          return;
        }
        sigma[v] = par;
      }
      pbes_expression p = pbes_rewrite(m_eq.formula(), m_pbes_rewriter, sigma);
      std::set<propositional_variable_instantiation> set = find_propositional_variable_instantiations(p);
      if (std::all_of(set.begin(),
              set.end(),
              [this](const propositional_variable_instantiation& v) { return v.name() != m_eq.variable().name(); }))
      {
        result = p;
        m_stable = false;
        return;
      }
      result = x;
      return;
    }
    result = x;
  }
};

} // namespace mcrl2::pbes_system::detail





#endif
