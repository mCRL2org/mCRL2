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
#include "mcrl2/fdr/model_expression_fwd.h"

namespace mcrl2 {

namespace fdr {

//--- start generated refined class declarations ---//
/// \brief A model
class refined: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    refined();

    /// \brief Constructor.
    /// \param term A term
    refined(atermpp::aterm_appl term);

    /// \brief Constructor.
    refined(const model_expression& m);

    model_expression m() const;
};

/// \brief list of refineds
typedef atermpp::term_list<refined> refined_list;

/// \brief vector of refineds
typedef atermpp::vector<refined>    refined_vector;

//--- end generated refined class declarations ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_REFINED_H
