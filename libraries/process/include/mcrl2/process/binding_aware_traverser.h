// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/binding_aware_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_BINDING_AWARE_TRAVERSER_H
#define MCRL2_PROCESS_BINDING_AWARE_TRAVERSER_H

#include "mcrl2/data/binding_aware_traverser.h"
#include "mcrl2/process/process_specification.h"

namespace mcrl2 {

namespace process {

  template <typename Derived>
  struct binding_aware_traverser: public data::binding_aware_traverser<Derived>
  {
    typedef data::binding_aware_traverser<Derived> super;

    using super::operator();
    using super::enter;
    using super::leave;

#include "mcrl2/lps/detail/traverser.inc.h" // needed for traversal of lps::action
#include "mcrl2/process/detail/traverser.inc.h"
  };

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_BINDING_AWARE_TRAVERSER_H
