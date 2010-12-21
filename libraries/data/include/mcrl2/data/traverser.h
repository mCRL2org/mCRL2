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

#include "boost/utility/enable_if.hpp"
#include "boost/type_traits/is_base_of.hpp"

#include "mcrl2/core/traverser.h"
#include "mcrl2/data/assignment.h"
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

#include "mcrl2/data/detail/traverser.inc.h"
  };

  /// \brief Add data traversal functions to the core binding aware traverser.
  template <typename Derived>
  class binding_aware_traverser_helper: public core::binding_aware_traverser<Derived, variable>
  {
    public:
      typedef core::binding_aware_traverser<Derived, variable> super;
      typedef typename super::variable_type variable_type;
      using super::operator();
      using super::enter;
      using super::leave;

      /// \brief Constructor
      binding_aware_traverser_helper()
      {}

      /// \brief Constructor
      template <typename Container>
      binding_aware_traverser_helper(Container const& bound_variables,
                              typename atermpp::detail::enable_if_container<Container, variable_type>::type* = 0)
        : super(bound_variables)
      {}

#include "mcrl2/data/detail/traverser.inc.h"
  };

  /// \brief Handle binding variables.
  template <typename Derived>
  class binding_aware_traverser : public binding_aware_traverser_helper<Derived>
  {
    public:
      typedef binding_aware_traverser_helper<Derived> super;
      typedef typename super::variable_type variable_type;
      using super::operator();
      using super::enter;
      using super::leave;
      using super::increase_bind_count;
      using super::decrease_bind_count;

      /// \brief Constructor
      binding_aware_traverser()
      {}

      /// \brief Constructor
      template <typename Container>
      binding_aware_traverser(Container const& bound_variables,
                              typename atermpp::detail::enable_if_container<Container, variable_type>::type* = 0)
        : super(bound_variables)
      {}

      void operator()(where_clause const& x)
      {
        increase_bind_count(make_assignment_left_hand_side_range(x.declarations()));
        super::operator()(x);
        decrease_bind_count(make_assignment_left_hand_side_range(x.declarations()));
      }
      
      void operator()(lambda const& x)
      {
        increase_bind_count(x.variables());
        super::operator()(x);
        decrease_bind_count(x.variables());
      }
      
      void operator()(exists const& x)
      {
        increase_bind_count(x.variables());
        super::operator()(x);
        decrease_bind_count(x.variables());
      }
      
      void operator()(forall const& x)
      {
        increase_bind_count(x.variables());
        super::operator()(x);
        decrease_bind_count(x.variables());
      }
  };

  /// \brief Selective traversal class for data library data types
  template <typename Derived, typename AdaptablePredicate>
  class selective_traverser: public core::selective_traverser<Derived, AdaptablePredicate, data::traverser>
  {
    public:
      typedef core::selective_traverser<Derived, AdaptablePredicate, data::traverser> super;
      using super::enter;
      using super::leave;
      using super::operator();

    protected:     

      template <typename Expression>
      void forward_call(Expression const& e, typename boost::disable_if<typename boost::is_base_of<data_expression, Expression>::type>::type* = 0,
                                             typename boost::disable_if<typename boost::is_base_of<sort_expression, Expression>::type>::type* = 0)
      {
        static_cast<super&>(*this)(e);
      }

      void forward_call(data_expression const& e)
      {
        static_cast<super&>(*this)(e);
      }

      void forward_call(sort_expression const& e)
      {
        static_cast<super&>(*this)(e);
      }

      template <typename Expression>
      void forward_call(Expression const& e, typename boost::enable_if<typename boost::is_base_of<data_expression, Expression>::type>::type* = 0)
      {
        if (m_traverse_condition(e))
        {
          static_cast<super&>(*this)(e);
        }
      }

      template <typename Expression>
      void forward_call(Expression const& e, typename boost::enable_if<typename boost::is_base_of<sort_expression, Expression>::type>::type* = 0) {
        if (m_traverse_condition(e))
        {
          static_cast<super&>(*this)(e);
        }
      }

    public:
      selective_traverser()
      { }

      selective_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_TRAVERSER_H
