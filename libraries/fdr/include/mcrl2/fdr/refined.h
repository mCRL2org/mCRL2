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

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief A model
class model: atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    model()
      : atermpp::aterm_appl(fdr::detail::constructModel())
    {}

    /// \brief Constructor.
    /// \param term A term
    model(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Model(m_term));
    }

    /// \brief Constructor.
    model(const model& m)
      : atermpp::aterm_appl(fdr::detail::gsMakeModel(m))
    {}

    model m() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief list of models
typedef atermpp::term_list<model> model_list;

/// \brief vector of models
typedef atermpp::vector<model>    model_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_REFINED_H
