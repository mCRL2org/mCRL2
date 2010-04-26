// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/model_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_MODEL_EXPRESSION_H
#define MCRL2_FDR_MODEL_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"

namespace mcrl2 {

namespace fdr {

//--- start generated model expression class declarations ---//
/// \brief class model_expression
class model_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    model_expression()
      : atermpp::aterm_appl(fdr::detail::constructModel())
    {}

    /// \brief Constructor.
    /// \param term A term
    model_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_Model(m_term));
    }
};

/// \brief list of model_expressions
typedef atermpp::term_list<model_expression> model_expression_list;

/// \brief vector of model_expressions
typedef atermpp::vector<model_expression>    model_expression_vector;

/// \brief An empty
class nil: public model_expression
{
  public:
    /// \brief Default constructor.
    nil();

    /// \brief Constructor.
    /// \param term A term
    nil(atermpp::aterm_appl term);
};

/// \brief A traces
class t: public model_expression
{
  public:
    /// \brief Default constructor.
    t();

    /// \brief Constructor.
    /// \param term A term
    t(atermpp::aterm_appl term);
};
//--- end generated model expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a nil expression
    /// \param t A term
    /// \return True if it is a nil expression
    inline
    bool is_nil(const model_expression& t)
    {
      return fdr::detail::gsIsNil(t);
    }

    /// \brief Test for a t expression
    /// \param t A term
    /// \return True if it is a t expression
    inline
    bool is_t(const model_expression& t)
    {
      return fdr::detail::gsIsT(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_MODEL_EXPRESSION_H
