// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/failuremodel.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_FAILUREMODEL_H
#define MCRL2_FDR_FAILUREMODEL_H

#include "mcrl2/fdr/term_include_files.h"

namespace mcrl2 {

namespace fdr {

/// \brief class failuremodel
class failuremodel: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    failuremodel()
      : atermpp::aterm_appl(fdr::detail::constructFailureModel())
    {}

    /// \brief Constructor.
    /// \param term A term
    failuremodel(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_FailureModel(m_term));
    }
};

/// \brief list of failuremodels
typedef atermpp::term_list<failuremodel> failuremodel_list;

/// \brief vector of failuremodels
typedef atermpp::vector<failuremodel>    failuremodel_vector;

//--- start generated classes ---//
/// \brief A failures
class f: public failuremodel
{
  public:
    /// \brief Default constructor.
    f()
      : failuremodel(fdr::detail::constructF())
    {}

    /// \brief Constructor.
    /// \param term A term
    f(atermpp::aterm_appl term)
      : failuremodel(term)
    {
      assert(fdr::detail::check_term_F(m_term));
    }
};

/// \brief A faulures/divergences
class fd: public failuremodel
{
  public:
    /// \brief Default constructor.
    fd()
      : failuremodel(fdr::detail::constructFD())
    {}

    /// \brief Constructor.
    /// \param term A term
    fd(atermpp::aterm_appl term)
      : failuremodel(term)
    {
      assert(fdr::detail::check_term_FD(m_term));
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_FAILUREMODEL_H
