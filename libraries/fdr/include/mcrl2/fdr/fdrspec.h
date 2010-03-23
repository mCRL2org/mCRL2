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

namespace mcrl2 {

namespace fdr {

  /// \brief FDR Specification
  class fdrspec: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      fdrspec()
        : atermpp::aterm_appl(fdr::detail::constructFDRSpec())
      {}

      /// \brief Constructor.
      /// \param term A term
      fdrspec(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_FDRSpec(m_term));
      }
  };

//--- start generated classes ---//
/// \brief An FDR specification
class fdrspec: public fdrspec
{
  public:
    /// \brief Default constructor.
    fdrspec()
      : fdrspec(fdr::detail::constructFDRSpec())
    {}

    /// \brief Constructor.
    /// \param term A term
    fdrspec(atermpp::aterm_appl term)
      : fdrspec(term)
    {
      assert(fdr::detail::check_term_FDRSpec(m_term));
    }

    /// \brief Constructor.
    fdrspec(const definition_list& defs)
      : fdrspec(fdr::detail::gsMakeFDRSpec(defs))
    {}

    definition_list defs() const
    {
      return atermpp::list_arg1(*this);
    }
};
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
