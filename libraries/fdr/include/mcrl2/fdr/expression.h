// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_EXPRESSION_H
#define MCRL2_FDR_EXPRESSION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Expression
  class expression: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      expression()
        : atermpp::aterm_appl(fdr::detail::constructExpression())
      {}

      /// \brief Constructor.
      /// \param term A term
      expression(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Expression(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A numeric expression
class numb: public expression
{
  public:
    /// \brief Default constructor.
    numb()
      : expression(fdr::detail::constructNumb())
    {}

    /// \brief Constructor.
    /// \param term A term
    numb(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Numb(m_term));
    }

    /// \brief Constructor.
    numb(const numeric_expression& operand)
      : expression(fdr::detail::gsMakeNumb(operand))
    {}

    numeric_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A boolean expression
class bool_: public expression
{
  public:
    /// \brief Default constructor.
    bool_()
      : expression(fdr::detail::constructBool())
    {}

    /// \brief Constructor.
    /// \param term A term
    bool_(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Bool(m_term));
    }

    /// \brief Constructor.
    bool_(const boolean_expression& operand)
      : expression(fdr::detail::gsMakeBool(operand))
    {}

    boolean_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A set expression
class set: public expression
{
  public:
    /// \brief Default constructor.
    set()
      : expression(fdr::detail::constructSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    set(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Set(m_term));
    }

    /// \brief Constructor.
    set(const set_expression& operand)
      : expression(fdr::detail::gsMakeSet(operand))
    {}

    set_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A seq expression
class seq: public expression
{
  public:
    /// \brief Default constructor.
    seq()
      : expression(fdr::detail::constructSeq())
    {}

    /// \brief Constructor.
    /// \param term A term
    seq(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Seq(m_term));
    }

    /// \brief Constructor.
    seq(const seq_expression& operand)
      : expression(fdr::detail::gsMakeSeq(operand))
    {}

    seq_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A tuple expression
class tuple: public expression
{
  public:
    /// \brief Default constructor.
    tuple()
      : expression(fdr::detail::constructTuple())
    {}

    /// \brief Constructor.
    /// \param term A term
    tuple(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Tuple(m_term));
    }

    /// \brief Constructor.
    tuple(const tuple_expression& operand)
      : expression(fdr::detail::gsMakeTuple(operand))
    {}

    tuple_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A dotted expression
class dotted: public expression
{
  public:
    /// \brief Default constructor.
    dotted()
      : expression(fdr::detail::constructDotted())
    {}

    /// \brief Constructor.
    /// \param term A term
    dotted(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Dotted(m_term));
    }

    /// \brief Constructor.
    dotted(const dotted_expression& operand)
      : expression(fdr::detail::gsMakeDotted(operand))
    {}

    dotted_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A lambda expression
class lambda: public expression
{
  public:
    /// \brief Default constructor.
    lambda()
      : expression(fdr::detail::constructLambda())
    {}

    /// \brief Constructor.
    /// \param term A term
    lambda(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Lambda(m_term));
    }

    /// \brief Constructor.
    lambda(const lambda_expression& operand)
      : expression(fdr::detail::gsMakeLambda(operand))
    {}

    lambda_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a numb expression
    /// \param t A term
    /// \return True if it is a numb expression
    inline
    bool is_numb(const expression& t)
    {
      return fdr::detail::gsIsNumb(t);
    }

    /// \brief Test for a bool_ expression
    /// \param t A term
    /// \return True if it is a bool_ expression
    inline
    bool is_bool_(const expression& t)
    {
      return fdr::detail::gsIsBool(t);
    }

    /// \brief Test for a set expression
    /// \param t A term
    /// \return True if it is a set expression
    inline
    bool is_set(const expression& t)
    {
      return fdr::detail::gsIsSet(t);
    }

    /// \brief Test for a seq expression
    /// \param t A term
    /// \return True if it is a seq expression
    inline
    bool is_seq(const expression& t)
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
