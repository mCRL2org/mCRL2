// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/selective_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SELECTIVE_TRAVERSER_H
#define MCRL2_DATA_SELECTIVE_TRAVERSER_H

#include "mcrl2/core/selective_traverser.h"

namespace mcrl2 {

namespace data {

/*
  /// \brief Selective traversal class for data library data types
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
*/

  /**
   * \brief expression traverser that can be used for cases in which parts
   * of the expression should not be traversed
   *
   * Types:
   *  \arg Derived the type of a derived class, as per CRTP
   *  \arg AdaptablePredicate is a unary predicate on expressions
   *
   * Before a subterm is explored the predicate is applied to see whether
   * traversal should continue.
   *
   * \see traverser
   **/
  template <typename Derived, typename AdaptablePredicate, template <class> class Traverser = core::selective_traverser>
  class selective_traverser : public Traverser<Derived>
  {
    public:
      typedef Traverser<Derived> super;

    protected:     
      void forward_call(data_expression const& e)
      { }

      void forward_call(sort_expression const& e)
      { }

    public:

      // Default constructor (only works if SelectionPredicate is Default Constructible)
      selective_traverser()
      { }

      selective_traverser(AdaptablePredicate predicate) : m_traverse_condition(predicate)
      { }
  };

  template <typename Derived, typename AdaptablePredicate>
  class selective_data_traverser : public selective_traverser<Derived, AdaptablePredicate>
  {
    typedef selective_traverser<Derived, AdaptablePredicate> super;

    public:

      selective_data_traverser()
      { }

      selective_data_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SELECTIVE_TRAVERSER_H
