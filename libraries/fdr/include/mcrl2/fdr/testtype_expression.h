// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/testtype_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_TESTTYPE_EXPRESSION_H
#define MCRL2_FDR_TESTTYPE_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"

namespace mcrl2 {

namespace fdr {

//--- start generated testtype expression class declarations ---//
/// \brief class testtype_expression
class testtype_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    testtype_expression()
      : atermpp::aterm_appl(fdr::detail::constructTestType())
    {}

    /// \brief Constructor.
    /// \param term A term
    testtype_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_TestType(m_term));
    }
};

/// \brief list of testtype_expressions
typedef atermpp::term_list<testtype_expression> testtype_expression_list;

/// \brief vector of testtype_expressions
typedef atermpp::vector<testtype_expression>    testtype_expression_vector;

/// \brief A deterministic
class deterministic: public testtype_expression
{
  public:
    /// \brief Default constructor.
    deterministic();

    /// \brief Constructor.
    /// \param term A term
    deterministic(atermpp::aterm_appl term);
};

/// \brief A deadlock free
class deadlock_free: public testtype_expression
{
  public:
    /// \brief Default constructor.
    deadlock_free();

    /// \brief Constructor.
    /// \param term A term
    deadlock_free(atermpp::aterm_appl term);
};

/// \brief A livelock free
class livelock_free: public testtype_expression
{
  public:
    /// \brief Default constructor.
    livelock_free();

    /// \brief Constructor.
    /// \param term A term
    livelock_free(atermpp::aterm_appl term);
};
//--- end generated testtype expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a deterministic expression
    /// \param t A term
    /// \return True if it is a deterministic expression
    inline
    bool is_deterministic(const testtype_expression& t)
    {
      return fdr::detail::gsIsdeterministic(t);
    }

    /// \brief Test for a deadlock_free expression
    /// \param t A term
    /// \return True if it is a deadlock_free expression
    inline
    bool is_deadlock_free(const testtype_expression& t)
    {
      return fdr::detail::gsIsdeadlock_free(t);
    }

    /// \brief Test for a livelock_free expression
    /// \param t A term
    /// \return True if it is a livelock_free expression
    inline
    bool is_livelock_free(const testtype_expression& t)
    {
      return fdr::detail::gsIslivelock_free(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_TESTTYPE_EXPRESSION_H
