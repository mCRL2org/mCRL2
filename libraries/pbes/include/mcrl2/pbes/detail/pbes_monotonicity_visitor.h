// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes_monotonicity_visitor.h
/// \brief Visitor that checks the monotonicity of a pbes expression.

#ifndef MCRL2_PBES_DETAIL_PBES_MONOTONICITY_VISITOR_H
#define MCRL2_PBES_DETAIL_PBES_MONOTONICITY_VISITOR_H

#include <vector>
#include "mcrl2/pbes/pbes_expression_visitor.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

enum pbes_monotonicity_value {
  pbes_monotonicity_positive,
  pbes_monotonicity_negative,
  pbes_monotonicity_both,
  pbes_monotonicity_none
};

template <typename Term>
struct pbes_monotonicity_visitor: public pbes_expression_visitor<Term>
{
  typedef pbes_expression_visitor<Term> super;
  typedef typename super::term_type term_type;
  typedef typename super::data_term_type data_term_type;
  typedef typename super::propositional_variable_type propositional_variable_type;

  /// \brief A stack containing monotonicity values of subterms.
  std::vector<pbes_monotonicity_value> m_monotonicity_values;

  /// \brief Return the top value and remove it from the stack.
  pbes_monotonicity_value pop()
  {
    pbes_monotonicity_value result = m_monotonicity_values.back();
    m_monotonicity_values.pop_back();
    return result;
  }

  /// \brief Implements shared functionality of and/or/imp.
  void do_binary_operator(pbes_monotonicity_value left, pbes_monotonicity_value right)
  {
    if (left == pbes_monotonicity_both)
    {
      m_monotonicity_values.push_back(right);
    }
    else if (right == pbes_monotonicity_both)
    {
      m_monotonicity_values.push_back(left);
    }
    else if (left == right)
    {
      m_monotonicity_values.push_back(left);
    }
    else
    {
      m_monotonicity_values.push_back(pbes_monotonicity_none);
    }
  }

  /// \brief Leave data expression node
  void leave_data_expression()
  {
    m_monotonicity_values.push_back(pbes_monotonicity_both);
  }

  /// \brief Leave true node
  void leave_true()
  {
    m_monotonicity_values.push_back(pbes_monotonicity_both);
  }

  /// \brief Leave false node
  void leave_false()
  {
    m_monotonicity_values.push_back(pbes_monotonicity_both);
  }

  /// \brief Leave not node
  void leave_not()
  {
    pbes_monotonicity_value b = pop();
    switch (b)
    {
      case pbes_monotonicity_both :
      {
        m_monotonicity_values.push_back(pbes_monotonicity_both);
        break;
      }
      case pbes_monotonicity_positive :
      {
        m_monotonicity_values.push_back(pbes_monotonicity_negative);
        break;
      }
      case pbes_monotonicity_negative :
      {
        m_monotonicity_values.push_back(pbes_monotonicity_positive);
        break;
      }
      default:
      {
        m_monotonicity_values.push_back(pbes_monotonicity_none);
        break;
      }
    }
  }

  /// \brief Leave and node
  void leave_and()
  {
    pbes_monotonicity_value right = pop();
    pbes_monotonicity_value left  = pop();
    do_binary_operator(left, right);
  }

  /// \brief Leave or node
  void leave_or()
  {
    pbes_monotonicity_value right = pop();
    pbes_monotonicity_value left  = pop();
    do_binary_operator(left, right);
  }

  /// \brief Leave imp node
  void leave_imp()
  {
    pbes_monotonicity_value right = pop();
    pbes_monotonicity_value left  = pop();
    
    // apply the effect of the negating the left hand side
    if (left == pbes_monotonicity_negative)
    {
      left = pbes_monotonicity_positive;
    }
    else if (left == pbes_monotonicity_positive)
    {
      left = pbes_monotonicity_negative;
    }

    do_binary_operator(left, right);
  }

  /// \brief Leave propositional_variable node
  /// Leave propositional variable node.
  void leave_propositional_variable()
  {
    m_monotonicity_values.push_back(pbes_monotonicity_positive);
  }
  
  /// \brief Returns the monotonicity result of visiting a term.
  bool result() const
  {
    return m_monotonicity_values.back() == pbes_monotonicity_positive || m_monotonicity_values.back() == pbes_monotonicity_both;
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_MONOTONICITY_VISITOR_H
