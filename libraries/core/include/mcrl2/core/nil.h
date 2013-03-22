// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/nil.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_NIL_H
#define MCRL2_CORE_NIL_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/soundness_checks.h"

namespace mcrl2 {

namespace core {

//--- start generated class nil ---//
/// \brief The value nil
class nil: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    nil()
      : atermpp::aterm_appl(core::detail::constructNil())
    {}

    /// \brief Constructor.
    /// \param term A term
    nil(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_Nil(*this));
    }
};

/// \brief list of nils
typedef atermpp::term_list<nil> nil_list;

/// \brief vector of nils
typedef std::vector<nil>    nil_vector;

//--- end generated class nil ---//

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_NIL_H
