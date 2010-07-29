// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/targ_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_TARG_EXPRESSION_H
#define MCRL2_FDR_TARG_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/comprehension_expression_fwd.h"
#include "mcrl2/fdr/expression_fwd.h"
#include "mcrl2/fdr/numeric_expression_fwd.h"

namespace mcrl2 {

namespace fdr {

//--- start generated targ expression class declarations ---//
/// \brief class targ_expression
class targ_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    targ_expression()
      : atermpp::aterm_appl(fdr::detail::constructFailureModel())
    {}

    /// \brief Constructor.
    /// \param term A term
    targ_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_FailureModel(m_term));
    }
};

/// \brief list of targ_expressions
typedef atermpp::term_list<targ_expression> targ_expression_list;

/// \brief vector of targ_expressions
typedef atermpp::vector<targ_expression>    targ_expression_vector;

/// \brief An empty
class nil: public targ_expression
{
  public:
    /// \brief Default constructor.
    nil();

    /// \brief Constructor.
    /// \param term A term
    nil(atermpp::aterm_appl term);
};

/// \brief A closed range
class closedrange: public targ_expression
{
  public:
    /// \brief Default constructor.
    closedrange();

    /// \brief Constructor.
    /// \param term A term
    closedrange(atermpp::aterm_appl term);

    /// \brief Constructor.
    closedrange(const numeric_expression& begin, const numeric_expression& end);

    numeric_expression begin() const;

    numeric_expression end() const;
};

/// \brief An open range
class openrange: public targ_expression
{
  public:
    /// \brief Default constructor.
    openrange();

    /// \brief Constructor.
    /// \param term A term
    openrange(atermpp::aterm_appl term);

    /// \brief Constructor.
    openrange(const numeric_expression& begin);

    numeric_expression begin() const;
};

/// \brief A comprehension
class compr: public targ_expression
{
  public:
    /// \brief Default constructor.
    compr();

    /// \brief Constructor.
    /// \param term A term
    compr(atermpp::aterm_appl term);

    /// \brief Constructor.
    compr(const expression& expr, const comprehension_expression_list& comprs);

    expression expr() const;

    comprehension_expression_list comprs() const;
};
//--- end generated targ expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a nil expression
    /// \param t A term
    /// \return True if it is a nil expression
    inline
    bool is_nil(const targ_expression& t)
    {
      return fdr::detail::gsIsNil(t);
    }

    /// \brief Test for a closedrange expression
    /// \param t A term
    /// \return True if it is a closedrange expression
    inline
    bool is_closedrange(const targ_expression& t)
    {
      return fdr::detail::gsIsClosedRange(t);
    }

    /// \brief Test for a openrange expression
    /// \param t A term
    /// \return True if it is a openrange expression
    inline
    bool is_openrange(const targ_expression& t)
    {
      return fdr::detail::gsIsOpenRange(t);
    }

    /// \brief Test for a compr expression
    /// \param t A term
    /// \return True if it is a compr expression
    inline
    bool is_compr(const targ_expression& t)
    {
      return fdr::detail::gsIsCompr(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_TARG_EXPRESSION_H
