// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/enum/standard.h

#ifndef _ENUM_STANDARD_H
#define _ENUM_STANDARD_H

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/detail/enumerator_variable_limit.h"

#define MAX_VARS_INIT   1000
#define MAX_VARS_FACTOR 5

/// \cond INTERNAL_DOCS
namespace mcrl2
{
namespace data
{
namespace detail
{

template <class TERM>
class fs_expr
{
  protected:
    variable_list m_vars;                              // The vars over which enumeration must take place.
    variable_list m_substituted_vars;                  // Variables for which a substitution exist. The substituted
                                                       // values are given in m_vals;
    data_expression_list m_vals;                       // Data expressions that are to be substituted
                                                       // in the variables in m_substituted_vars.
    TERM m_expr;                                       // data_expression to which internal variables
                                                       // must adhere.

  public:
    // Default constructor
    fs_expr()
    {}

    // Constructor
    fs_expr(
        const variable_list& vars, 
        const variable_list& substituted_vars, 
        const data_expression_list& vals, 
        const TERM& expr):
       m_vars(vars), m_substituted_vars(substituted_vars),m_vals(vals), m_expr(expr)
    {
    }

    const variable_list& vars() const
    {
      return m_vars;
    }

    const variable_list& substituted_vars() const
    {
      return m_substituted_vars;
    }

    const data_expression_list& vals() const
    {
      return m_vals;
    }

    const data_expression& expr() const
    {
      return m_expr;
    }
};

} // namespace detail
} // namespace data
} // namespace mcrl2
/// \endcond

#endif
