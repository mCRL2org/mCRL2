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

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/targ.h"
                                      
namespace mcrl2 {

namespace fdr {

//--- start generated set expression class declarations ---//
/// \brief class set_expression
class set_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
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

/// \brief list of set_expressions
typedef atermpp::term_list<set_expression> set_expression_list;

/// \brief vector of set_expressions
typedef atermpp::vector<set_expression>    set_expression_vector;

/// \brief A simple argument (for channels)
class chanset: public set_expression
{
  public:
    /// \brief Default constructor.
    chanset();

    /// \brief Constructor.
    /// \param term A term
    chanset(atermpp::aterm_appl term);

    /// \brief Constructor.
    chanset(const targ& argument);

    targ argument() const;
};

/// \brief A union
class union: public set_expression
{
  public:
    /// \brief Default constructor.
    union();

    /// \brief Constructor.
    /// \param term A term
    union(atermpp::aterm_appl term);

    /// \brief Constructor.
    union(const set_expression& left, const set_expression& right);

    set_expression left() const;

    set_expression right() const;
};

/// \brief An intersection
class inter: public set_expression
{
  public:
    /// \brief Default constructor.
    inter();

    /// \brief Constructor.
    /// \param term A term
    inter(atermpp::aterm_appl term);

    /// \brief Constructor.
    inter(const set_expression& left, const set_expression& right);

    set_expression left() const;

    set_expression right() const;
};

/// \brief A difference
class diff: public set_expression
{
  public:
    /// \brief Default constructor.
    diff();

    /// \brief Constructor.
    /// \param term A term
    diff(atermpp::aterm_appl term);

    /// \brief Constructor.
    diff(const set_expression& left, const set_expression& right);

    set_expression left() const;

    set_expression right() const;
};

/// \brief A set union
class union: public set_expression
{
  public:
    /// \brief Default constructor.
    union();

    /// \brief Constructor.
    /// \param term A term
    union(atermpp::aterm_appl term);

    /// \brief Constructor.
    union(const set_expression& set);

    set_expression set() const;
};

/// \brief A set intersection
class inter: public set_expression
{
  public:
    /// \brief Default constructor.
    inter();

    /// \brief Constructor.
    /// \param term A term
    inter(atermpp::aterm_appl term);

    /// \brief Constructor.
    inter(const set_expression& set);

    set_expression set() const;
};

/// \brief The set of a sequence
class set: public set_expression
{
  public:
    /// \brief Default constructor.
    set();

    /// \brief Constructor.
    /// \param term A term
    set(atermpp::aterm_appl term);

    /// \brief Constructor.
    set(const seq_expression& seq);

    seq_expression seq() const;
};

/// \brief The set of sets of a set
class set: public set_expression
{
  public:
    /// \brief Default constructor.
    set();

    /// \brief Constructor.
    /// \param term A term
    set(atermpp::aterm_appl term);

    /// \brief Constructor.
    set(const set_expression& set);

    set_expression set() const;
};

/// \brief The set of sequences of a sequence
class seq: public set_expression
{
  public:
    /// \brief Default constructor.
    seq();

    /// \brief Constructor.
    /// \param term A term
    seq(atermpp::aterm_appl term);

    /// \brief Constructor.
    seq(const seq_expression& seq);

    seq_expression seq() const;
};

/// \brief The extension set
class extensions: public set_expression
{
  public:
    /// \brief Default constructor.
    extensions();

    /// \brief Constructor.
    /// \param term A term
    extensions(atermpp::aterm_appl term);

    /// \brief Constructor.
    extensions(const expression& expr);

    expression expr() const;
};

/// \brief The production set
class productions: public set_expression
{
  public:
    /// \brief Default constructor.
    productions();

    /// \brief Constructor.
    /// \param term A term
    productions(atermpp::aterm_appl term);

    /// \brief Constructor.
    productions(const expression& expr);

    expression expr() const;
};
//--- end generated set expression class declarations ---//

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
