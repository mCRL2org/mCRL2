// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/boolean_expression_decl.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_BOOLEAN_EXPRESSION_DECL_H
#define MCRL2_FDR_BOOLEAN_EXPRESSION_DECL_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

#include "mcrl2/fdr/common_expression_decl.h"
#include "mcrl2/fdr/seq_expression_decl.h"
#include "mcrl2/fdr/set_expression_decl.h"
#include "mcrl2/fdr/expression_decl.h"

namespace mcrl2 {

namespace fdr {

//--- start generated class declarations ---//
/// \brief class boolean_expression
class boolean_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    boolean_expression()
      : atermpp::aterm_appl(fdr::detail::constructBooleanExpression())
    {}

    /// \brief Constructor.
    /// \param term A term
    boolean_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_BooleanExpression(m_term));
    }
};

/// \brief list of boolean_expressions
typedef atermpp::term_list<boolean_expression> boolean_expression_list;

/// \brief vector of boolean_expressions
typedef atermpp::vector<boolean_expression>    boolean_expression_vector;

/// \brief The true value
class true_: public boolean_expression
{
  public:
    /// \brief Default constructor.
    true_();

    /// \brief Constructor.
    /// \param term A term
    true_(atermpp::aterm_appl term);
};

/// \brief The false value
class false_: public boolean_expression
{
  public:
    /// \brief Default constructor.
    false_();

    /// \brief Constructor.
    /// \param term A term
    false_(atermpp::aterm_appl term);
};

/// \brief An and
class and_: public boolean_expression
{
  public:
    /// \brief Default constructor.
    and_();

    /// \brief Constructor.
    /// \param term A term
    and_(atermpp::aterm_appl term);

    /// \brief Constructor.
    and_(const boolean_expression& left, const boolean_expression& right);

    boolean_expression left() const;

    boolean_expression right() const;
};

/// \brief An or
class or_: public boolean_expression
{
  public:
    /// \brief Default constructor.
    or_();

    /// \brief Constructor.
    /// \param term A term
    or_(atermpp::aterm_appl term);

    /// \brief Constructor.
    or_(const boolean_expression& left, const boolean_expression& right);

    boolean_expression left() const;

    boolean_expression right() const;
};

/// \brief An not
class not_: public boolean_expression
{
  public:
    /// \brief Default constructor.
    not_();

    /// \brief Constructor.
    /// \param term A term
    not_(atermpp::aterm_appl term);

    /// \brief Constructor.
    not_(const boolean_expression& operand);

    boolean_expression operand() const;
};

/// \brief The empty sequence test
class null: public boolean_expression
{
  public:
    /// \brief Default constructor.
    null();

    /// \brief Constructor.
    /// \param term A term
    null(atermpp::aterm_appl term);

    /// \brief Constructor.
    null(const seq_expression& seq);

    seq_expression seq() const;
};

/// \brief The membership of a sequence test
class elem: public boolean_expression
{
  public:
    /// \brief Default constructor.
    elem();

    /// \brief Constructor.
    /// \param term A term
    elem(atermpp::aterm_appl term);

    /// \brief Constructor.
    elem(const expression& expr, const seq_expression& seq);

    expression expr() const;

    seq_expression seq() const;
};

/// \brief The membership of a set test
class member: public boolean_expression
{
  public:
    /// \brief Default constructor.
    member();

    /// \brief Constructor.
    /// \param term A term
    member(atermpp::aterm_appl term);

    /// \brief Constructor.
    member(const expression& expr, set_expression& set);

    expression expr() const;

    set_expression set() const;
};

/// \brief The empty set test
class empty: public boolean_expression
{
  public:
    /// \brief Default constructor.
    empty();

    /// \brief Constructor.
    /// \param term A term
    empty(atermpp::aterm_appl term);

    /// \brief Constructor.
    empty(const set_expression& set);

    set_expression set() const;
};

/// \brief An equality test
class equal: public boolean_expression
{
  public:
    /// \brief Default constructor.
    equal();

    /// \brief Constructor.
    /// \param term A term
    equal(atermpp::aterm_appl term);

    /// \brief Constructor.
    equal(const expression& left, const expression& right);

    expression left() const;

    expression right() const;
};

/// \brief An inequality test
class notequal: public boolean_expression
{
  public:
    /// \brief Default constructor.
    notequal();

    /// \brief Constructor.
    /// \param term A term
    notequal(atermpp::aterm_appl term);

    /// \brief Constructor.
    notequal(const expression& left, const expression& right);

    expression left() const;

    expression right() const;
};

/// \brief A less test
class less: public boolean_expression
{
  public:
    /// \brief Default constructor.
    less();

    /// \brief Constructor.
    /// \param term A term
    less(atermpp::aterm_appl term);

    /// \brief Constructor.
    less(const expression& left, const expression& right);

    expression left() const;

    expression right() const;
};

/// \brief A less or equal test
class lessorequal: public boolean_expression
{
  public:
    /// \brief Default constructor.
    lessorequal();

    /// \brief Constructor.
    /// \param term A term
    lessorequal(atermpp::aterm_appl term);

    /// \brief Constructor.
    lessorequal(const expression& left, const expression& right);

    expression left() const;

    expression right() const;
};

/// \brief A greater test
class greater: public boolean_expression
{
  public:
    /// \brief Default constructor.
    greater();

    /// \brief Constructor.
    /// \param term A term
    greater(atermpp::aterm_appl term);

    /// \brief Constructor.
    greater(const expression& left, const expression& right);

    expression left() const;

    expression right() const;
};

/// \brief A greater or equal test
class greaterorequal: public boolean_expression
{
  public:
    /// \brief Default constructor.
    greaterorequal();

    /// \brief Constructor.
    /// \param term A term
    greaterorequal(atermpp::aterm_appl term);

    /// \brief Constructor.
    greaterorequal(const expression& left, const expression& right);

    expression left() const;

    expression right() const;
};
//--- end generated class declarations ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_BOOLEAN_EXPRESSION_DECL_H
