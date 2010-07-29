// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/seq_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_SEQ_EXPRESSION_H
#define MCRL2_FDR_SEQ_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/common_expression.h"
                                      
namespace mcrl2 {

namespace fdr {

//--- start generated seq expression class declarations ---//
/// \brief class seq_expression
class seq_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
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

/// \brief list of seq_expressions
typedef atermpp::term_list<seq_expression> seq_expression_list;

/// \brief vector of seq_expressions
typedef atermpp::vector<seq_expression>    seq_expression_vector;

/// \brief The concatenation of two sequences
class cat: public seq_expression
{
  public:
    /// \brief Default constructor.
    cat();

    /// \brief Constructor.
    /// \param term A term
    cat(atermpp::aterm_appl term);

    /// \brief Constructor.
    cat(const seq_expression& left, const seq_expression& right);

    seq_expression left() const;

    seq_expression right() const;
};

/// \brief The concatenation of a sequence of sequences
class concat: public seq_expression
{
  public:
    /// \brief Default constructor.
    concat();

    /// \brief Constructor.
    /// \param term A term
    concat(atermpp::aterm_appl term);

    /// \brief Constructor.
    concat(const seq_expression& seq);

    seq_expression seq() const;
};

/// \brief The head of a non-empty sequence
class head: public seq_expression
{
  public:
    /// \brief Default constructor.
    head();

    /// \brief Constructor.
    /// \param term A term
    head(atermpp::aterm_appl term);

    /// \brief Constructor.
    head(const seq_expression& seq);

    seq_expression seq() const;
};

/// \brief The tail of a non-empty sequence
class tail: public seq_expression
{
  public:
    /// \brief Default constructor.
    tail();

    /// \brief Constructor.
    /// \param term A term
    tail(atermpp::aterm_appl term);

    /// \brief Constructor.
    tail(const seq_expression& seq);

    seq_expression seq() const;
};
//--- end generated seq expression class declarations ---//

//--- start generated is-functions ---//

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
