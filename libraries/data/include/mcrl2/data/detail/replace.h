// Author(s): Wieger Wesselink, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/replace.h
/// \brief Contains a function for replacing data variables in a term.

#ifndef MCRL2_DATA_DETAIL_REPLACE_H
#define MCRL2_DATA_DETAIL_REPLACE_H

#include <algorithm>
#include <iterator>
#include <utility>

#include "boost/config.hpp"
#include "boost/assert.hpp"
#include "boost/type_traits/add_reference.hpp"
#include "boost/type_traits/remove_reference.hpp"
#include "boost/utility/enable_if.hpp"

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/core/substitution_function.h"
#include "mcrl2/core/deprecation.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/detail/manipulator.h"
#include "mcrl2/data/detail/concepts.h"

namespace mcrl2 {
  namespace data {

    /// \cond INTERNAL_DOCS
    namespace detail {

      // Component for doing top-down replacement of variables by expressions.
      //
      // Binding is disregarded. Especially, replacements are also executed on
      // the left-hand sides of assignments. The Derived type parameter
      // represents the type of a derived class (as per CRTP).
      //
      // The means of specifying and execution of replacement is deferred to the derived class.
      template < typename Substitution >
      class variable_replace_helper : public expression_manipulator< variable_replace_helper< Substitution > >
      {
        BOOST_CONCEPT_ASSERT((concepts::Substitution< typename boost::remove_const<
                                        typename boost::remove_reference< Substitution >::type >::type>));

        protected:

          Substitution m_substitution;

          typedef expression_manipulator< variable_replace_helper< Substitution > > super;

        public:

          using super::operator();

#if BOOST_MSVC
          // Workaround for malfunctioning MSVC 2008 overload resolution
          template < typename Container >
          Container operator()(Container const& a)
          {
            return super::operator()(a);
          }
#endif

          data_expression operator()(variable const& v)
          {
            return m_substitution(v);
          }

          variable_replace_helper()
          {}

          variable_replace_helper(typename boost::add_reference< Substitution >::type s) : m_substitution(s)
          { }
      };

      // Component for doing top-down replacement of free variables by expressions.
      template < typename Substitution >
      class free_variable_replace_helper : public binding_aware_expression_manipulator< free_variable_replace_helper< Substitution > >
      {
        BOOST_CONCEPT_ASSERT((concepts::Substitution< typename boost::remove_const<
                                        typename boost::remove_reference< Substitution >::type >::type>));

        typedef binding_aware_expression_manipulator< free_variable_replace_helper< Substitution > > super;

        protected:

          Substitution m_substitution;

          // Check that variables in replaced expressions do not become bound
          bool check_replacement_assumption(variable const& v)
          {
            std::set< variable > free_variables(find_free_variables(m_substitution(v), super::m_bound));
            std::set< variable > result;

            std::set_intersection(free_variables.begin(), free_variables.end(),
                                  super::m_bound.begin(), super::m_bound.end(), std::inserter(result, result.end()));

            return result.empty();
          }

        public:

          using super::operator();

#if BOOST_MSVC
          // Workaround for malfunctioning MSVC 2008 overload resolution
          template < typename Container >
          Container operator()(Container const& a)
          {
            return super::operator()(a);
          }
#endif

          assignment operator()(assignment const& a)
          {
            return assignment(a.lhs(), (*this)(a.rhs()));
          }

          data_expression operator()(variable const& v)
          {
            BOOST_ASSERT((super::m_bound.find(v) != super::m_bound.end()) || check_replacement_assumption(v));

            return (super::is_bound(v)) ? static_cast< data_expression >(v) : static_cast< data_expression >(m_substitution(v));
          }

          free_variable_replace_helper()
          {}

          free_variable_replace_helper(typename boost::add_reference< Substitution >::type s) : m_substitution(s)
          {}

          template < typename Sequence >
          free_variable_replace_helper(Sequence const& bound, typename boost::add_reference< Substitution >::type s) : super(bound), m_substitution(s)
          {}
      };
    } // namespace detail
    /// \endcond

  } // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_REPLACE_H
