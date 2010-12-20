// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/traverser.h
/// \brief add your file description here.

// To avoid circular inclusion problems
#ifndef MCRL2_LPS_SPECIFICATION_H
#include "mcrl2/lps/specification.h"
#endif

#ifndef MCRL2_LPS_TRAVERSER_H
#define MCRL2_LPS_TRAVERSER_H

#include "mcrl2/data/traverser.h"

namespace mcrl2 {

namespace lps {

  /// \brief Traversal class for LPS data types
  template <typename Derived>
  class traverser: public data::traverser<Derived>
  {
    public:
      typedef data::traverser<Derived> super;
      using super::enter;
      using super::leave;
      using super::operator();

#include "mcrl2/lps/detail/traverser.inc.h"
  };

  template <typename Derived>
  class binding_aware_traverser_base: public data::binding_aware_traverser<Derived>
  {
    public:
      typedef data::binding_aware_traverser<Derived> super;
      using super::operator();
      using super::enter;
      using super::leave;
      using super::increase_bind_count;
      using super::decrease_bind_count;

#include "mcrl2/lps/detail/traverser.inc.h"
  };

  template <typename Derived>
  class binding_aware_traverser: public binding_aware_traverser_base<Derived>
  {
    public:
      typedef binding_aware_traverser_base<Derived> super;
      using super::operator();
      using super::enter;
      using super::leave;
      using super::increase_bind_count;
      using super::decrease_bind_count;

      void operator()(const action_summand& x)
      {
        increase_bind_count(x.summation_variables());
        super::operator()(x);
        decrease_bind_count(x.summation_variables());
      }

      void operator()(const deadlock_summand& x)
      {
        increase_bind_count(x.summation_variables());
        super::operator()(x);
        decrease_bind_count(x.summation_variables());
      }

      void operator()(const linear_process& x)
      {
        increase_bind_count(x.process_parameters());
        super::operator()(x);
        decrease_bind_count(x.process_parameters());
      }
  };

  /// \brief Selective traversal class for BES data types
  template <typename Derived, typename AdaptablePredicate>
  class selective_traverser : public core::selective_traverser<Derived, AdaptablePredicate, lps::traverser>
  {
    public:
      typedef core::selective_traverser<Derived, AdaptablePredicate, lps::traverser> super;
      using super::enter;
      using super::leave;
      using super::operator();

      selective_traverser()
      { }

      selective_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_TRAVERSER_H
