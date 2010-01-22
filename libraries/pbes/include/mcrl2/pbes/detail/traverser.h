// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/traverser.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_TRAVERSER_H
#define MCRL2_PBES_DETAIL_TRAVERSER_H

#include "mcrl2/pbes/pbes.h"
#include "mcrl2/data/detail/traverser.h"

namespace mcrl2 {

namespace pbes_system {

// TODO: this is a temporary solution to get access to the pbes expression classes
namespace traversal {

  template <typename Derived>
  class traverser: public data::detail::traverser<Derived>
  {
    public:
      typedef data::detail::traverser<Derived> super;
      using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/pbes/detail/traverser.inc"
  };

template<typename Derived, typename AdaptablePredicate>
class selective_data_traverser: public data::detail::selective_traverser<
		Derived, AdaptablePredicate> {
	typedef data::detail::selective_traverser<Derived, AdaptablePredicate>
			super;

public:
	selective_data_traverser() {
	}

	selective_data_traverser(AdaptablePredicate predicate) :
		super(predicate) {
	}
};

} // namespace traversal

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_TRAVERSER_H
