// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_expression_with_variables.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DATA_EXPRESSION_WITH_VARIABLES_H
#define MCRL2_DATA_DATA_EXPRESSION_WITH_VARIABLES_H

#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/core/term_traits.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/variable.h"

namespace mcrl2
{

namespace data
{

/// \brief Data expression with a sequence of variables attached to it
/// The intended use case is to store the free variables of the expression.
class data_expression_with_variables: public data_expression
{
  protected:

    /// \brief The attached variables.
    variable_list m_variables;

  public:

    /// \brief Constructor.
    data_expression_with_variables()
    {}

    /// \brief Constructor. Creates a data expression with an empty sequence of variables.
    /// \param term A term
    data_expression_with_variables(const atermpp::aterm_appl& term)
      : data_expression(term)
    {}

    /// \brief Constructor. Creates a data expression with an empty sequence of variables.
    /// \param term A term
    data_expression_with_variables(const variable& v)
      : data_expression(v), m_variables(atermpp::make_list(v))
    {}

//      /// \brief Constructor. Creates a data expression with an empty sequence of variables.
//      /// \param term A term
//      data_expression_with_variables(aterm_appl term)
//        : data_expression(term)
//      {}

    /// \brief Constructor.
    /// \param expression A data expression
    /// \param variables A sequence of data variables
    template < typename Container >
    data_expression_with_variables(data_expression expression, const Container& variables)
      : data_expression(expression), m_variables(atermpp::convert< variable_list >(variables))
    {}

    /// \brief Return the variables.
    /// \return The variables of the data expression.
    variable_list variables() const
    {
      return m_variables;
    }

    /// \brief Return the variables.
    /// \return The variables of the data expression.
    variable_list& variables()
    {
      return m_variables;
    }

    /// \brief Returns true if the sequence of variables is empty.
    /// \return True if the sequence of variables is empty.
    bool is_constant() const
    {
      return m_variables.empty();
    }
};

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DATA_EXPRESSION_WITH_VARIABLES_H
