// Author(s): Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/traverser.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_TRAVERSER_H
#define MCRL2_PROCESS_TRAVERSER_H

#include "mcrl2/process/process_specification.h"
#include "mcrl2/core/selective_traverser.h"
#include "mcrl2/data/traverser.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace process {

  /// \brief Traversal class for process data types
  template <typename Derived>
  class traverser: public data::traverser<Derived>
  {
    public:
      typedef data::traverser<Derived> super;
      using super::operator();
      using super::enter;
      using super::leave;

#include "mcrl2/lps/detail/traverser.inc.h" // needed for traversal of lps::action
#include "mcrl2/process/detail/traverser.inc.h"
  };

  /// \brief Selective traversal class for process data types
  template <typename Derived, typename AdaptablePredicate>
  class selective_traverser : public core::selective_traverser<Derived, AdaptablePredicate, process::traverser>
  {
    typedef core::selective_traverser<Derived, AdaptablePredicate, process::traverser> super;

    public:

      selective_traverser()
      { }

      selective_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_TRAVERSER_H
