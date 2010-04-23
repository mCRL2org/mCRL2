// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_EXPRESSION_H
#define MCRL2_FDR_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"

namespace mcrl2 {

namespace fdr {

class boolean_expression ;
class dotted_expression  ;
class lambda_expression  ;
class numeric_expression ;
class seq_expression     ;
class set_expression     ;
class tuple_expression   ;

//--- start generated expression class declarations ---//
/// \brief class expression
class expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    expression()
      : atermpp::aterm_appl(fdr::detail::constructExpr())
    {}

    /// \brief Constructor.
    /// \param term A term
    expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_Expr(m_term));
    }
};

/// \brief list of expressions
typedef atermpp::term_list<expression> expression_list;

/// \brief vector of expressions
typedef atermpp::vector<expression>    expression_vector;

/// \brief A numeric expression
class numb: public expression
{
  public:
    /// \brief Default constructor.
    numb();

    /// \brief Constructor.
    /// \param term A term
    numb(atermpp::aterm_appl term);

    /// \brief Constructor.
    numb(const numeric_expression& operand);

    numeric_expression operand() const;
};

/// \brief A boolean expression
class bool_: public expression
{
  public:
    /// \brief Default constructor.
    bool_();

    /// \brief Constructor.
    /// \param term A term
    bool_(atermpp::aterm_appl term);

    /// \brief Constructor.
    bool_(const boolean_expression& operand);

    boolean_expression operand() const;
};

/// \brief A set expression
class Set: public expression
{
  public:
    /// \brief Default constructor.
    Set();

    /// \brief Constructor.
    /// \param term A term
    Set(atermpp::aterm_appl term);

    /// \brief Constructor.
    Set(const set_expression& operand);

    set_expression operand() const;
};

/// \brief A seq expression
class Seq: public expression
{
  public:
    /// \brief Default constructor.
    Seq();

    /// \brief Constructor.
    /// \param term A term
    Seq(atermpp::aterm_appl term);

    /// \brief Constructor.
    Seq(const seq_expression& operand);

    seq_expression operand() const;
};

/// \brief A tuple expression
class tuple: public expression
{
  public:
    /// \brief Default constructor.
    tuple();

    /// \brief Constructor.
    /// \param term A term
    tuple(atermpp::aterm_appl term);

    /// \brief Constructor.
    tuple(const tuple_expression& operand);

    tuple_expression operand() const;
};

/// \brief A dotted expression
class dotted: public expression
{
  public:
    /// \brief Default constructor.
    dotted();

    /// \brief Constructor.
    /// \param term A term
    dotted(atermpp::aterm_appl term);

    /// \brief Constructor.
    dotted(const dotted_expression& operand);

    dotted_expression operand() const;
};

/// \brief A lambda expression
class lambda: public expression
{
  public:
    /// \brief Default constructor.
    lambda();

    /// \brief Constructor.
    /// \param term A term
    lambda(atermpp::aterm_appl term);

    /// \brief Constructor.
    lambda(const lambda_expression& operand);

    lambda_expression operand() const;
};
//--- end generated expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a numb expression
    /// \param t A term
    /// \return True if it is a numb expression
    inline
    bool is_numb(const expression& t)
    {
      return fdr::detail::gsIsNumb(t);
    }

    /// \brief Test for a bool expression
    /// \param t A term
    /// \return True if it is a bool expression
    inline
    bool is_bool(const expression& t)
    {
      return fdr::detail::gsIsBool(t);
    }

    /// \brief Test for a Set expression
    /// \param t A term
    /// \return True if it is a Set expression
    inline
    bool is_Set(const expression& t)
    {
      return fdr::detail::gsIsSet(t);
    }

    /// \brief Test for a Seq expression
    /// \param t A term
    /// \return True if it is a Seq expression
    inline
    bool is_Seq(const expression& t)
    {
      return fdr::detail::gsIsSeq(t);
    }

    /// \brief Test for a tuple expression
    /// \param t A term
    /// \return True if it is a tuple expression
    inline
    bool is_tuple(const expression& t)
    {
      return fdr::detail::gsIsTuple(t);
    }

    /// \brief Test for a dotted expression
    /// \param t A term
    /// \return True if it is a dotted expression
    inline
    bool is_dotted(const expression& t)
    {
      return fdr::detail::gsIsDotted(t);
    }

    /// \brief Test for a lambda expression
    /// \param t A term
    /// \return True if it is a lambda expression
    inline
    bool is_lambda(const expression& t)
    {
      return fdr::detail::gsIsLambda(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_EXPRESSION_H
