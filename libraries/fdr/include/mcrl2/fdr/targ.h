// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/targ.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_TARG_H
#define MCRL2_FDR_TARG_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief An empty
class nil
{
  public:
    /// \brief Default constructor.
    nil()
      : atermpp::aterm_appl(fdr::detail::constructNil())
    {}

    /// \brief Constructor.
    /// \param term A term
    nil(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Nil(m_term));
    }
};/// \brief list of nils
    typedef atermpp::term_list<nil> nil_list;

    /// \brief vector of nils
    typedef atermpp::vector<nil>    nil_vector;


/// \brief An expression list
class exprs
{
  public:
    /// \brief Default constructor.
    exprs()
      : atermpp::aterm_appl(fdr::detail::constructExprs())
    {}

    /// \brief Constructor.
    /// \param term A term
    exprs(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Exprs(m_term));
    }

    /// \brief Constructor.
    exprs(const expression_list& exprs)
      : atermpp::aterm_appl(fdr::detail::gsMakeExprs(exprs))
    {}

    expression_list exprs() const
    {
      return atermpp::list_arg1(*this);
    }
};/// \brief list of exprss
    typedef atermpp::term_list<exprs> exprs_list;

    /// \brief vector of exprss
    typedef atermpp::vector<exprs>    exprs_vector;


/// \brief A closed range
class closedrange
{
  public:
    /// \brief Default constructor.
    closedrange()
      : atermpp::aterm_appl(fdr::detail::constructClosedRange())
    {}

    /// \brief Constructor.
    /// \param term A term
    closedrange(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_ClosedRange(m_term));
    }

    /// \brief Constructor.
    closedrange(const numeric_expression& begin, const numeric_expression& end)
      : atermpp::aterm_appl(fdr::detail::gsMakeClosedRange(begin, end))
    {}

    numeric_expression begin() const
    {
      return atermpp::arg1(*this);
    }

    numeric_expression end() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of closedranges
    typedef atermpp::term_list<closedrange> closedrange_list;

    /// \brief vector of closedranges
    typedef atermpp::vector<closedrange>    closedrange_vector;


/// \brief An open range
class openrange
{
  public:
    /// \brief Default constructor.
    openrange()
      : atermpp::aterm_appl(fdr::detail::constructOpenRange())
    {}

    /// \brief Constructor.
    /// \param term A term
    openrange(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_OpenRange(m_term));
    }

    /// \brief Constructor.
    openrange(const numeric_expression& begin)
      : atermpp::aterm_appl(fdr::detail::gsMakeOpenRange(begin))
    {}

    numeric_expression begin() const
    {
      return atermpp::arg1(*this);
    }
};/// \brief list of openranges
    typedef atermpp::term_list<openrange> openrange_list;

    /// \brief vector of openranges
    typedef atermpp::vector<openrange>    openrange_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a nil expression
    /// \param t A term
    /// \return True if it is a nil expression
    inline
    bool is_nil(const targ& t)
    {
      return fdr::detail::gsIsNil(t);
    }

    /// \brief Test for a exprs expression
    /// \param t A term
    /// \return True if it is a exprs expression
    inline
    bool is_exprs(const targ& t)
    {
      return fdr::detail::gsIsExprs(t);
    }

    /// \brief Test for a closedrange expression
    /// \param t A term
    /// \return True if it is a closedrange expression
    inline
    bool is_closedrange(const targ& t)
    {
      return fdr::detail::gsIsClosedRange(t);
    }

    /// \brief Test for a openrange expression
    /// \param t A term
    /// \return True if it is a openrange expression
    inline
    bool is_openrange(const targ& t)
    {
      return fdr::detail::gsIsOpenRange(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_TARG_H
