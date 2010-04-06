// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/set_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_SET_EXPRESSION_H
#define MCRL2_FDR_SET_EXPRESSION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Set expression
  class set_expression: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      set_expression()
        : atermpp::aterm_appl(fdr::detail::constructSet())
      {}

      /// \brief Constructor.
      /// \param term A term
      set_expression(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Set(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A simple argument (for channels)
class chanset: public set_expression
{
  public:
    /// \brief Default constructor.
    chanset()
      : set_expression(fdr::detail::constructChanSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    chanset(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_ChanSet(m_term));
    }

    /// \brief Constructor.
    chanset(const targgens& argument)
      : set_expression(fdr::detail::gsMakeChanSet(argument))
    {}

    targgens argument() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A union
class union: public set_expression
{
  public:
    /// \brief Default constructor.
    union()
      : set_expression(fdr::detail::constructunion())
    {}

    /// \brief Constructor.
    /// \param term A term
    union(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_union(m_term));
    }

    /// \brief Constructor.
    union(const set_expression& left, const set_expression& right)
      : set_expression(fdr::detail::gsMakeunion(left, right))
    {}

    set_expression left() const
    {
      return atermpp::arg1(*this);
    }

    set_expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief An intersection
class inter: public set_expression
{
  public:
    /// \brief Default constructor.
    inter()
      : set_expression(fdr::detail::constructinter())
    {}

    /// \brief Constructor.
    /// \param term A term
    inter(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_inter(m_term));
    }

    /// \brief Constructor.
    inter(const set_expression& left, const set_expression& right)
      : set_expression(fdr::detail::gsMakeinter(left, right))
    {}

    set_expression left() const
    {
      return atermpp::arg1(*this);
    }

    set_expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A difference
class diff: public set_expression
{
  public:
    /// \brief Default constructor.
    diff()
      : set_expression(fdr::detail::constructdiff())
    {}

    /// \brief Constructor.
    /// \param term A term
    diff(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_diff(m_term));
    }

    /// \brief Constructor.
    diff(const set_expression& left, const set_expression& right)
      : set_expression(fdr::detail::gsMakediff(left, right))
    {}

    set_expression left() const
    {
      return atermpp::arg1(*this);
    }

    set_expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A set union
class union: public set_expression
{
  public:
    /// \brief Default constructor.
    union()
      : set_expression(fdr::detail::constructUnion())
    {}

    /// \brief Constructor.
    /// \param term A term
    union(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Union(m_term));
    }

    /// \brief Constructor.
    union(const set_expression& set)
      : set_expression(fdr::detail::gsMakeUnion(set))
    {}

    set_expression set() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A set intersection
class inter: public set_expression
{
  public:
    /// \brief Default constructor.
    inter()
      : set_expression(fdr::detail::constructInter())
    {}

    /// \brief Constructor.
    /// \param term A term
    inter(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Inter(m_term));
    }

    /// \brief Constructor.
    inter(const set_expression& set)
      : set_expression(fdr::detail::gsMakeInter(set))
    {}

    set_expression set() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief The set of a sequence
class set: public set_expression
{
  public:
    /// \brief Default constructor.
    set()
      : set_expression(fdr::detail::constructset())
    {}

    /// \brief Constructor.
    /// \param term A term
    set(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_set(m_term));
    }

    /// \brief Constructor.
    set(const seq_expression& seq)
      : set_expression(fdr::detail::gsMakeset(seq))
    {}

    seq_expression seq() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief The set of sets of a set
class set: public set_expression
{
  public:
    /// \brief Default constructor.
    set()
      : set_expression(fdr::detail::constructSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    set(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Set(m_term));
    }

    /// \brief Constructor.
    set(const set_expression& set)
      : set_expression(fdr::detail::gsMakeSet(set))
    {}

    set_expression set() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief The set of sequences of a sequence
class seq: public set_expression
{
  public:
    /// \brief Default constructor.
    seq()
      : set_expression(fdr::detail::constructSeq())
    {}

    /// \brief Constructor.
    /// \param term A term
    seq(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Seq(m_term));
    }

    /// \brief Constructor.
    seq(const seq_expression& seq)
      : set_expression(fdr::detail::gsMakeSeq(seq))
    {}

    seq_expression seq() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief The extension set
class extensions: public set_expression
{
  public:
    /// \brief Default constructor.
    extensions()
      : set_expression(fdr::detail::constructextensions())
    {}

    /// \brief Constructor.
    /// \param term A term
    extensions(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_extensions(m_term));
    }

    /// \brief Constructor.
    extensions(const expression& expr)
      : set_expression(fdr::detail::gsMakeextensions(expr))
    {}

    expression expr() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief The production set
class productions: public set_expression
{
  public:
    /// \brief Default constructor.
    productions()
      : set_expression(fdr::detail::constructproductions())
    {}

    /// \brief Constructor.
    /// \param term A term
    productions(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_productions(m_term));
    }

    /// \brief Constructor.
    productions(const expression& expr)
      : set_expression(fdr::detail::gsMakeproductions(expr))
    {}

    expression expr() const
    {
      return atermpp::arg1(*this);
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a chanset expression
    /// \param t A term
    /// \return True if it is a chanset expression
    inline
    bool is_chanset(const set_expression& t)
    {
      return fdr::detail::gsIsChanSet(t);
    }

    /// \brief Test for a union expression
    /// \param t A term
    /// \return True if it is a union expression
    inline
    bool is_union(const set_expression& t)
    {
      return fdr::detail::gsIsunion(t);
    }

    /// \brief Test for a inter expression
    /// \param t A term
    /// \return True if it is a inter expression
    inline
    bool is_inter(const set_expression& t)
    {
      return fdr::detail::gsIsinter(t);
    }

    /// \brief Test for a diff expression
    /// \param t A term
    /// \return True if it is a diff expression
    inline
    bool is_diff(const set_expression& t)
    {
      return fdr::detail::gsIsdiff(t);
    }

    /// \brief Test for a union expression
    /// \param t A term
    /// \return True if it is a union expression
    inline
    bool is_union(const set_expression& t)
    {
      return fdr::detail::gsIsUnion(t);
    }

    /// \brief Test for a inter expression
    /// \param t A term
    /// \return True if it is a inter expression
    inline
    bool is_inter(const set_expression& t)
    {
      return fdr::detail::gsIsInter(t);
    }

    /// \brief Test for a set expression
    /// \param t A term
    /// \return True if it is a set expression
    inline
    bool is_set(const set_expression& t)
    {
      return fdr::detail::gsIsset(t);
    }

    /// \brief Test for a set expression
    /// \param t A term
    /// \return True if it is a set expression
    inline
    bool is_set(const set_expression& t)
    {
      return fdr::detail::gsIsSet(t);
    }

    /// \brief Test for a seq expression
    /// \param t A term
    /// \return True if it is a seq expression
    inline
    bool is_seq(const set_expression& t)
    {
      return fdr::detail::gsIsSeq(t);
    }

    /// \brief Test for a extensions expression
    /// \param t A term
    /// \return True if it is a extensions expression
    inline
    bool is_extensions(const set_expression& t)
    {
      return fdr::detail::gsIsextensions(t);
    }

    /// \brief Test for a productions expression
    /// \param t A term
    /// \return True if it is a productions expression
    inline
    bool is_productions(const set_expression& t)
    {
      return fdr::detail::gsIsproductions(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_SET_EXPRESSION_H
