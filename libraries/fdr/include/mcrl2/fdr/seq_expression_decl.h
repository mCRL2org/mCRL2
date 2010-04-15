// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/seq_expression_decl.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_SEQ_EXPRESSION_DECL_H
#define MCRL2_FDR_SEQ_EXPRESSION_DECL_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

#include "mcrl2/fdr/common_expression_decl.h"

namespace mcrl2 {

namespace fdr {

//--- start generated class declarations ---//
/// \brief class seq_expression
class seq_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    seq_expression()
      : atermpp::aterm_appl(fdr::detail::constructSeqExpression())
    {}

    /// \brief Constructor.
    /// \param term A term
    seq_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_SeqExpression(m_term));
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
//--- end generated class declarations ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_SEQ_EXPRESSION_DECL_H
