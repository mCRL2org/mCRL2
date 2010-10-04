// Author(s): Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/traverser.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_TRAVERSER_H
#define MCRL2_DATA_TRAVERSER_H

#include "mcrl2/core/traverser.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/multiple_possible_sorts.h"
#include "mcrl2/data/unknown_sort.h"

namespace mcrl2 {

namespace data {

  /// \brief Traversal class for data library types
  template <typename Derived>
  class traverser: public core::traverser<Derived>
  {
    public:
      typedef core::traverser<Derived> super;
      using super::operator();
      using super::enter;
      using super::leave;

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/data/detail/traverser.inc.h"
  };

  /// \brief Selective traversal class for BES data types
  template <typename Derived, typename AdaptablePredicate>
  class selective_traverser : public core::selective_traverser<Derived, AdaptablePredicate, data::traverser>
  {
    typedef core::selective_traverser<Derived, AdaptablePredicate, data::traverser> super;

    public:

      selective_traverser()
      { }

      selective_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_TRAVERSER_H
