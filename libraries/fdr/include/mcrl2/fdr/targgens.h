// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/targgens.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_TARGGENS_H
#define MCRL2_FDR_TARGGENS_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"
#include "mcrl2/fdr/targ.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief A complex targ
class targgens: atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    targgens()
      : atermpp::aterm_appl(fdr::detail::constructTargGens())
    {}

    /// \brief Constructor.
    /// \param term A term
    targgens(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_TargGens(m_term));
    }

    /// \brief Constructor.
    targgens(const targ& argument, const generator_list& gens)
      : atermpp::aterm_appl(fdr::detail::gsMakeTargGens(argument, gens))
    {}

    targ argument() const
    {
      return atermpp::arg1(*this);
    }

    generator_list gens() const
    {
      return atermpp::list_arg2(*this);
    }
};

/// \brief list of targgenss
typedef atermpp::term_list<targgens> targgens_list;

/// \brief vector of targgenss
typedef atermpp::vector<targgens>    targgens_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_TARGGENS_H
