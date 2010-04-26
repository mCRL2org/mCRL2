// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/failuremodel_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_FAILUREMODEL_EXPRESSION_H
#define MCRL2_FDR_FAILUREMODEL_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"

namespace mcrl2 {

namespace fdr {

//--- start generated failuremodel expression class declarations ---//
/// \brief class failuremodel_expression
class failuremodel_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    failuremodel_expression()
      : atermpp::aterm_appl(fdr::detail::constructFailureModel())
    {}

    /// \brief Constructor.
    /// \param term A term
    failuremodel_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_FailureModel(m_term));
    }
};

/// \brief list of failuremodel_expressions
typedef atermpp::term_list<failuremodel_expression> failuremodel_expression_list;

/// \brief vector of failuremodel_expressions
typedef atermpp::vector<failuremodel_expression>    failuremodel_expression_vector;

/// \brief A failures
class f: public failuremodel_expression
{
  public:
    /// \brief Default constructor.
    f();

    /// \brief Constructor.
    /// \param term A term
    f(atermpp::aterm_appl term);
};

/// \brief A faulures/divergences
class fd: public failuremodel_expression
{
  public:
    /// \brief Default constructor.
    fd();

    /// \brief Constructor.
    /// \param term A term
    fd(atermpp::aterm_appl term);
};
//--- end generated failuremodel expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a f expression
    /// \param t A term
    /// \return True if it is a f expression
    inline
    bool is_f(const failuremodel_expression& t)
    {
      return fdr::detail::gsIsF(t);
    }

    /// \brief Test for a fd expression
    /// \param t A term
    /// \return True if it is a fd expression
    inline
    bool is_fd(const failuremodel_expression& t)
    {
      return fdr::detail::gsIsFD(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_FAILUREMODEL_EXPRESSION_H
