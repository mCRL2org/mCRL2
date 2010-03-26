// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/seq_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_SEQ_EXPRESSION_H
#define MCRL2_FDR_SEQ_EXPRESSION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"
#include "mcrl2/fdr/common_expression.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Set expression
  class seq_expression: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      seq_expression()
        : atermpp::aterm_appl(fdr::detail::constructSeq())
      {}

      /// \brief Constructor.
      /// \param term A term
      seq_expression(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Seq(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A common expression
class common: public seq_expression
{
  public:
    /// \brief Default constructor.
    common()
      : seq_expression(fdr::detail::constructCommon())
    {}

    /// \brief Constructor.
    /// \param term A term
    common(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Common(m_term));
    }

    /// \brief Constructor.
    common(const common_expression& operand)
      : seq_expression(fdr::detail::gsMakeCommon(operand))
    {}

    common_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A simple argument
class targ: public seq_expression
{
  public:
    /// \brief Default constructor.
    targ()
      : seq_expression(fdr::detail::constructTarg())
    {}

    /// \brief Constructor.
    /// \param term A term
    targ(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Targ(m_term));
    }

    /// \brief Constructor.
    targ(const targ& targ)
      : seq_expression(fdr::detail::gsMakeTarg(targ))
    {}

    targ targ() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A complex argument
class targgens: public seq_expression
{
  public:
    /// \brief Default constructor.
    targgens()
      : seq_expression(fdr::detail::constructTargGens())
    {}

    /// \brief Constructor.
    /// \param term A term
    targgens(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_TargGens(m_term));
    }

    /// \brief Constructor.
    targgens(const targ& targ, const generator_list& gens)
      : seq_expression(fdr::detail::gsMakeTargGens(targ, gens))
    {}

    targ targ() const
    {
      return atermpp::arg1(*this);
    }

    generator_list gens() const
    {
      return atermpp::list_arg2(*this);
    }
};

/// \brief The concatenation of two sequences
class cat: public seq_expression
{
  public:
    /// \brief Default constructor.
    cat()
      : seq_expression(fdr::detail::constructCat())
    {}

    /// \brief Constructor.
    /// \param term A term
    cat(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Cat(m_term));
    }

    /// \brief Constructor.
    cat(const seq_expression& left, const seq_expression& right)
      : seq_expression(fdr::detail::gsMakeCat(left, right))
    {}

    seq_expression left() const
    {
      return atermpp::arg1(*this);
    }

    seq_expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The concatenation of a sequence of sequences
class concat: public seq_expression
{
  public:
    /// \brief Default constructor.
    concat()
      : seq_expression(fdr::detail::constructConcat())
    {}

    /// \brief Constructor.
    /// \param term A term
    concat(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Concat(m_term));
    }

    /// \brief Constructor.
    concat(const seq_expression& seq)
      : seq_expression(fdr::detail::gsMakeConcat(seq))
    {}

    seq_expression seq() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief The head of a non-empty sequence
class head: public seq_expression
{
  public:
    /// \brief Default constructor.
    head()
      : seq_expression(fdr::detail::constructHead())
    {}

    /// \brief Constructor.
    /// \param term A term
    head(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Head(m_term));
    }

    /// \brief Constructor.
    head(const seq_expression& seq)
      : seq_expression(fdr::detail::gsMakeHead(seq))
    {}

    seq_expression seq() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief The tail of a non-empty sequence
class tail: public seq_expression
{
  public:
    /// \brief Default constructor.
    tail()
      : seq_expression(fdr::detail::constructTail())
    {}

    /// \brief Constructor.
    /// \param term A term
    tail(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Tail(m_term));
    }

    /// \brief Constructor.
    tail(const seq_expression& seq)
      : seq_expression(fdr::detail::gsMakeTail(seq))
    {}

    seq_expression seq() const
    {
      return atermpp::arg1(*this);
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a common expression
    /// \param t A term
    /// \return True if it is a common expression
    inline
    bool is_common(const seq_expression& t)
    {
      return fdr::detail::gsIsCommon(t);
    }

    /// \brief Test for a targ expression
    /// \param t A term
    /// \return True if it is a targ expression
    inline
    bool is_targ(const seq_expression& t)
    {
      return fdr::detail::gsIsTarg(t);
    }

    /// \brief Test for a targgens expression
    /// \param t A term
    /// \return True if it is a targgens expression
    inline
    bool is_targgens(const seq_expression& t)
    {
      return fdr::detail::gsIsTargGens(t);
    }

    /// \brief Test for a cat expression
    /// \param t A term
    /// \return True if it is a cat expression
    inline
    bool is_cat(const seq_expression& t)
    {
      return fdr::detail::gsIsCat(t);
    }

    /// \brief Test for a concat expression
    /// \param t A term
    /// \return True if it is a concat expression
    inline
    bool is_concat(const seq_expression& t)
    {
      return fdr::detail::gsIsConcat(t);
    }

    /// \brief Test for a head expression
    /// \param t A term
    /// \return True if it is a head expression
    inline
    bool is_head(const seq_expression& t)
    {
      return fdr::detail::gsIsHead(t);
    }

    /// \brief Test for a tail expression
    /// \param t A term
    /// \return True if it is a tail expression
    inline
    bool is_tail(const seq_expression& t)
    {
      return fdr::detail::gsIsTail(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_SEQ_EXPRESSION_H
