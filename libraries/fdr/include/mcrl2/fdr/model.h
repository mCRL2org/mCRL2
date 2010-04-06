// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/model.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_MODEL_H
#define MCRL2_FDR_MODEL_H

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


/// \brief A traces
class t
{
  public:
    /// \brief Default constructor.
    t()
      : atermpp::aterm_appl(fdr::detail::constructT())
    {}

    /// \brief Constructor.
    /// \param term A term
    t(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_T(m_term));
    }
};/// \brief list of ts
    typedef atermpp::term_list<t> t_list;

    /// \brief vector of ts
    typedef atermpp::vector<t>    t_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_MODEL_H
