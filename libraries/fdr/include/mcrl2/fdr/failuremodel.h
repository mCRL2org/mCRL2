// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/failuremodel.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_FAILUREMODEL_H
#define MCRL2_FDR_FAILUREMODEL_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief A failures
class f
{
  public:
    /// \brief Default constructor.
    f()
      : atermpp::aterm_appl(fdr::atermpp::detail::constructF())
    {}

    /// \brief Constructor.
    /// \param term A term
    f(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_F(m_term));
    }
};/// \brief list of fs
    typedef atermpp::term_list<f> f_list;

    /// \brief vector of fs
    typedef atermpp::vector<f>    f_vector;


/// \brief A faulures/divergences
class fd
{
  public:
    /// \brief Default constructor.
    fd()
      : atermpp::aterm_appl(fdr::atermpp::detail::constructFD())
    {}

    /// \brief Constructor.
    /// \param term A term
    fd(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_FD(m_term));
    }
};/// \brief list of fds
    typedef atermpp::term_list<fd> fd_list;

    /// \brief vector of fds
    typedef atermpp::vector<fd>    fd_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a f expression
    /// \param t A term
    /// \return True if it is a f expression
    inline
    bool is_f(const failuremodel& t)
    {
      return fdr::detail::gsIsF(t);
    }

    /// \brief Test for a fd expression
    /// \param t A term
    /// \return True if it is a fd expression
    inline
    bool is_fd(const failuremodel& t)
    {
      return fdr::detail::gsIsFD(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_FAILUREMODEL_H
