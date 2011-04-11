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

#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/detail/precedence.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/multiple_possible_sorts.h"
#include "mcrl2/data/unknown_sort.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/real.h"

namespace atermpp
{
namespace detail
{

// This is here to make the std::list container work with the pp container overload.
template < typename T >
struct is_container_impl< std::list< T > >
{
  typedef boost::true_type type;
};

} // namespace detail
} // namespace atermpp

namespace mcrl2
{

namespace data
{

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

namespace detail
{

template <typename Derived>
struct printer: public data::add_traverser_sort_expressions<core::detail::printer, Derived>
{
  typedef data::add_traverser_sort_expressions<core::detail::printer, Derived> super;

  // using super::enter;
  // using super::leave;
  using super::operator();

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  template <typename Container>
  void print_container(const Container& container,
                       int container_precedence = -1,
                       const std::string& separator = ", ",
                       const std::string& open_bracket = "(",
                       const std::string& close_bracket = ")"
                      )
  {
    for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
    {
      if (i != container.begin())
      {
        derived().print(separator);
      }
      bool print_brackets = (container.size() > 1) && (data::detail::precedence(*i) < container_precedence);
      if (print_brackets)
      {
        derived().print(open_bracket);
      }
      derived()(*i);
      if (print_brackets)
      {
        derived().print(close_bracket);
      }
    }
  }

#ifdef MCRL2_PRINT_DEBUG
  std::string print_debug(const variable& x)
  {
    std::string result = pp(x);
    if (derived().print_sorts())
    {
      result = result + ": " + pp(x.sort());
    }
    return result;
  }
#endif

  bool is_cons_list(data_expression x) const
  {
    while (sort_list::is_cons_application(x))
    {
      x = sort_list::tail(x);
    }
    return sort_list::is_nil_function_symbol(x);
  }

  bool is_snoc_list(data_expression x) const
  {
    while (sort_list::is_snoc_application(x))
    {
      x = sort_list::rtail(x);
    }
    return sort_list::is_nil_function_symbol(x);
  }

  bool is_fset_cons_list(data_expression x)
  {
    while (sort_fset::is_fset_cons_application(x))
    {
      x = sort_fset::tail(x);
    }
    return sort_fset::is_fset_empty_function_symbol(x);
  }

  bool is_fset_insert_list(data_expression x)
  {
    while (sort_fset::is_fsetinsert_application(x))
    {
      x = sort_fset::right(x);
    }
    return sort_fset::is_fset_empty_function_symbol(x);
  }

  bool is_numeric_cast(const data_expression& x)
  {
    return data::sort_nat::is_pos2nat_application(x)
           || data::sort_int::is_pos2int_application(x)
           || data::sort_real::is_pos2real_application(x)
           || data::sort_int::is_nat2int_application(x)
           || data::sort_real::is_nat2real_application(x)
           || data::sort_real::is_int2real_application(x)
           ;
  }

  bool is_numeric_constant(const application& x)
  {
    return data::sort_pos::is_c1_function_symbol(x.head())
           || data::sort_nat::is_c0_function_symbol(x.head())
           || data::sort_pos::is_cdub_application(x)
           || data::sort_nat::is_cnat_application(x)
           ;
  }

  bool is_fset_true(data_expression x)
  {
    return sort_bool::is_true_function_symbol(sort_set::left(x));
  }

  bool is_fset_false(data_expression x)
  {
    return sort_bool::is_false_function_symbol(sort_set::left(x));
  }

  bool is_fset_lambda(data_expression x)
  {
    return is_lambda(sort_set::left(x)) && sort_fset::is_fset_empty_function_symbol(sort_set::right(x));
  }

  void print_cons_list(data_expression x)
  {
    data_expression_vector arguments;
    while (sort_list::is_cons_application(x))
    {
      arguments.push_back(sort_list::head(x));
      x = sort_list::tail(x);
    }
    derived().print("[");
    print_container(arguments, 6);
    derived().print("]");
  }

  void print_snoc_list(data_expression x)
  {
    data_expression_vector arguments;
    while (sort_list::is_snoc_application(x))
    {
      arguments.insert(arguments.begin(), sort_list::rhead(x));
      x = sort_list::rtail(x);
    }
    derived().print("[");
    print_container(arguments, 7);
    derived().print("]");
  }

  void print_fset_cons_list(data_expression x)
  {
    data_expression_vector arguments;
    while (sort_fset::is_fset_cons_application(x))
    {
      arguments.push_back(sort_fset::head(x));
      x = sort_fset::tail(x);
    }
    derived().print("{");
    print_container(arguments, 6);
    derived().print("}");
  }

  void print_fset_insert_list(data_expression x)
  {
    data_expression_vector arguments;
    while (sort_fset::is_fsetinsert_application(x))
    {
      arguments.push_back(sort_fset::left(x));
      x = sort_fset::right(x);
    }
    derived().print("{");
    print_container(arguments, 6);
    derived().print("}");
  }
  
  void print_fset_true(data_expression x)
  {
    derived().print("!{");
//std::cout << "\n<fset_true>" << core::pp(x) << " " << x << std::endl;
    // TODO: compute the complement of the set
    derived()(sort_set::right(x));
    derived().print("}");
  }
  
  void print_fset_false(data_expression x)
  {
    derived().print("{");
//std::cout << "\n<fset_false>" << core::pp(x) << " " << x << std::endl;
    derived()(sort_set::right(x));
    derived().print("}");   
  }
  
  void print_fset_lambda(data_expression x)
  {
    sort_expression s = function_sort(sort_set::left(x).sort()).domain().front(); // the sort of the set elements
    data::lambda left(sort_set::left(x));

//std::cout << "\n<fset_lambda>" << core::pp(left.variables()) << std::endl;

    derived().print("{ ");
    derived()(left.variables());
    derived().print(" | ");
    derived()(left.body());
    derived().print(" }");
  }
  
  void print_fset_default(data_expression x)
  {
//std::cout << "\n<fset_default>" << core::pp(x) << " " << x << std::endl;
    data_expression right = sort_set::right(x);
    // TODO: check if this is the correct way to handle this case
    if (sort_fset::is_fset_empty_function_symbol(right))
    {
      derived().print("{}");
    }
    else
    {
      sort_expression s = function_sort(sort_set::left(x).sort()).domain().front();
      variable var("x", s); // TODO: generate fresh variable w.r.t. x
      data_expression lhs(sort_set::left(x)(var));
      data_expression rhs(sort_set::setin(s, var, sort_set::setfset(s, right)));
      data_expression body = not_equal_to(lhs, rhs);
      derived().print("{ ");
      derived()(var);
      derived().print(" | ");
      derived()(body);
      derived().print(" }");
    }
  }  

  void print_abstraction(const abstraction& x, const std::string& op)
  {
    derived().enter(x);
    derived().print(op + " ");
    derived().print_sorts() = true;
    print_container(x.variables());
    derived().print_sorts() = false;
    derived().print(". ");
    derived()(x.body());
    derived().leave(x);
  }

  void operator()(const data::container_type& x)
  {
    // skip
  }

  void operator()(const data::assignment& x)
  {
    derived().enter(x);
    derived()(x.lhs());
    derived().print(":=");
    derived()(x.rhs());
    derived().leave(x);
  }

  void operator()(const data::identifier_assignment& x)
  {
    derived().enter(x);
    derived()(x.lhs());
    derived().print(":=");
    derived()(x.rhs());
    derived().leave(x);
  }

  void operator()(const data::set_or_bag_comprehension_binder& x)
  {
    derived().enter(x);
    derived().leave(x);
  }

  void operator()(const data::set_comprehension_binder& x)
  {
    derived().enter(x);
    derived().leave(x);
  }

  void operator()(const data::bag_comprehension_binder& x)
  {
    derived().enter(x);
    derived().leave(x);
  }

  void operator()(const data::forall_binder& x)
  {
    derived().enter(x);
    derived().leave(x);
  }

  void operator()(const data::exists_binder& x)
  {
    derived().enter(x);
    derived().leave(x);
  }

  void operator()(const data::lambda_binder& x)
  {
    derived().enter(x);
    derived().leave(x);
  }

  void operator()(const data::structured_sort_constructor_argument& x)
  {
    derived().enter(x);
    derived()(x.name());
    derived()(x.sort());
    derived().leave(x);
  }

  void operator()(const data::structured_sort_constructor& x)
  {
    derived().enter(x);
    derived()(x.name());
    derived()(x.arguments());
    derived()(x.recogniser());
    derived().leave(x);
  }

  void operator()(const data::list_container& x)
  {
    derived().enter(x);
    derived().leave(x);
  }

  void operator()(const data::set_container& x)
  {
    derived().enter(x);
    derived().leave(x);
  }

  void operator()(const data::bag_container& x)
  {
    derived().enter(x);
    derived().leave(x);
  }

  void operator()(const data::fset_container& x)
  {
    derived().enter(x);
    derived().leave(x);
  }

  void operator()(const data::fbag_container& x)
  {
    derived().enter(x);
    derived().leave(x);
  }

  void operator()(const data::basic_sort& x)
  {
    derived().enter(x);
    derived()(x.name());
    derived().leave(x);
  }

  void operator()(const data::container_sort& x)
  {
    derived().enter(x);
    derived()(x.container_name());
    derived()(x.element_sort());
    derived().leave(x);
  }

  void operator()(const data::structured_sort& x)
  {
    derived().enter(x);
    derived()(x.constructors());
    derived().leave(x);
  }

  void operator()(const data::function_sort& x)
  {
    derived().enter(x);
    derived()(x.domain());
    derived().print(" -> ");
    derived()(x.codomain());
    derived().leave(x);
  }

  void operator()(const data::unknown_sort& x)
  {
    derived().enter(x);
    derived().leave(x);
  }

  void operator()(const data::multiple_possible_sorts& x)
  {
    derived().enter(x);
    derived()(x.sorts());
    derived().leave(x);
  }

  void operator()(const data::identifier& x)
  {
    derived().enter(x);
    derived()(x.name());
    derived().leave(x);
  }

  void operator()(const data::variable& x)
  {
    derived().enter(x);
    derived()(x.name());
    if (derived().print_sorts())
    {
      derived().print(": ");
      derived()(x.sort());
    }
    derived().leave(x);
  }

  void operator()(const data::function_symbol& x)
  {
    derived().enter(x);
    derived()(x.name());
//    derived()(x.sort());
    derived().leave(x);
  }

  void operator()(const data::application& x)
  {
    derived().enter(x);

    //-------------------------------------------------------------------//
    //                            sort_bool
    //-------------------------------------------------------------------//
    if (sort_bool::is_implies_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " => ");
    }
    else if (sort_bool::is_and_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " && ");
    }
    else if (sort_bool::is_or_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " || ");
    }

    //-------------------------------------------------------------------//
    //                            "data"
    //-------------------------------------------------------------------//
    else if (data::is_equal_to_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " == ");
    }
    else if (data::is_not_equal_to_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " != ");
    }
    else if (data::is_less_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " < ");
    }
    else if (data::is_less_equal_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " <= ");
    }
    else if (data::is_greater_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " > ");
    }
    else if (data::is_greater_equal_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " >= ");
    }

    //-------------------------------------------------------------------//
    //                            sort_real
    //-------------------------------------------------------------------//
    else if (sort_real::is_plus_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " + ");
    }
    else if (sort_real::is_minus_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " - ");
    }
    else if (sort_real::is_divides_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " / ");
    }

    //-------------------------------------------------------------------//
    //                            sort_list
    //-------------------------------------------------------------------//
    else if (sort_list::is_in_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " in ");
    }
    else if (sort_list::is_concat_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " ++ ");
    }
    else if (sort_list::is_element_at_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " . ");
    }
    else if (sort_list::is_cons_application(x))
    {
      if (is_cons_list(x))
      {
        print_cons_list(x);
      }
      else
      {
        print_container(x.arguments(), data::detail::precedence(x), " |> ");
      }
    }
    else if (sort_list::is_snoc_application(x))
    {
      if (is_snoc_list(x))
      {
        print_snoc_list(x);
      }
      else
      {
        print_container(x.arguments(), data::detail::precedence(x), " <| ");
      }
    }

    //-------------------------------------------------------------------//
    //                            sort_set
    //-------------------------------------------------------------------//
    else if (sort_set::is_setunion_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " + ");
    }
    else if (sort_set::is_setdifference_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " - ");
    }
    else if (sort_set::is_setintersection_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " * ");
    }
    else if (sort_set::is_setconstructor_application(x))
    {
      if (is_fset_true(x))
      {
        print_fset_true(x);
      }
      else if (is_fset_false(x))
      {
        print_fset_false(x);
      }
      else if (is_fset_lambda(x))
      {
        print_fset_lambda(x);
      }
      else
      {
        print_fset_default(x);
      }
    }
    else if (sort_set::is_setcomprehension_application(x))
    {
      sort_expression s = function_sort(sort_set::arg(x).sort()).domain().front();
      variable var("x", s); // TODO: generate fresh variable w.r.t. x
      data_expression body(sort_set::arg(x)(var));
      derived().print("{ ");
      derived()(var);
      derived().print(" | ");
      derived()(body);
      derived().print(" }");   
    }
    else if (sort_set::is_setfset_application(x))
    {
      //std::cout << "\n<setfset>" << core::pp(x) << " " << x << std::endl;
      data_expression y = sort_set::arg(x);
      if (sort_fset::is_fset_empty_function_symbol(y))
      {
        derived().print("{}");
      }
      else
      {
        derived()(y);
      }
    }
    else if (sort_fset::is_fset_cons_application(x))
    {
      if (is_fset_cons_list(x))
      {
        print_fset_cons_list(x);
      }
    }
    else if (sort_fset::is_fsetinsert_application(x))
    {
      if (is_fset_insert_list(x))
      {
        print_fset_insert_list(x);
      }
    }

    //-------------------------------------------------------------------//
    //                            sort_bag
    //-------------------------------------------------------------------//
    else if (sort_bag::is_bagjoin_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " + ");
    }
    else if (sort_bag::is_bagdifference_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " - ");
    }
    else if (sort_bag::is_bagintersect_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " * ");
    }

    //-------------------------------------------------------------------//
    //                            sort_int
    //-------------------------------------------------------------------//
    else if (sort_int::is_div_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " / ");
    }
    else if (sort_int::is_mod_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " % ");
    }
    else if (sort_int::is_times_application(x))
    {
      print_container(x.arguments(), data::detail::precedence(x), " * ");
    }

    //-------------------------------------------------------------------//
    //                            numeric values
    //-------------------------------------------------------------------//
    else if (is_numeric_cast(x))
    {
      // ignore numeric casts like Pos2Nat
      derived()(x.arguments().front());
    }
    else if (is_numeric_constant(x))
    {
      // TODO: fall back on old pretty printer, since it is unknown how to print numeric constants
      derived().print(core::pp(x));
    }

    //-------------------------------------------------------------------//
    //                            default case
    //-------------------------------------------------------------------//
    else
    {
      //std::cout << "\n<default>" << core::pp(x) << "</default>\n";
      derived()(x.head());
      if (x.arguments().size() > 0)
      {
        derived().print("(");
      }
      print_container(x.arguments());
      if (x.arguments().size() > 0)
      {
        derived().print(")");
      }
    }
    derived().leave(x);
  }

  void operator()(const data::where_clause& x)
  {
    derived().enter(x);
    derived()(x.body());
    derived()(x.declarations());
    derived().leave(x);
  }

  void operator()(const data::forall& x)
  {
    print_abstraction(x, "forall");
  }

  void operator()(const data::exists& x)
  {
    print_abstraction(x, "exists");
  }

  void operator()(const data::lambda& x)
  {
    print_abstraction(x, "lambda");
  }

  void operator()(const data::data_equation& x)
  {
    derived().enter(x);
    derived()(x.variables());
    derived()(x.condition());
    derived()(x.lhs());
    derived()(x.rhs());
    derived().leave(x);
  }
};

} // namespace detail

/// \brief Prints the object t to a stream.
template <typename T>
void print(const T& t, std::ostream& out)
{
  core::detail::apply_printer<data::detail::printer> printer(out);
  printer(t);
}

/// \brief Returns a string representation of the object t.
template <typename T>
std::string print(const T& t)
{
  std::ostringstream out;
  data::print(t, out);
  return out.str();
}

} // namespace data

} // namespace mcrl2

#endif

