// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/refined.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_REFINED_H
#define MCRL2_FDR_REFINED_H

#include "mcrl2/fdr/term_include_files.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief A model
class Refined: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    Refined()
      : atermpp::aterm_appl(fdr::detail::constructRefined())
    {}

    /// \brief Constructor.
    /// \param term A term
    Refined(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Refined(m_term));
    }

    /// \brief Constructor.
    Refined(const model_expression& m)
      : atermpp::aterm_appl(fdr::detail::gsMakeRefined(m))
    {}

    model_expression m() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief list of Refineds
typedef atermpp::term_list<Refined> Refined_list;

/// \brief vector of Refineds
typedef atermpp::vector<Refined>    Refined_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_REFINED_H
