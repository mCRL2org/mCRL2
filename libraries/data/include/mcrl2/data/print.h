// Author(s): Jeroen van der Wulp, Wieger Wesselink
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
#include "mcrl2/data/standard.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/real.h"

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

          int precedence(const data_expression& x) const
          {
            if (is_application(x))
            {
              if (sort_bool::is_implies_application(x))
              {
                return 2;
              }
              else if (sort_bool::is_and_application(x)
                    || sort_bool::is_or_application(x)
                      )
              {
                return 3;
              }
              else if (data::is_equal_to_application(x)
                    || data::is_not_equal_to_application(x)
                      )
              {
                return 4;
              }
              else if (data::is_less_application(x)
                    || data::is_less_equal_application(x)
                    || data::is_greater_application(x)
                    || data::is_greater_equal_application(x)
                    || sort_list::is_in_application(x)
                      )
              {
                return 5;
              }
              else if (sort_list::is_cons_application(x))
              {
                return 6;
              }
              else if (sort_list::is_snoc_application(x))
              {
                return 7;
              }
              else if (sort_list::is_concat_application(x))
              {
                return 8;
              }
              else if (sort_real::is_plus_application(x)
                    || sort_real::is_minus_application(x)
                    || sort_set::is_setunion_application(x)
                    || sort_set::is_setdifference_application(x)
                    || sort_bag::is_bagjoin_application(x)
                    || sort_bag::is_bagdifference_application(x)
                  )
              {
                return 9;
              }
              else if (sort_int::is_div_application(x)
                    || sort_int::is_mod_application(x)
                    || sort_real::is_divides_application(x)
                      )
              {
                return 10;
              }
              else if (sort_int::is_times_application(x)
                    || sort_list::is_element_at_application(x)
                    || sort_set::is_setintersection_application(x)
                    || sort_bag::is_bagintersect_application(x)
                      )
              {
                return 11;
              }
            }
            return -1;
          }

          bool is_numeric_cast(const data_expression& x)
          {
            return data::sort_nat::is_pos2nat_function_symbol(x)  
                || data::sort_int::is_pos2int_function_symbol(x)  
                || data::sort_real::is_pos2real_function_symbol(x)
                || data::sort_int::is_nat2int_function_symbol(x)  
                || data::sort_real::is_nat2real_function_symbol(x)
                || data::sort_real::is_int2real_function_symbol(x)
              ;
          }

          bool is_numeric_constant(const application& x)
          {
            return data::sort_pos::is_c1_function_symbol(x.head())
                || data::sort_pos::is_cdub_application(x)  
                || data::sort_nat::is_cnat_application(x)
              ;
          }

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
            // static_cast<Derived&>(*this)(x.sort());
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
            if (sort_bool::is_implies_application(x))
            {
              super::set_sequence_separator(" => ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_bool::is_and_application(x))
            {
              super::set_sequence_separator(" && ");
              static_cast<Derived&>(*this)(x.arguments());
            }              
            else if (sort_bool::is_or_application(x))
            {
              super::set_sequence_separator(" || ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (data::is_equal_to_application(x))
            {
              super::set_sequence_separator(" == ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (data::is_not_equal_to_application(x))
            {
              super::set_sequence_separator(" != ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (data::is_less_application(x))
            {
              super::set_sequence_separator(" < ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (data::is_less_equal_application(x))
            {
              super::set_sequence_separator(" < ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (data::is_greater_application(x))
            {
              super::set_sequence_separator(" < ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (data::is_greater_equal_application(x))
            {
              super::set_sequence_separator(" < ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_list::is_in_application(x))
            {
              super::set_sequence_separator(" in ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_list::is_cons_application(x))
            {
              super::set_sequence_separator(" cons ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_list::is_snoc_application(x))
            {
              super::set_sequence_separator(" snoc ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_list::is_concat_application(x))
            {
              super::set_sequence_separator(" concat ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_real::is_plus_application(x))
            {
              super::set_sequence_separator(" + ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_real::is_minus_application(x))
            {
              super::set_sequence_separator(" - ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_set::is_setunion_application(x))
            {
              super::set_sequence_separator(" union ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_set::is_setdifference_application(x))
            {
              super::set_sequence_separator(" difference ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_bag::is_bagjoin_application(x))
            {
              super::set_sequence_separator(" bagjoin ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_bag::is_bagdifference_application(x))
            {
              super::set_sequence_separator(" bagdifference ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_int::is_div_application(x))
            {
              super::set_sequence_separator(" bagdifference ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_int::is_mod_application(x))
            {
              super::set_sequence_separator(" mod ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_real::is_divides_application(x))
            {
              super::set_sequence_separator(" divides ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_int::is_times_application(x))
            {
              super::set_sequence_separator(" * ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_list::is_element_at_application(x))
            {
              super::set_sequence_separator(" element_at ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_set::is_setintersection_application(x))
            {
              super::set_sequence_separator(" setintersection ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (sort_bag::is_bagintersect_application(x))
            {
              super::set_sequence_separator(" bagintersect ");
              static_cast<Derived&>(*this)(x.arguments());
            }
            else if (is_numeric_cast(x))
            {
              // ignore numeric casts like Pos2Nat
              static_cast<Derived&>(*this)(x.arguments().front());
            }
            else if (is_numeric_constant(x))
            {
              // TODO: fall back on old pretty printer, since it is unknown how to print numeric constants
              super::print(core::pp(x));
            }
            else
            {
              static_cast<Derived&>(*this)(x.head());
              super::set_sequence_separator(", ");
              static_cast<Derived&>(*this)(x.arguments());
            }
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

