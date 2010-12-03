// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/print.h
/// \brief Provides utilities for pretty printing.

#ifndef MCRL2_DATA_PRINT_H
#define MCRL2_DATA_PRINT_H

#include <iostream>
#include <sstream>
#include <list>
#include <iterator>

#include "boost/utility/enable_if.hpp"

#include "mcrl2/core/print.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/list.h"
// #include "mcrl2/data/data_specification.h"
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/multiple_possible_sorts.h"
#include "mcrl2/data/unknown_sort.h"

namespace atermpp {
  namespace detail {

    // This is here to make the std::list container work with the pp container overload.
    template < typename T >
    struct is_container_impl< std::list< T > > {
      typedef boost::true_type type;
    };

  } // namespace detail
} // namespace atermpp

namespace mcrl2 {

  namespace data {

    /// \brief Pretty prints a data specification
    /// \param[in] specification a data specification
    /* inline std::string pp(data_specification const& specification)
    {
      return core::pp(detail::data_specification_to_aterm_data_spec(specification));
    } */

    /// \brief Pretty prints the contents of a container
    /// \param[in] c a container with data or sort expressions
    template < typename Container >
    inline std::string pp(Container const& c, typename atermpp::detail::enable_if_container< Container >::type* = 0)
    {
      std::string result;

      if (c.begin() != c.end())
      {
        result.append(mcrl2::core::pp(*c.begin()));

        for (typename Container::const_iterator i = ++(c.begin()); i != c.end(); ++i)
        {
          result.append(", ").append(mcrl2::core::pp(*i));
        }
      }

      return result;
    }

    /// \brief Pretty prints a data and sort expressions
    /// \param[in] c A data or sort expression
    inline std::string pp(atermpp::aterm_appl const& c)
    {
      return core::pp(c);
    }

    /// \brief Pretty prints a data and sort expressions
    /// \param[in] c A data or sort expression.
    template < typename Expression >
    inline std::string pp(atermpp::term_list< Expression > const& c)
    {
      return core::pp(c);
    }

    namespace detail {

      template <typename Derived>
      class print_traverser_base: public core::detail::print_traverser<Derived>
      {
        public:
          typedef core::detail::print_traverser<Derived> super;

          using super::enter;
          using super::leave;
          using super::operator();

          print_traverser_base(std::ostream& o)
            : super(o)
          {}
#include "mcrl2/data/detail/traverser.inc.h"
      };

      template <typename Derived>
      class print_traverser: public print_traverser_base<Derived>
      {
        public:
          typedef print_traverser_base<Derived> super;

          using super::enter;
          using super::leave;
          using super::operator();

          print_traverser(std::ostream& o)
            : super(o)
          {}
          
          void operator()(const data::assignment& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.lhs());
            super::print(":=");
            static_cast<Derived&>(*this)(x.rhs());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::identifier_assignment& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.lhs());
            super::print(":=");
            static_cast<Derived&>(*this)(x.rhs());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::set_or_bag_comprehension_binder& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::set_comprehension_binder& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::bag_comprehension_binder& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::forall_binder& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::exists_binder& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::lambda_binder& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::structured_sort_constructor_argument& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.name());
            static_cast<Derived&>(*this)(x.sort());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::structured_sort_constructor& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.name());
            static_cast<Derived&>(*this)(x.arguments());
            static_cast<Derived&>(*this)(x.recogniser());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::list_container& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::set_container& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::bag_container& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::fset_container& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::fbag_container& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::basic_sort& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.name());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::container_sort& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.container_name());
            static_cast<Derived&>(*this)(x.element_sort());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::structured_sort& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.constructors());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::function_sort& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.domain());
            super::print(" -> ");
            static_cast<Derived&>(*this)(x.codomain());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::unknown_sort& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::multiple_possible_sorts& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.sorts());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::identifier& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.name());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::variable& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.name());
            static_cast<Derived&>(*this)(x.sort());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::function_symbol& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.name());
            // static_cast<Derived&>(*this)(x.sort());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::application& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.head());
            static_cast<Derived&>(*this)(x.arguments());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::where_clause& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.body());
            static_cast<Derived&>(*this)(x.declarations());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::forall& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.variables());
            static_cast<Derived&>(*this)(x.body());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::exists& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.variables());
            static_cast<Derived&>(*this)(x.body());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::lambda& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.variables());
            static_cast<Derived&>(*this)(x.body());
            static_cast<Derived&>(*this).leave(x);
          }
          
          void operator()(const data::data_equation& x)
          {
            static_cast<Derived&>(*this).enter(x);
            static_cast<Derived&>(*this)(x.variables());
            static_cast<Derived&>(*this)(x.condition());
            static_cast<Derived&>(*this)(x.lhs());
            static_cast<Derived&>(*this)(x.rhs());
            static_cast<Derived&>(*this).leave(x);
          }         

#ifdef MCRL2_PRINT_DEBUG
          template <typename T>
          std::string print_debug(const T& t)
          {
            return pp(t);
          }
#endif
      };

    } // namespace detail

    /// \brief Prints the object t to a stream.
    template <typename T>
    void print(const T& t, std::ostream& out)
    {
      core::detail::apply_print_traverser<detail::print_traverser, std::ostringstream> printer(out);
      printer(t);
    }

    /// \brief Returns a string representation of the object t.
    template <typename T>
    std::string print(const T& t)
    {
      std::ostringstream out;
      print(t, out);
      return out.str();
    }

  } // namespace data

} // namespace mcrl2

#endif

