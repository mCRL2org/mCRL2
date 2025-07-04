// Author(s): Jan Friso Groote based on the PBES one-point-rewriter by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/rewriters/one_point_rule_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_REWRITERS_ONE_POINT_RULE_REWRITER_H
#define MCRL2_PRES_REWRITERS_ONE_POINT_RULE_REWRITER_H

#include "mcrl2/data/consistency.h"
#include "mcrl2/data/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/pres/find_equalities.h"
#include "mcrl2/pres/replace.h"

namespace mcrl2::pres_system
{

/// \cond INTERNAL_DOCS
namespace detail {

template <typename Derived>
struct one_point_rule_rewrite_builder: public pres_system::pres_expression_builder<Derived>
{
  using super = pres_system::pres_expression_builder<Derived>;
  using super::apply;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  template <class T>
  void apply(T& result, const data::data_expression& x)
  {
    data::one_point_rule_rewriter r;
    result = r(x);
  }

  template <class T>
  void apply(T& result, const infimum& x)
  {
    pres_expression body;
    derived().apply(body, x.body());

    std::map<data::variable, std::set<data::data_expression> > inequalities = find_inequalities(body);
    mCRL2log(log::debug) << "x = " << x << std::endl;
    mCRL2log(log::debug) << "\ninequalities(body) = " << data::print_inequalities(inequalities) << std::endl;
    if (!inequalities.empty())
    {
      auto p = data::make_one_point_rule_substitution(inequalities, x.variables());
      data::mutable_map_substitution<>& sigma = p.first;
      const std::vector<data::variable>& remaining_variables = p.second;
      if (remaining_variables.size() != x.variables().size()) // one or more substitutions were found
      {
        mCRL2log(log::debug) << "Apply substitution sigma = " << sigma << " to x = " << body << std::endl;
        body = pres_system::replace_variables_capture_avoiding(body, sigma);
        mCRL2log(log::debug) << "sigma(x) = " << body << std::endl;
        if (remaining_variables.empty())
        {
          mCRL2log(log::debug) << "Replaced " << x << "\nwith " << body << std::endl;
          result = body;
          return;
        }
        data::variable_list v(remaining_variables.begin(), remaining_variables.end());
        mCRL2log(log::debug) << "Replaced " << x << "\nwith " << infimum(v, body) << std::endl;
        make_infimum(result, v, body);
        return;
      }
    }
    make_infimum(result, x.variables(), body);
  }

  template <class T>
  void apply(T& result, const supremum& x)
  {
    pres_expression body;
    derived().apply(body, x.body());

    std::map<data::variable, std::set<data::data_expression> > equalities = find_equalities(body);
    mCRL2log(log::debug) << "x = " << body << "\nequalities(x) = " << data::print_inequalities(equalities) << std::endl;
    if (!equalities.empty())
    {
      auto p = data::make_one_point_rule_substitution(equalities, x.variables());
      data::mutable_map_substitution<>& sigma = p.first;
      const std::vector<data::variable>& remaining_variables = p.second;
      if (remaining_variables.size() != x.variables().size()) // one or more substitutions were found
      {
        mCRL2log(log::debug) << "Apply substitution sigma = " << sigma << " to x = " << body << std::endl;
        body = pres_system::replace_variables_capture_avoiding(body, sigma);
        mCRL2log(log::debug) << "sigma(x) = " << body << std::endl;
        if (remaining_variables.empty())
        {
          mCRL2log(log::debug) << "Replaced " << x << "\nwith " << body << std::endl;
          result = body;
          return;
        }
        data::variable_list v(remaining_variables.begin(), remaining_variables.end());
        mCRL2log(log::debug) << "Replaced " << x << "\nwith " << supremum(v, body) << std::endl;
        make_supremum(result, v, body);
        return;
      }
    }
    make_supremum(result, x.variables(), body);
  }

  template <class T>
  void apply(T& result, const sum& x)
  {
    pres_expression body;
    derived().apply(body, x.body());

    std::map<data::variable, std::set<data::data_expression> > equalities = find_equalities(body);
    mCRL2log(log::debug) << "x = " << body << "\nequalities(x) = " << data::print_inequalities(equalities) << std::endl;
    if (!equalities.empty())
    {
      auto p = data::make_one_point_rule_substitution(equalities, x.variables());
      data::mutable_map_substitution<>& sigma = p.first;
      const std::vector<data::variable>& remaining_variables = p.second;
      if (remaining_variables.size() != x.variables().size()) // one or more substitutions were found
      {
        mCRL2log(log::debug) << "Apply substitution sigma = " << sigma << " to x = " << body << std::endl;
        body = pres_system::replace_variables_capture_avoiding(body, sigma);
        mCRL2log(log::debug) << "sigma(x) = " << body << std::endl;
        if (remaining_variables.empty())
        {
          mCRL2log(log::debug) << "Replaced " << x << "\nwith " << body << std::endl;
          result = body;
          return;
        }
        data::variable_list v(remaining_variables.begin(), remaining_variables.end());
        mCRL2log(log::debug) << "Replaced " << x << "\nwith " << sum(v, body) << std::endl;
        make_sum(result, v, body);
        return;
      }
    }
    make_sum(result, x.variables(), body);
  }

  // TODO: This case was added to prevent a data not to be transformed into a PRES not.
  // It should not be necessary to do this, but otherwise a PFNF test case fails.
  template <class T>
  void apply(T& result, const minus& x)
  {
    pres_expression operand; 
    derived().apply(operand, x.operand());
    /* if (data::is_data_expression(operand))
    {
      result = minus(atermpp::down_cast<data::data_expression>(operand));
    }
    else */
    {
      result = minus(operand);
    }
  }
};

} // namespace detail
/// \endcond

/// \brief A rewriter that applies one point rule quantifier elimination to a PRES.
class one_point_rule_rewriter
{
  public:
    /// \brief The term type
    using term_type = pres_expression;

    /// \brief The variable type
    using variable_type = data::variable;

    /// \brief Rewrites a pres expression.
    /// \param x A term
    /// \return The rewrite result.
    pres_expression operator()(const pres_expression& x) const
    {
      pres_expression result;
      core::make_apply_builder<detail::one_point_rule_rewrite_builder>().apply(result, x);
      return result;
    }
};

} // namespace mcrl2::pres_system

#endif // MCRL2_PRES_REWRITERS_ONE_POINT_RULE_REWRITER_H
