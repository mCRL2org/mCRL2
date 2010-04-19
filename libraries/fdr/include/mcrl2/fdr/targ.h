// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/targ.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_TARG_H
#define MCRL2_FDR_TARG_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"
#include "mcrl2/fdr/numeric_expression.h"

namespace mcrl2 {

namespace fdr {

/// \brief class targ
class targ: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    targ()
      : atermpp::aterm_appl(fdr::detail::constructTarg())
    {}

    /// \brief Constructor.
    /// \param term A term
    targ(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Targ(m_term));
    }
};

/// \brief list of targs
typedef atermpp::term_list<targ> targ_list;

/// \brief vector of targs
typedef atermpp::vector<targ>    targ_vector;

//--- start generated classes ---//
/// \brief An empty
class nil: public targ
{
  public:
    /// \brief Default constructor.
    nil()
      : targ(fdr::detail::constructNil())
    {}

    /// \brief Constructor.
    /// \param term A term
    nil(atermpp::aterm_appl term)
      : targ(term)
    {
      assert(fdr::detail::check_term_Nil(m_term));
    }
};

/// \brief An expression list
class exprs: public targ
{
  public:
    /// \brief Default constructor.
    exprs()
      : targ(fdr::detail::constructExprs())
    {}

    /// \brief Constructor.
    /// \param term A term
    exprs(atermpp::aterm_appl term)
      : targ(term)
    {
      assert(fdr::detail::check_term_Exprs(m_term));
    }

    /// \brief Constructor.
    exprs(const expression_list& exprs)
      : targ(fdr::detail::gsMakeExprs(exprs))
    {}

    expression_list exprs() const
    {
      return atermpp::list_arg1(*this);
    }
};

/// \brief A closed range
class closedrange: public targ
{
  public:
    /// \brief Default constructor.
    closedrange()
      : targ(fdr::detail::constructClosedRange())
    {}

    /// \brief Constructor.
    /// \param term A term
    closedrange(atermpp::aterm_appl term)
      : targ(term)
    {
      assert(fdr::detail::check_term_ClosedRange(m_term));
    }

    /// \brief Constructor.
    closedrange(const numeric_expression& begin, const numeric_expression& end)
      : targ(fdr::detail::gsMakeClosedRange(begin, end))
    {}

    numeric_expression begin() const
    {
      return atermpp::arg1(*this);
    }

    numeric_expression end() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief An open range
class openrange: public targ
{
  public:
    /// \brief Default constructor.
    openrange()
      : targ(fdr::detail::constructOpenRange())
    {}

    /// \brief Constructor.
    /// \param term A term
    openrange(atermpp::aterm_appl term)
      : targ(term)
    {
      assert(fdr::detail::check_term_OpenRange(m_term));
    }

    /// \brief Constructor.
    openrange(const numeric_expression& begin)
      : targ(fdr::detail::gsMakeOpenRange(begin))
    {}

    numeric_expression begin() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A comprehension
class compr: public targ
{
  public:
    /// \brief Default constructor.
    compr()
      : targ(fdr::detail::constructCompr())
    {}

    /// \brief Constructor.
    /// \param term A term
    compr(atermpp::aterm_appl term)
      : targ(term)
    {
      assert(fdr::detail::check_term_Compr(m_term));
    }

    /// \brief Constructor.
    compr(const expression& expr, const comprehension_list& comprs)
      : targ(fdr::detail::gsMakeCompr(expr, comprs))
    {}

    expression expr() const
    {
      return atermpp::arg1(*this);
    }

    comprehension_list comprs() const
    {
      return atermpp::list_arg2(*this);
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_TARG_H
