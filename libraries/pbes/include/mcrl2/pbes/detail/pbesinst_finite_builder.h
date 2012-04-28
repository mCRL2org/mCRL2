// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbesinst_finite_builder.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBESINST_FINITE_BUILDER_H
#define MCRL2_PBES_DETAIL_PBESINST_FINITE_BUILDER_H

#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/pbes_expr_builder.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

/// Visitor that applies a propositional variable substitution to a pbes expression.
template <typename DataRewriter, typename RenameFunction, typename Substitution>
struct pbesinst_finite_builder: public pbes_expr_builder<pbes_expression, Substitution>
{
  typedef core::term_traits<pbes_expression> tr;

  const DataRewriter& m_rewriter;
  RenameFunction& m_rename;
  const data::data_specification& m_dataspec;
  std::map<core::identifier_string, std::vector<data::variable> >& m_finite_variables;

  pbesinst_finite_builder(const DataRewriter& r, RenameFunction& rho, const data::data_specification& dataspec, std::map<core::identifier_string, std::vector<data::variable> >& finite_variables)
    : m_rewriter(r), m_rename(rho), m_dataspec(dataspec), m_finite_variables(finite_variables)
  {}

  /// \brief Computes the subset with variables of finite sort and infinite.
  // TODO: this should be done more efficiently, by avoiding aterm lists
  /// \param e A sequence of data expressions
  /// \param data A data specification
  /// \param finite A sequence of data expressions
  /// \param infinite A sequence of data expressions
  void split_expressions(const data::data_expression_list& e,
                         const data::data_specification& data,
                         std::vector<data::data_expression>& finite,
                         std::vector<data::data_expression>& infinite
                        )
  {
    for (data::data_expression_list::iterator i = e.begin(); i != e.end(); ++i)
    {
      if (data.is_certainly_finite(i->sort()))
      {
        finite.push_back(*i);
      }
      else
      {
        infinite.push_back(*i);
      }
    }
  }

  template <typename VariableContainer, typename ExpressionContainer>
  data::data_expression make_condition(const VariableContainer& variables, const ExpressionContainer& expressions) const
  {
    using namespace data::sort_bool;
    data::data_expression result = true_();
    assert(variables.size() == expressions.size());
    typename VariableContainer::const_iterator vi = variables.begin();
    typename ExpressionContainer::const_iterator ei = expressions.begin();
    for (; vi != variables.end(); ++vi, ++ei)
    {
      result = and_(result, equals(*vi, *ei));
    }
    return result;
  }

  template <typename ExpressionContainer, typename Substitution1>
  data::data_expression_list rewrite(const ExpressionContainer& e, const Substitution1& sigma) const
  {
    std::vector<data::data_expression> result = atermpp::convert<std::vector<data::data_expression> >(e);
    for (std::vector<data::data_expression>::iterator i = e.begin(); i != e.end(); ++i)
    {
      *i = m_rewriter(*i, sigma);
    }
    return atermpp::convert<data::data_expression_list>(result);
  }

  /// \brief Visit data_expression node
  /// \param e A PBES expression
  /// \param d A data expression
  /// \return The result of visiting the node
  pbes_expression visit_data_expression(const pbes_expression& e, const data::data_expression& d, Substitution& sigma)
  {
    return data::replace_free_variables(d, sigma);
  }

  /// \brief Visit propositional_variable node
  /// \param x A term
  /// \return The result of visiting the node
  pbes_expression visit_propositional_variable(const pbes_expression& x, const propositional_variable_instantiation& v, Substitution& sigma)
  {
    std::vector<data::data_expression> finite_expressions;
    std::vector<data::data_expression> infinite_expressions;
    split_expressions(v.parameters(), m_dataspec, finite_expressions, infinite_expressions);

    const std::vector<data::variable>& finite_variables = m_finite_variables[v.name()];
    data::data_expression condition = make_condition(finite_variables, finite_expressions);

    std::set<pbes_expression> result;
    data::classic_enumerator<> enumerator(m_dataspec,m_rewriter);
    for (data::classic_enumerator<>::iterator i=enumerator.begin(finite_variables, m_rewriter); 
              i != enumerator.end(); ++i)
    {
      data::data_expression c = (*i)(condition);
      data::data_expression_list e_finite = rewrite(finite_expressions, *i);
      core::identifier_string Y = m_rename(v.name(), e_finite);
      data::data_expression_list e_infinite = rewrite(infinite_expressions, *i);
      result.insert(tr::and_(c, propositional_variable_instantiation(Y, e_infinite)));
    }

    return pbes_expr::join_or(result.begin(), result.end());
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBESINST_FINITE_BUILDER_H
