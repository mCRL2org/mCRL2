// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/check_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_CHECK_EXPRESSION_H
#define MCRL2_FDR_CHECK_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/refined.h"
#include "mcrl2/fdr/boolean_expression_fwd.h"
#include "mcrl2/fdr/process_expression_fwd.h"
#include "mcrl2/fdr/test_expression_fwd.h"

namespace mcrl2 {

namespace fdr {

//--- start generated check expression class declarations ---//
/// \brief class check_expression
class check_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    check_expression()
      : atermpp::aterm_appl(fdr::detail::constructCheck())
    {}

    /// \brief Constructor.
    /// \param term A term
    check_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_Check(m_term));
    }
};

/// \brief list of check_expressions
typedef atermpp::term_list<check_expression> check_expression_list;

/// \brief vector of check_expressions
typedef atermpp::vector<check_expression>    check_expression_vector;

/// \brief A boolean check
class bcheck: public check_expression
{
  public:
    /// \brief Default constructor.
    bcheck();

    /// \brief Constructor.
    /// \param term A term
    bcheck(atermpp::aterm_appl term);

    /// \brief Constructor.
    bcheck(const boolean_expression& expr);

    boolean_expression expr() const;
};

/// \brief A refinement check
class rcheck: public check_expression
{
  public:
    /// \brief Default constructor.
    rcheck();

    /// \brief Constructor.
    /// \param term A term
    rcheck(atermpp::aterm_appl term);

    /// \brief Constructor.
    rcheck(const process_expression& left, const process_expression& right, const refined& refinement);

    process_expression left() const;

    process_expression right() const;

    refined refinement() const;
};

/// \brief A test
class tcheck: public check_expression
{
  public:
    /// \brief Default constructor.
    tcheck();

    /// \brief Constructor.
    /// \param term A term
    tcheck(atermpp::aterm_appl term);

    /// \brief Constructor.
    tcheck(const process_expression& proc, const test_expression& operand);

    process_expression proc() const;

    test_expression operand() const;
};

/// \brief A negated check
class notcheck: public check_expression
{
  public:
    /// \brief Default constructor.
    notcheck();

    /// \brief Constructor.
    /// \param term A term
    notcheck(atermpp::aterm_appl term);

    /// \brief Constructor.
    notcheck(const check_expression& chk);

    check_expression chk() const;
};
//--- end generated check expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a bcheck expression
    /// \param t A term
    /// \return True if it is a bcheck expression
    inline
    bool is_bcheck(const check_expression& t)
    {
      return fdr::detail::gsIsBCheck(t);
    }

    /// \brief Test for a rcheck expression
    /// \param t A term
    /// \return True if it is a rcheck expression
    inline
    bool is_rcheck(const check_expression& t)
    {
      return fdr::detail::gsIsRCheck(t);
    }

    /// \brief Test for a tcheck expression
    /// \param t A term
    /// \return True if it is a tcheck expression
    inline
    bool is_tcheck(const check_expression& t)
    {
      return fdr::detail::gsIsTCheck(t);
    }

    /// \brief Test for a notcheck expression
    /// \param t A term
    /// \return True if it is a notcheck expression
    inline
    bool is_notcheck(const check_expression& t)
    {
      return fdr::detail::gsIsNotCheck(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_CHECK_EXPRESSION_H
