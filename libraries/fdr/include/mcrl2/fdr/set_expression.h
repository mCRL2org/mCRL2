// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/set_expression.h
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
/// \brief A common expression
class common: public set_expression
{
  public:
    /// \brief Default constructor.
    common()
      : set_expression(fdr::detail::constructCommon())
    {}

    /// \brief Constructor.
    /// \param term A term
    common(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Common(m_term));
    }

    /// \brief Constructor.
    common(const common_expression& operand)
      : set_expression(fdr::detail::gsMakeCommon(operand))
    {}

    common_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A simple argument
class targ: public set_expression
{
  public:
    /// \brief Default constructor.
    targ()
      : set_expression(fdr::detail::constructTarg())
    {}

    /// \brief Constructor.
    /// \param term A term
    targ(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Targ(m_term));
    }

    /// \brief Constructor.
    targ(const targ& targ)
      : set_expression(fdr::detail::gsMakeTarg(targ))
    {}

    targ targ() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A complex argument
class targgens: public set_expression
{
  public:
    /// \brief Default constructor.
    targgens()
      : set_expression(fdr::detail::constructTargGens())
    {}

    /// \brief Constructor.
    /// \param term A term
    targgens(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_TargGens(m_term));
    }

    /// \brief Constructor.
    targgens(const targ& targ, const generator_list& gens)
      : set_expression(fdr::detail::gsMakeTargGens(targ, gens))
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

/// \brief A simple argument (for channels)
class targ0: public set_expression
{
  public:
    /// \brief Default constructor.
    targ0()
      : set_expression(fdr::detail::constructTarg0())
    {}

    /// \brief Constructor.
    /// \param term A term
    targ0(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Targ0(m_term));
    }

    /// \brief Constructor.
    targ0(const targ& targ)
      : set_expression(fdr::detail::gsMakeTarg0(targ))
    {}

    targ targ() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A complex argument (for channels)
class targgens0: public set_expression
{
  public:
    /// \brief Default constructor.
    targgens0()
      : set_expression(fdr::detail::constructTargGens0())
    {}

    /// \brief Constructor.
    /// \param term A term
    targgens0(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_TargGens0(m_term));
    }

    /// \brief Constructor.
    targgens0(const targ& targ, const generator_list& gens)
      : set_expression(fdr::detail::gsMakeTargGens0(targ, gens))
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

    /// \brief Test for a common expression
    /// \param t A term
    /// \return True if it is a common expression
    inline
    bool is_common(const set_expression& t)
    {
      return fdr::detail::gsIsCommon(t);
    }

    /// \brief Test for a targ expression
    /// \param t A term
    /// \return True if it is a targ expression
    inline
    bool is_targ(const set_expression& t)
    {
      return fdr::detail::gsIsTarg(t);
    }

    /// \brief Test for a targgens expression
    /// \param t A term
    /// \return True if it is a targgens expression
    inline
    bool is_targgens(const set_expression& t)
    {
      return fdr::detail::gsIsTargGens(t);
    }

    /// \brief Test for a targ0 expression
    /// \param t A term
    /// \return True if it is a targ0 expression
    inline
    bool is_targ0(const set_expression& t)
    {
      return fdr::detail::gsIsTarg0(t);
    }

    /// \brief Test for a targgens0 expression
    /// \param t A term
    /// \return True if it is a targgens0 expression
    inline
    bool is_targgens0(const set_expression& t)
    {
      return fdr::detail::gsIsTargGens0(t);
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
