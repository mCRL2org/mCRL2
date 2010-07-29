// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/test_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_TEST_EXPRESSION_H
#define MCRL2_FDR_TEST_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/failuremodel_expression_fwd.h"
#include "mcrl2/fdr/testtype_expression_fwd.h"

namespace mcrl2 {

namespace fdr {

//--- start generated test expression class declarations ---//
/// \brief class test_expression
class test_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    test_expression()
      : atermpp::aterm_appl(fdr::detail::constructTest())
    {}

    /// \brief Constructor.
    /// \param term A term
    test_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_Test(m_term));
    }
};

/// \brief list of test_expressions
typedef atermpp::term_list<test_expression> test_expression_list;

/// \brief vector of test_expressions
typedef atermpp::vector<test_expression>    test_expression_vector;

/// \brief A divergence free
class divergence_free: public test_expression
{
  public:
    /// \brief Default constructor.
    divergence_free();

    /// \brief Constructor.
    /// \param term A term
    divergence_free(atermpp::aterm_appl term);
};

/// \brief A complex test
class test: public test_expression
{
  public:
    /// \brief Default constructor.
    test();

    /// \brief Constructor.
    /// \param term A term
    test(atermpp::aterm_appl term);

    /// \brief Constructor.
    test(const testtype_expression& tt, const failuremodel_expression& fm);

    testtype_expression tt() const;

    failuremodel_expression fm() const;
};
//--- end generated test expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a divergence_free expression
    /// \param t A term
    /// \return True if it is a divergence_free expression
    inline
    bool is_divergence_free(const test_expression& t)
    {
      return fdr::detail::gsIsdivergence_free(t);
    }

    /// \brief Test for a test expression
    /// \param t A term
    /// \return True if it is a test expression
    inline
    bool is_test(const test_expression& t)
    {
      return fdr::detail::gsIsTest(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_TEST_EXPRESSION_H
