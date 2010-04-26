// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/vartype_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_VARTYPE_EXPRESSION_H
#define MCRL2_FDR_VARTYPE_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/name.h"

namespace mcrl2 {

namespace fdr {

//--- start generated vartype expression class declarations ---//
/// \brief class vartype_expression
class vartype_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    vartype_expression()
      : atermpp::aterm_appl(fdr::detail::constructVarType())
    {}

    /// \brief Constructor.
    /// \param term A term
    vartype_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_VarType(m_term));
    }
};

/// \brief list of vartype_expressions
typedef atermpp::term_list<vartype_expression> vartype_expression_list;

/// \brief vector of vartype_expressions
typedef atermpp::vector<vartype_expression>    vartype_expression_vector;

/// \brief A simple branch
class simplebranch: public vartype_expression
{
  public:
    /// \brief Default constructor.
    simplebranch();

    /// \brief Constructor.
    /// \param term A term
    simplebranch(atermpp::aterm_appl term);

    /// \brief Constructor.
    simplebranch(const name& id);

    name id() const;
};

/// \brief A branch
class branch: public vartype_expression
{
  public:
    /// \brief Default constructor.
    branch();

    /// \brief Constructor.
    /// \param term A term
    branch(atermpp::aterm_appl term);

    /// \brief Constructor.
    branch(const name& id, const type_expression& type_name);

    name id() const;

    type_expression type_name() const;
};
//--- end generated vartype expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a simplebranch expression
    /// \param t A term
    /// \return True if it is a simplebranch expression
    inline
    bool is_simplebranch(const vartype_expression& t)
    {
      return fdr::detail::gsIsSimpleBranch(t);
    }

    /// \brief Test for a branch expression
    /// \param t A term
    /// \return True if it is a branch expression
    inline
    bool is_branch(const vartype_expression& t)
    {
      return fdr::detail::gsIsBranch(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_VARTYPE_EXPRESSION_H
