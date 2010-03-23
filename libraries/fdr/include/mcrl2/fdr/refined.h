// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/refined.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_REFINED_H
#define MCRL2_FDR_REFINED_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Refined
  class refined: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      refined()
        : atermpp::aterm_appl(fdr::detail::constructRefined())
      {}

      /// \brief Constructor.
      /// \param term A term
      refined(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Refined(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A model
class model: public refined
{
  public:
    /// \brief Default constructor.
    model()
      : refined(fdr::detail::constructModel())
    {}

    /// \brief Constructor.
    /// \param term A term
    model(atermpp::aterm_appl term)
      : refined(term)
    {
      assert(fdr::detail::check_term_Model(m_term));
    }

    /// \brief Constructor.
    model(const model& model)
      : refined(fdr::detail::gsMakeModel(model))
    {}

    model model() const
    {
      return atermpp::arg1(*this);
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a model expression
    /// \param t A term
    /// \return True if it is a model expression
    inline
    bool is_model(const refine& t)
    {
      return fdr::detail::gsIsModel(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_REFINED_H
