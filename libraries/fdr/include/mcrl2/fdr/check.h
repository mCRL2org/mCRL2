// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/check.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_CHECK_H
#define MCRL2_FDR_CHECK_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief A boolean check
class bcheck
{
  public:
    /// \brief Default constructor.
    bcheck()
      : atermpp::aterm_appl(fdr::atermpp::detail::constructBCheck())
    {}

    /// \brief Constructor.
    /// \param term A term
    bcheck(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_BCheck(m_term));
    }

    /// \brief Constructor.
    bcheck(const boolean_expression& expr)
      : atermpp::aterm_appl(fdr::detail::gsMakeBCheck(expr))
    {}

    boolean_expression expr() const
    {
      return atermpp::arg1(*this);
    }
};/// \brief list of bchecks
    typedef atermpp::term_list<bcheck> bcheck_list;

    /// \brief vector of bchecks
    typedef atermpp::vector<bcheck>    bcheck_vector;


/// \brief A refinement check
class rcheck
{
  public:
    /// \brief Default constructor.
    rcheck()
      : atermpp::aterm_appl(fdr::atermpp::detail::constructRCheck())
    {}

    /// \brief Constructor.
    /// \param term A term
    rcheck(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_RCheck(m_term));
    }

    /// \brief Constructor.
    rcheck(const process& left, const process& right, const refined& refined)
      : atermpp::aterm_appl(fdr::detail::gsMakeRCheck(left, right, refined))
    {}

    process left() const
    {
      return atermpp::arg1(*this);
    }

    process right() const
    {
      return atermpp::arg2(*this);
    }

    refined refined() const
    {
      return atermpp::arg3(*this);
    }
};/// \brief list of rchecks
    typedef atermpp::term_list<rcheck> rcheck_list;

    /// \brief vector of rchecks
    typedef atermpp::vector<rcheck>    rcheck_vector;


/// \brief A test
class tcheck
{
  public:
    /// \brief Default constructor.
    tcheck()
      : atermpp::aterm_appl(fdr::atermpp::detail::constructTCheck())
    {}

    /// \brief Constructor.
    /// \param term A term
    tcheck(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_TCheck(m_term));
    }

    /// \brief Constructor.
    tcheck(const process& process, const test& test)
      : atermpp::aterm_appl(fdr::detail::gsMakeTCheck(process, test))
    {}

    process process() const
    {
      return atermpp::arg1(*this);
    }

    test test() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of tchecks
    typedef atermpp::term_list<tcheck> tcheck_list;

    /// \brief vector of tchecks
    typedef atermpp::vector<tcheck>    tcheck_vector;


/// \brief A negated check
class notcheck
{
  public:
    /// \brief Default constructor.
    notcheck()
      : atermpp::aterm_appl(fdr::atermpp::detail::constructNotCheck())
    {}

    /// \brief Constructor.
    /// \param term A term
    notcheck(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_NotCheck(m_term));
    }

    /// \brief Constructor.
    notcheck(const check& check)
      : atermpp::aterm_appl(fdr::detail::gsMakeNotCheck(check))
    {}

    check check() const
    {
      return atermpp::arg1(*this);
    }
};/// \brief list of notchecks
    typedef atermpp::term_list<notcheck> notcheck_list;

    /// \brief vector of notchecks
    typedef atermpp::vector<notcheck>    notcheck_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a bcheck expression
    /// \param t A term
    /// \return True if it is a bcheck expression
    inline
    bool is_bcheck(const check& t)
    {
      return fdr::detail::gsIsBCheck(t);
    }

    /// \brief Test for a rcheck expression
    /// \param t A term
    /// \return True if it is a rcheck expression
    inline
    bool is_rcheck(const check& t)
    {
      return fdr::detail::gsIsRCheck(t);
    }

    /// \brief Test for a tcheck expression
    /// \param t A term
    /// \return True if it is a tcheck expression
    inline
    bool is_tcheck(const check& t)
    {
      return fdr::detail::gsIsTCheck(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_CHECK_H
