// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/one_point_rule_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_ONE_POINT_RULE_REWRITER_H
#define MCRL2_PBES_REWRITERS_ONE_POINT_RULE_REWRITER_H

#include "mcrl2/data/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/find_equalities.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/utilities/logger.h"
#include <cassert>

namespace mcrl2 {

namespace pbes_system {

/// \cond INTERNAL_DOCS
namespace detail {

template <typename Derived>
struct one_point_rule_rewrite_builder: public pbes_system::pbes_expression_builder<Derived>
{
  typedef pbes_system::pbes_expression_builder<Derived> super;
  using super::apply;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  data::data_expression apply(const data::data_expression& x)
  {
    data::one_point_rule_rewriter r;
    return r(x);
  }

  pbes_expression apply(const forall& x)
  {
    pbes_expression body = derived().apply(forall(x).body());

    std::map<data::variable, std::set<data::data_expression> > inequalities = find_inequalities(body);
    mCRL2log(log::debug) << "x = " << x << std::endl;
    mCRL2log(log::debug) << "\ninequalities(body) = " << data::print_inequalities(inequalities) << std::endl;
    if (!inequalities.empty())
    {
      auto p = data::detail::make_one_point_rule_substitution(inequalities, x.variables());
      data::mutable_map_substitution<>& sigma = p.first;
      const std::vector<data::variable>& remaining_variables = p.second;
      if (remaining_variables.size() != x.variables().size()) // one or more substitutions were found
      {
        mCRL2log(log::debug) << "Apply substitution sigma = " << sigma << " to x = " << body << std::endl;
        body = pbes_system::replace_variables_capture_avoiding(body, sigma, substitution_variables(sigma));
        mCRL2log(log::debug) << "sigma(x) = " << body << std::endl;
        if (remaining_variables.empty())
        {
          mCRL2log(log::debug) << "Replaced " << x << "\nwith " << body << std::endl;
          return body;
        }
        data::variable_list v(remaining_variables.begin(), remaining_variables.end());
        mCRL2log(log::debug) << "Replaced " << x << "\nwith " << forall(v, body) << std::endl;
        return forall(v, body);
      }
    }
    return forall(x.variables(), body);
  }

  pbes_expression apply(const exists& x)
  {
    pbes_expression body = derived().apply(exists(x).body());

    std::map<data::variable, std::set<data::data_expression> > equalities = find_equalities(body);
    mCRL2log(log::debug) << "x = " << body << "\nequalities(x) = " << data::print_inequalities(equalities) << std::endl;
    if (!equalities.empty())
    {
      auto p = data::detail::make_one_point_rule_substitution(equalities, x.variables());
      data::mutable_map_substitution<>& sigma = p.first;
      const std::vector<data::variable>& remaining_variables = p.second;
      if (remaining_variables.size() != x.variables().size()) // one or more substitutions were found
      {
        mCRL2log(log::debug) << "Apply substitution sigma = " << sigma << " to x = " << body << std::endl;
        body = pbes_system::replace_variables_capture_avoiding(body, sigma, substitution_variables(sigma));
        mCRL2log(log::debug) << "sigma(x) = " << body << std::endl;
        if (remaining_variables.empty())
        {
          mCRL2log(log::debug) << "Replaced " << x << "\nwith " << body << std::endl;
          return body;
        }
        data::variable_list v(remaining_variables.begin(), remaining_variables.end());
        mCRL2log(log::debug) << "Replaced " << x << "\nwith " << exists(v, body) << std::endl;
        return exists(v, body);
      }
    }
    return exists(x.variables(), body);
  }

  // TODO: This case was added to prevent a data not to be transformed into a PBES not.
  // It should not be necessary to do this, but otherwise a PFNF test case fails.
  pbes_expression apply(const not_& x)
  {
    pbes_expression operand = derived().apply(x.operand());
    if (data::is_data_expression(operand))
    {
      return data::sort_bool::not_(atermpp::down_cast<data::data_expression>(operand));
    }
    else
    {
      return not_(operand);
    }
  }
};

} // namespace detail
/// \endcond

/// \brief A rewriter that applies one point rule quantifier elimination to a PBES.
class one_point_rule_rewriter
{
  public:
    /// \brief The term type
    typedef pbes_expression term_type;

    /// \brief The variable type
    typedef data::variable variable_type;

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    pbes_expression operator()(const pbes_expression& x) const
    {
      return core::make_apply_builder<detail::one_point_rule_rewrite_builder>().apply(x);
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_ONE_POINT_RULE_REWRITER_H
