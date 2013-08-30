// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/print.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_PRINT_H
#define MCRL2_LPS_PRINT_H

#include <boost/lexical_cast.hpp>

#include "mcrl2/core/print.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/print.h"
#include "mcrl2/lps/traverser.h"
#include "mcrl2/lps/state.h"

namespace mcrl2
{

namespace lps
{

using core::detail::precedences::max_precedence;

namespace detail
{

template <typename Derived>
struct printer: public lps::add_traverser_sort_expressions<data::detail::printer, Derived>
{
  typedef lps::add_traverser_sort_expressions<data::detail::printer, Derived> super;

  using super::enter;
  using super::leave;
  using super::operator();
  using super::print_assignments;
  using super::print_condition;
  using super::print_expression;
  using super::print_list;
  using super::print_variables;

  bool m_print_summand_numbers;

  printer()
    : m_print_summand_numbers(false)
  {}

  bool& print_summand_numbers()
  {
  	return m_print_summand_numbers;
  }

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  template <typename Container>
  void print_numbered_list(const Container& container,
                           const std::string& separator = ", ",
                           const std::string& number_separator = "",
                           std::size_t index = 0,
                           bool print_start_separator = false,
                           bool print_empty_container = false
                          )
  {
    if (container.empty() && !print_empty_container)
    {
      return;
    }
    for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
    {
      derived().print("\n");
      derived().print(number_separator);
      derived().print("%");
      derived().print(boost::lexical_cast<std::string>(index++));

      derived().print("\n");
      if (i == container.begin() && !print_start_separator)
      {
        derived().print(number_separator);
      }
      else
      {
        derived().print(separator);
      }
      derived()(*i);
    }
  }

  // Container contains elements of type T such that t.sort() is a sort_expression.
  template <typename Container>
  void print_action_declarations(const Container& container,
                                 const std::string& opener = "(",
                                 const std::string& closer = ")",
                                 const std::string& separator = ", "
                                )
  {
    typedef typename Container::value_type T;

    // print nothing if the container is empty
    if (container.empty())
    {
      return;
    }

    typename Container::const_iterator first = container.begin();
    typename Container::const_iterator last = container.end();

    derived().print(opener);

    while (first != last)
    {
      if (first != container.begin())
      {
        derived().print(separator);
      }

      typename Container::const_iterator i = first;
      do
      {
        ++i;
      }
      while (i != last && first->sorts() == i->sorts());

      print_list(std::vector<action_label>(first, i), "", "", ",");
      if (!first->sorts().empty())
      {
        derived().print(": ");
        print_list(first->sorts(), "", "", " # ");
      }

      first = i;
    }
    derived().print(closer);
  }

  // Container contains elements of type T such that t.sort() is a sort_expression.
  template <typename Container>
  void print_action_declarations_maximally_shared(const Container& container,
                                                  const std::string& opener = "(",
                                                  const std::string& closer = ")",
                                                  const std::string& separator = ", "
                                                 )
  {
    typedef typename Container::value_type T;

    // print nothing if the container is empty
    if (container.empty())
    {
      return;
    }

    // sort_map[s] will contain all elements t of container with t.sorts() == s.
    std::map<data::sort_expression_list, std::vector<T> > sort_map;

    // sort_lists will contain all sort expression lists s that appear as a key in sort_map,
    // in the order they are encountered in container
    std::vector<data::sort_expression_list> sort_lists;

    for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
    {
      if (sort_map.find(i->sorts()) == sort_map.end())
      {
        sort_lists.push_back(i->sorts());
      }
      sort_map[i->sorts()].push_back(*i);
    }

    // do the actual printing
    derived().print(opener);
    for (std::vector<data::sort_expression_list>::iterator i = sort_lists.begin(); i != sort_lists.end(); ++i)
    {
      if (i != sort_lists.begin())
      {
        derived().print(separator);
      }
      const std::vector<T>& v = sort_map[*i];
      print_list(v, "", "", ",");
      if (!i->empty())
      {
        derived().print(": ");
        print_list(*i, "", "", " # ");
      }
    }
    derived().print(closer);
  }

  void operator()(const lps::action_label& x)
  {
    derived().enter(x);
    derived()(x.name());
    derived().leave(x);
  }

  void operator()(const lps::action& x)
  {
    derived().enter(x);
    derived()(x.label());
    print_list(x.arguments(), "(", ")", ", ");
    derived().leave(x);
  }

  void operator()(const lps::untyped_action& x)
  {
    derived().enter(x);
    derived()(x.name());
    print_list(x.arguments(), "(", ")", ", ");
    derived().leave(x);
  }

  void operator()(const lps::deadlock& x)
  {
    derived().enter(x);
    derived().print("delta");
    if (x.has_time())
    {
      derived().print(" @ ");
      print_expression(x.time(), max_precedence);
    }
    derived().leave(x);
  }

  void operator()(const lps::multi_action& x)
  {
    derived().enter(x);
    if (x.actions().empty())
    {
      derived().print("tau");
    }
    else
    {
      print_list(x.actions(), "", "", "|");
    }
    if (x.has_time())
    {
      derived().print(" @ ");
      print_expression(x.time(), max_precedence);
    }
    derived().leave(x);
  }

  void operator()(const lps::deadlock_summand& x)
  {
    derived().enter(x);
    print_variables(x.summation_variables(), true, true, false, "sum ", ".\n         ", ",");
    print_condition(x.condition(), " ->\n         ", max_precedence);
    derived()(x.deadlock());
    derived().leave(x);
  }

  void operator()(const lps::action_summand& x)
  {
    derived().enter(x);
    print_variables(x.summation_variables(), true, true, false, "sum ", ".\n         ", ",");
    print_condition(x.condition(), " ->\n         ", max_precedence);
    derived()(x.multi_action());
    derived().print(" .\n         ");
    derived().print("P(");
    print_assignments(x.assignments(), true, "", "", ", ");
    derived().print(")");
    derived().leave(x);
  }

  void operator()(const lps::process_initializer& x)
  {
    derived().enter(x);
    derived().print("init P");
    print_assignments(x.assignments(), false, "(", ")", ", ");
    derived().print(";");
    derived().leave(x);
  }

  void operator()(const lps::linear_process& x)
  {
    derived().enter(x);
    derived().print("proc P");
    print_variables(x.process_parameters(), true, true, false, "(", ")", ", ");

    if (m_print_summand_numbers)
    {
      derived().print(" =");

      std::string separator        = "     + ";
      std::string number_separator = "       ";

      // print action summands
      print_numbered_list(x.action_summands(), separator, number_separator, 1, false);

      // print deadlock summands
      print_numbered_list(x.deadlock_summands(), separator, number_separator, x.action_summands().size() + 1, true);

      // print delta if there are no summands
      if (x.action_summands().empty() && (x.deadlock_summands().empty()))
      {
        deadlock_summand_vector v;
        v.push_back(deadlock_summand(data::variable_list(), data::sort_bool::true_(), lps::deadlock(data::parse_data_expression("0"))));
        print_numbered_list(v, separator, number_separator, 1, true);
      }
    }
    else
    {
      derived().print(" =\n       ");

      // print action summands
      std::string opener = "";
      std::string closer = "";
      std::string separator = "\n     + ";
      print_list(x.action_summands(), opener, closer, separator);

      // print deadlock summands
      if (!x.action_summands().empty())
      {
        opener = separator;
      }
      print_list(x.deadlock_summands(), opener, closer, separator);

      // print delta if there are no summands
      if (x.action_summands().empty() && (x.deadlock_summands().empty()))
      {
        deadlock_summand_vector v;
        v.push_back(deadlock_summand(data::variable_list(), data::sort_bool::true_(), lps::deadlock(data::parse_data_expression("0"))));
        print_list(v, opener, closer, separator);
      }
    }

    derived().print(";\n");
    derived().leave(x);
  }

  void operator()(const lps::specification& x)
  {
    derived().enter(x);
    derived()(x.data());
    print_action_declarations(x.action_labels(), "act  ",";\n\n", ";\n     ");
    print_variables(x.global_variables(), true, true, true, "glob ", ";\n\n", ";\n     ");
    derived()(x.process());
    derived().print("\n");
    derived()(x.initial_process());
    derived().print("\n");
    derived().leave(x);
  }

  void operator()(const lps::state &x)
  {
    derived().enter(x);
    derived().print("state(");
    bool first = true;
    for (lps::state::const_iterator i = x.begin(); i != x.end(); i++)
    {
      if (!first)
      {
        derived().print(", ");
      }
      first = false;
      print_expression(*i);
    }
    derived().print(")");
    derived().leave(x);
  }
};

} // namespace detail

/// \brief Prints the object x to a stream.
struct stream_printer
{
  template <typename T>
  void operator()(const T& x, std::ostream& out)
  {
    core::detail::apply_printer<lps::detail::printer> printer(out);
    printer(x);
  }
};

/// \brief Returns a string representation of the object x.
template <typename T>
std::string pp(const T& x)
{
  std::ostringstream out;
  stream_printer()(x, out);
  return out.str();
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_PRINT_H
