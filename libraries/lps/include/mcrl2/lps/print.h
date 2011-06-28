// Author(s): Jeroen Keiren
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

#include "mcrl2/core/print.h"
#include "mcrl2/data/print.h"
#include "mcrl2/lps/traverser.h"

namespace mcrl2
{

namespace lps
{

/// \brief Pretty prints a term.
/// \param[in] t A term
template <typename T>
std::string pp(const T& t)
{
  return core::pp(t);
}

namespace detail
{

template <typename Derived>
struct printer: public lps::add_traverser_sort_expressions<data::detail::printer, Derived>
{
  typedef lps::add_traverser_sort_expressions<data::detail::printer, Derived> super;

  using super::enter;
  using super::leave;
  using super::operator();
  using core::detail::printer<Derived>::print_sorts;
  using super::print_assignments;
  using super::print_condition;
  using super::print_list;
  using super::print_time;
  using super::print_variables;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
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

  void operator()(const lps::deadlock& x)
  {
    derived().enter(x);
    derived().print("delta");
    if (x.has_time())
    {
      derived().print("@");
      print_time(x.time());
    }
    derived().leave(x);
  }

  void operator()(const lps::multi_action& x)
  {
    derived().enter(x);
    derived()(x.actions());
    if (x.has_time())
    {
      derived().print("@");
      print_time(x.time());
    }
    derived().leave(x);
  }

  void operator()(const lps::deadlock_summand& x)
  {
    derived().enter(x);
    derived()(x.summation_variables());
    derived()(x.condition());
    derived()(x.deadlock());
    derived().leave(x);
  }

  void operator()(const lps::action_summand& x)
  {
    derived().enter(x);
    print_variables(x.summation_variables(), true, true, "sum ", ".\n         ", ", ");
    print_condition(x.condition(), " ->\n         ");
    derived()(x.multi_action());
    derived().print(" .\n         ");
    derived().print("P");
    print_assignments(x.assignments(), true, "P(", ")", ", ");
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
    print_sorts() = true;
    print_list(x.process_parameters(), "(", ")", ", ");
    print_sorts() = false;
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

    derived().leave(x);
  }

  void operator()(const lps::specification& x)
  {
    derived().enter(x);
    derived()(x.data());
    print_action_declarations(x.action_labels(), "act  ",";\n\n", ";\n     ");
    print_variables(x.global_variables(), true, true, "glob ", ";\n\n", ";\n     ");
    derived()(x.process());
    derived().print(";\n\n");
    derived()(x.initial_process());
    derived().leave(x);
  }
};

} // namespace detail

/// \brief Prints the object t to a stream.
template <typename T>
void print(const T& t, std::ostream& out)
{
  core::detail::apply_printer<lps::detail::printer> printer(out);
  printer(t);
}

/// \brief Returns a string representation of the object t.
template <typename T>
std::string print(const T& t)
{
  std::ostringstream out;
  lps::print(t, out);
  return out.str();
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_PRINT_H
