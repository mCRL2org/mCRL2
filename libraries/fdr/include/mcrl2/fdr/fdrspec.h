// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/fdrspec.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_FDRSPEC_H
#define MCRL2_FDR_FDRSPEC_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief An FDR specification
class fdrspec
{
  public:
    /// \brief Default constructor.
    fdrspec()
      : atermpp::aterm_appl(fdr::atermpp::detail::constructFDRSpec())
    {}

    /// \brief Constructor.
    /// \param term A term
    fdrspec(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_FDRSpec(m_term));
    }

    /// \brief Constructor.
    fdrspec(const definition_list& defs)
      : atermpp::aterm_appl(fdr::detail::gsMakeFDRSpec(defs))
    {}

    definition_list defs() const
    {
      return atermpp::list_arg1(*this);
    }
};/// \brief list of fdrspecs
    typedef atermpp::term_list<fdrspec> fdrspec_list;

    /// \brief vector of fdrspecs
    typedef atermpp::vector<fdrspec>    fdrspec_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a fdrspec expression
    /// \param t A term
    /// \return True if it is a fdrspec expression
    inline
    bool is_fdrspec(const fdrspec& t)
    {
      return fdr::detail::gsIsFDRSpec(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_FDRSPEC_H
