// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/summand.h
/// \brief The class summand.

#ifndef MCRL2_LPS_SUMMAND_H
#define MCRL2_LPS_SUMMAND_H

#include <string>
#include <algorithm>
#include <iterator>
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/bool.h"

namespace mcrl2
{

namespace lps
{

/// \brief Base class for LPS summands.
class summand_base
{
  protected:
    /// \brief The summation variables of the summand
    data::variable_list m_summation_variables;

    /// \brief The condition of the summand
    data::data_expression m_condition;

  public:
    /// \brief Constructor.
    summand_base()
    {}

    /// \brief Constructor.
    summand_base(const data::variable_list& summation_variables, const data::data_expression& condition)
      : m_summation_variables(summation_variables),
        m_condition(condition)
    {}

    /// \brief Returns the sequence of summation variables.
    /// \return The sequence of summation variables.
    data::variable_list& summation_variables()
    {
      return m_summation_variables;
    }

    /// \brief Returns the sequence of summation variables.
    /// \return The sequence of summation variables.
    const data::variable_list& summation_variables() const
    {
      return m_summation_variables;
    }

    /// \brief Returns the condition expression.
    /// \return The condition expression.
    const data::data_expression& condition() const
    {
      return m_condition;
    }

    /// \brief Returns the condition expression.
    /// \return The condition expression.
    data::data_expression& condition()
    {
      return m_condition;
    }

    /// \brief Swaps the contents
    void swap(summand_base& other)
    {
      using std::swap;
      swap(m_summation_variables, other.m_summation_variables);
      swap(m_condition, other.m_condition);
    }
};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_SUMMAND_H
