// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/boolean_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_BOOLEAN_EXPRESSION_H
#define MCRL2_FDR_BOOLEAN_EXPRESSION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/common_expression.h"
#include "mcrl2/fdr/seq_expression.h"
#include "mcrl2/fdr/set_expression.h"
#include "mcrl2/fdr/expression.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Boolean expression
  class boolean_expression: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      boolean_expression()
        : atermpp::aterm_appl(fdr::detail::constructBool())
      {}

      /// \brief Constructor.
      /// \param term A term
      boolean_expression(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Bool(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A common expression
class common: public boolean_expression
{
  public:
    /// \brief Default constructor.
    common()
      : boolean_expression(fdr::detail::constructCommon())
    {}

    /// \brief Constructor.
    /// \param term A term
    common(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Common(m_term));
    }

    /// \brief Constructor.
    common(const common_expression& operand)
      : boolean_expression(fdr::detail::gsMakeCommon(operand))
    {}

    common_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief The true value
class true_: public boolean_expression
{
  public:
    /// \brief Default constructor.
    true_()
      : boolean_expression(fdr::detail::constructtrue())
    {}

    /// \brief Constructor.
    /// \param term A term
    true_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_true(m_term));
    }
};

/// \brief The false value
class false_: public boolean_expression
{
  public:
    /// \brief Default constructor.
    false_()
      : boolean_expression(fdr::detail::constructfalse())
    {}

    /// \brief Constructor.
    /// \param term A term
    false_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_false(m_term));
    }
};

/// \brief An and
class and_: public boolean_expression
{
  public:
    /// \brief Default constructor.
    and_()
      : boolean_expression(fdr::detail::constructAnd())
    {}

    /// \brief Constructor.
    /// \param term A term
    and_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_And(m_term));
    }

    /// \brief Constructor.
    and_(const boolean_expression& left, const boolean_expression& right)
      : boolean_expression(fdr::detail::gsMakeAnd(left, right))
    {}

    boolean_expression left() const
    {
      return atermpp::arg1(*this);
    }

    boolean_expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief An or
class or_: public boolean_expression
{
  public:
    /// \brief Default constructor.
    or_()
      : boolean_expression(fdr::detail::constructOr())
    {}

    /// \brief Constructor.
    /// \param term A term
    or_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Or(m_term));
    }

    /// \brief Constructor.
    or_(const boolean_expression& left, const boolean_expression& right)
      : boolean_expression(fdr::detail::gsMakeOr(left, right))
    {}

    boolean_expression left() const
    {
      return atermpp::arg1(*this);
    }

    boolean_expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief An not
class not: public boolean_expression
{
  public:
    /// \brief Default constructor.
    not()
      : boolean_expression(fdr::detail::constructNot())
    {}

    /// \brief Constructor.
    /// \param term A term
    not(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Not(m_term));
    }

    /// \brief Constructor.
    not(const boolean_expression& operand)
      : boolean_expression(fdr::detail::gsMakeNot(operand))
    {}

    boolean_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief The empty sequence test
class null: public boolean_expression
{
  public:
    /// \brief Default constructor.
    null()
      : boolean_expression(fdr::detail::constructNull())
    {}

    /// \brief Constructor.
    /// \param term A term
    null(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Null(m_term));
    }

    /// \brief Constructor.
    null(const seq_expression& seq)
      : boolean_expression(fdr::detail::gsMakeNull(seq))
    {}

    seq_expression seq() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief The membership of a sequence test
class elem: public boolean_expression
{
  public:
    /// \brief Default constructor.
    elem()
      : boolean_expression(fdr::detail::constructElem())
    {}

    /// \brief Constructor.
    /// \param term A term
    elem(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Elem(m_term));
    }

    /// \brief Constructor.
    elem(const expression& expr, const seq_expression& seq)
      : boolean_expression(fdr::detail::gsMakeElem(expr, seq))
    {}

    expression expr() const
    {
      return atermpp::arg1(*this);
    }

    seq_expression seq() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The membership of a set test
class member: public boolean_expression
{
  public:
    /// \brief Default constructor.
    member()
      : boolean_expression(fdr::detail::constructMember())
    {}

    /// \brief Constructor.
    /// \param term A term
    member(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Member(m_term));
    }

    /// \brief Constructor.
    member(const expression& expr, set_expression& set)
      : boolean_expression(fdr::detail::gsMakeMember(expr, set))
    {}

    expression expr() const
    {
      return atermpp::arg1(*this);
    }

    set_expression set() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The empty set test
class empty: public boolean_expression
{
  public:
    /// \brief Default constructor.
    empty()
      : boolean_expression(fdr::detail::constructEmpty())
    {}

    /// \brief Constructor.
    /// \param term A term
    empty(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Empty(m_term));
    }

    /// \brief Constructor.
    empty(const set_expression& set)
      : boolean_expression(fdr::detail::gsMakeEmpty(set))
    {}

    set_expression set() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief An equality test
class equal: public boolean_expression
{
  public:
    /// \brief Default constructor.
    equal()
      : boolean_expression(fdr::detail::constructEqual())
    {}

    /// \brief Constructor.
    /// \param term A term
    equal(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Equal(m_term));
    }

    /// \brief Constructor.
    equal(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeEqual(left, right))
    {}

    expression left() const
    {
      return atermpp::arg1(*this);
    }

    expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief An inequality test
class notequal: public boolean_expression
{
  public:
    /// \brief Default constructor.
    notequal()
      : boolean_expression(fdr::detail::constructNotEqual())
    {}

    /// \brief Constructor.
    /// \param term A term
    notequal(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_NotEqual(m_term));
    }

    /// \brief Constructor.
    notequal(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeNotEqual(left, right))
    {}

    expression left() const
    {
      return atermpp::arg1(*this);
    }

    expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A less test
class less: public boolean_expression
{
  public:
    /// \brief Default constructor.
    less()
      : boolean_expression(fdr::detail::constructLess())
    {}

    /// \brief Constructor.
    /// \param term A term
    less(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Less(m_term));
    }

    /// \brief Constructor.
    less(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeLess(left, right))
    {}

    expression left() const
    {
      return atermpp::arg1(*this);
    }

    expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A less or equal test
class lessorequal: public boolean_expression
{
  public:
    /// \brief Default constructor.
    lessorequal()
      : boolean_expression(fdr::detail::constructLessOrEqual())
    {}

    /// \brief Constructor.
    /// \param term A term
    lessorequal(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_LessOrEqual(m_term));
    }

    /// \brief Constructor.
    lessorequal(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeLessOrEqual(left, right))
    {}

    expression left() const
    {
      return atermpp::arg1(*this);
    }

    expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A greater test
class greater: public boolean_expression
{
  public:
    /// \brief Default constructor.
    greater()
      : boolean_expression(fdr::detail::constructGreater())
    {}

    /// \brief Constructor.
    /// \param term A term
    greater(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Greater(m_term));
    }

    /// \brief Constructor.
    greater(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeGreater(left, right))
    {}

    expression left() const
    {
      return atermpp::arg1(*this);
    }

    expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A greater or equal test
class greaterorequal: public boolean_expression
{
  public:
    /// \brief Default constructor.
    greaterorequal()
      : boolean_expression(fdr::detail::constructGreaterOrEqual())
    {}

    /// \brief Constructor.
    /// \param term A term
    greaterorequal(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_GreaterOrEqual(m_term));
    }

    /// \brief Constructor.
    greaterorequal(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeGreaterOrEqual(left, right))
    {}

    expression left() const
    {
      return atermpp::arg1(*this);
    }

    expression right() const
    {
      return atermpp::arg2(*this);
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a common expression
    /// \param t A term
    /// \return True if it is a common expression
    inline
    bool is_common(const boolean_expression& t)
    {
      return fdr::detail::gsIsCommon(t);
    }

    /// \brief Test for a true_ expression
    /// \param t A term
    /// \return True if it is a true_ expression
    inline
    bool is_true_(const boolean_expression& t)
    {
      return fdr::detail::gsIstrue(t);
    }

    /// \brief Test for a false_ expression
    /// \param t A term
    /// \return True if it is a false_ expression
    inline
    bool is_false_(const boolean_expression& t)
    {
      return fdr::detail::gsIsfalse(t);
    }

    /// \brief Test for a and_ expression
    /// \param t A term
    /// \return True if it is a and_ expression
    inline
    bool is_and_(const boolean_expression& t)
    {
      return fdr::detail::gsIsAnd(t);
    }

    /// \brief Test for a or_ expression
    /// \param t A term
    /// \return True if it is a or_ expression
    inline
    bool is_or_(const boolean_expression& t)
    {
      return fdr::detail::gsIsOr(t);
    }

    /// \brief Test for a not expression
    /// \param t A term
    /// \return True if it is a not expression
    inline
    bool is_not(const boolean_expression& t)
    {
      return fdr::detail::gsIsNot(t);
    }

    /// \brief Test for a null expression
    /// \param t A term
    /// \return True if it is a null expression
    inline
    bool is_null(const boolean_expression& t)
    {
      return fdr::detail::gsIsNull(t);
    }

    /// \brief Test for a elem expression
    /// \param t A term
    /// \return True if it is a elem expression
    inline
    bool is_elem(const boolean_expression& t)
    {
      return fdr::detail::gsIsElem(t);
    }

    /// \brief Test for a member expression
    /// \param t A term
    /// \return True if it is a member expression
    inline
    bool is_member(const boolean_expression& t)
    {
      return fdr::detail::gsIsMember(t);
    }

    /// \brief Test for a empty expression
    /// \param t A term
    /// \return True if it is a empty expression
    inline
    bool is_empty(const boolean_expression& t)
    {
      return fdr::detail::gsIsEmpty(t);
    }

    /// \brief Test for a equal expression
    /// \param t A term
    /// \return True if it is a equal expression
    inline
    bool is_equal(const boolean_expression& t)
    {
      return fdr::detail::gsIsEqual(t);
    }

    /// \brief Test for a notequal expression
    /// \param t A term
    /// \return True if it is a notequal expression
    inline
    bool is_notequal(const boolean_expression& t)
    {
      return fdr::detail::gsIsNotEqual(t);
    }

    /// \brief Test for a less expression
    /// \param t A term
    /// \return True if it is a less expression
    inline
    bool is_less(const boolean_expression& t)
    {
      return fdr::detail::gsIsLess(t);
    }

    /// \brief Test for a lessorequal expression
    /// \param t A term
    /// \return True if it is a lessorequal expression
    inline
    bool is_lessorequal(const boolean_expression& t)
    {
      return fdr::detail::gsIsLessOrEqual(t);
    }

    /// \brief Test for a greater expression
    /// \param t A term
    /// \return True if it is a greater expression
    inline
    bool is_greater(const boolean_expression& t)
    {
      return fdr::detail::gsIsGreater(t);
    }

    /// \brief Test for a greaterorequal expression
    /// \param t A term
    /// \return True if it is a greaterorequal expression
    inline
    bool is_greaterorequal(const boolean_expression& t)
    {
      return fdr::detail::gsIsGreaterOrEqual(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_BOOLEAN_EXPRESSION_H
