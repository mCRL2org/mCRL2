// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/type_checker.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_TYPE_CHECKER_H
#define MCRL2_DATA_TYPE_CHECKER_H

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/sort_type_checker.h"
#include "mcrl2/data/standard_container_utility.h"
#include <algorithm>

namespace mcrl2 {

namespace data {

namespace detail {

inline
bool is_pos(const core::identifier_string& Number)
{
  char c = Number.function().name()[0];
  return isdigit(c) && c > '0';
}

inline
bool is_nat(const core::identifier_string& Number)
{
  return isdigit(Number.function().name()[0]) != 0;
}

inline
function_sort make_function_sort(const sort_expression& domain, const sort_expression& codomain)
{
  return function_sort({ domain }, codomain);
}

template <typename Function, typename T>
atermpp::term_list<T> transform_aterm_list(const Function& f, const atermpp::term_list<T>& x)
{
  atermpp::term_list<T> result;
  for (const T& t: x)
  {
    result.push_front(f(t));
  }
  return atermpp::reverse(result);
}

inline
sort_expression unwind_sort_expression(const sort_expression& x, const alias_vector& aliases)
{
  if (is_container_sort(x))
  {
    const container_sort& cs = atermpp::down_cast<const container_sort>(x);
    return container_sort(cs.container_name(), unwind_sort_expression(cs.element_sort(), aliases));
  }
  else if (is_function_sort(x))
  {
    const function_sort& fs = atermpp::down_cast<function_sort>(x);
    auto new_arguments = detail::transform_aterm_list([&](const sort_expression& s) { return unwind_sort_expression(s, aliases); }, fs.domain());
    return function_sort(new_arguments, unwind_sort_expression(fs.codomain(), aliases));
  }
  else if (is_basic_sort(x))
  {
    const basic_sort& bs = atermpp::down_cast<const basic_sort>(x);
    for(const alias& a: aliases)
    { 
      if (bs == a.name())
      {
        return unwind_sort_expression(a.reference(), aliases);
      }
      return x;
    }
  }
  return x;
}

inline
bool is_numeric_type(const sort_expression& x)
{
  if (data::is_untyped_sort(x))
  {
    return false;
  }
  return sort_bool::is_bool(x)  ||
         sort_pos::is_pos(x)    ||
         sort_nat::is_nat(x)    ||
         sort_int::is_int(x)    ||
         sort_real::is_real(x);
}

} // namespace detail

class type_checker: public sort_type_checker
{
  protected:
    std::map<core::identifier_string, sort_expression_list> m_system_constants;
    std::map<core::identifier_string, function_sort_list> m_system_functions;
    std::map<core::identifier_string, sort_expression> m_user_constants;
    std::map<core::identifier_string, function_sort_list> m_user_functions;

    void add_system_constant(const data::function_symbol& f)
    {
      // append the Type to the entry of the Name of the OpId in system constants table
      auto i = m_system_constants.find(f.name());
      sort_expression_list sorts;
      if (i != m_system_constants.end())
      {
        sorts = i->second;
      }
      sorts = push_back(sorts, f.sort());
      m_system_constants[f.name()] = sorts;
    }

    void add_system_function(const data::function_symbol& f)
    {
      m_system_functions[f.name()] = m_system_functions[f.name()] + function_sort_list({ atermpp::down_cast<function_sort>(f.sort()) });
    }

    sort_expression unwind_sort_expression(const sort_expression& x) const
    {
      return detail::unwind_sort_expression(x, get_sort_specification().user_defined_aliases());
    }

    bool equal_types(const sort_expression& x1, const sort_expression& x2) const
    {
      if (x1 == x2)
      {
        return true;
      }
      return unwind_sort_expression(x1) == unwind_sort_expression(x2);
    }

    bool find_sort(const sort_expression& x, const function_sort_list& sorts) const
    {
      return std::any_of(sorts.begin(), sorts.end(), [&](const function_sort& s) { return equal_types(x, s); });
    }

    void initialise_system_defined_functions()
    {
      //Creation of operation identifiers for system defined operations.
      //Bool
      add_system_constant(sort_bool::true_());
      add_system_constant(sort_bool::false_());
      add_system_function(sort_bool::not_());
      add_system_function(sort_bool::and_());
      add_system_function(sort_bool::or_());
      add_system_function(sort_bool::implies());
      add_system_function(equal_to(data::untyped_sort()));
      add_system_function(not_equal_to(data::untyped_sort()));
      add_system_function(if_(data::untyped_sort()));
      add_system_function(less(data::untyped_sort()));
      add_system_function(less_equal(data::untyped_sort()));
      add_system_function(greater_equal(data::untyped_sort()));
      add_system_function(greater(data::untyped_sort()));
      //Numbers
      add_system_function(sort_nat::pos2nat());
      add_system_function(sort_nat::cnat());
      add_system_function(sort_real::pos2real());
      add_system_function(sort_nat::nat2pos());
      add_system_function(sort_int::nat2int());
      add_system_function(sort_int::cint());
      add_system_function(sort_real::nat2real());
      add_system_function(sort_int::int2pos());
      add_system_function(sort_int::int2nat());
      add_system_function(sort_real::int2real());
      add_system_function(sort_real::creal());
      add_system_function(sort_real::real2pos());
      add_system_function(sort_real::real2nat());
      add_system_function(sort_real::real2int());
      add_system_constant(sort_pos::c1());
      //Square root for the natural numbers.
      add_system_function(sort_nat::sqrt());
      //more about numbers
      add_system_function(sort_real::maximum(sort_pos::pos(),sort_pos::pos()));
      add_system_function(sort_real::maximum(sort_pos::pos(),sort_nat::nat()));
      add_system_function(sort_real::maximum(sort_nat::nat(),sort_pos::pos()));
      add_system_function(sort_real::maximum(sort_nat::nat(),sort_nat::nat()));
      add_system_function(sort_real::maximum(sort_pos::pos(),sort_int::int_()));
      add_system_function(sort_real::maximum(sort_int::int_(),sort_pos::pos()));
      add_system_function(sort_real::maximum(sort_nat::nat(),sort_int::int_()));
      add_system_function(sort_real::maximum(sort_int::int_(),sort_nat::nat()));
      add_system_function(sort_real::maximum(sort_int::int_(),sort_int::int_()));
      add_system_function(sort_real::maximum(sort_real::real_(),sort_real::real_()));
      //more
      add_system_function(sort_real::minimum(sort_pos::pos(), sort_pos::pos()));
      add_system_function(sort_real::minimum(sort_nat::nat(), sort_nat::nat()));
      add_system_function(sort_real::minimum(sort_int::int_(), sort_int::int_()));
      add_system_function(sort_real::minimum(sort_real::real_(), sort_real::real_()));
      //more
      // add_system_function(sort_real::abs(sort_pos::pos()));
      // add_system_function(sort_real::abs(sort_nat::nat()));
      add_system_function(sort_real::abs(sort_int::int_()));
      add_system_function(sort_real::abs(sort_real::real_()));
      //more
      add_system_function(sort_real::negate(sort_pos::pos()));
      add_system_function(sort_real::negate(sort_nat::nat()));
      add_system_function(sort_real::negate(sort_int::int_()));
      add_system_function(sort_real::negate(sort_real::real_()));
      add_system_function(sort_real::succ(sort_pos::pos()));
      add_system_function(sort_real::succ(sort_nat::nat()));
      add_system_function(sort_real::succ(sort_int::int_()));
      add_system_function(sort_real::succ(sort_real::real_()));
      add_system_function(sort_real::pred(sort_pos::pos()));
      add_system_function(sort_real::pred(sort_nat::nat()));
      add_system_function(sort_real::pred(sort_int::int_()));
      add_system_function(sort_real::pred(sort_real::real_()));
      add_system_function(sort_real::plus(sort_pos::pos(),sort_pos::pos()));
      add_system_function(sort_real::plus(sort_pos::pos(),sort_nat::nat()));
      add_system_function(sort_real::plus(sort_nat::nat(),sort_pos::pos()));
      add_system_function(sort_real::plus(sort_nat::nat(),sort_nat::nat()));
      add_system_function(sort_real::plus(sort_int::int_(),sort_int::int_()));
      add_system_function(sort_real::plus(sort_real::real_(),sort_real::real_()));
      //more
      add_system_function(sort_real::minus(sort_pos::pos(), sort_pos::pos()));
      add_system_function(sort_real::minus(sort_nat::nat(), sort_nat::nat()));
      add_system_function(sort_real::minus(sort_int::int_(), sort_int::int_()));
      add_system_function(sort_real::minus(sort_real::real_(), sort_real::real_()));
      add_system_function(sort_real::times(sort_pos::pos(), sort_pos::pos()));
      add_system_function(sort_real::times(sort_nat::nat(), sort_nat::nat()));
      add_system_function(sort_real::times(sort_int::int_(), sort_int::int_()));
      add_system_function(sort_real::times(sort_real::real_(), sort_real::real_()));
      //more
      // add_system_function(sort_int::div(sort_pos::pos(), sort_pos::pos()));
      add_system_function(sort_int::div(sort_nat::nat(), sort_pos::pos()));
      add_system_function(sort_int::div(sort_int::int_(), sort_pos::pos()));
      // add_system_function(sort_int::mod(sort_pos::pos(), sort_pos::pos()));
      add_system_function(sort_int::mod(sort_nat::nat(), sort_pos::pos()));
      add_system_function(sort_int::mod(sort_int::int_(), sort_pos::pos()));
      add_system_function(sort_real::divides(sort_pos::pos(), sort_pos::pos()));
      add_system_function(sort_real::divides(sort_nat::nat(), sort_nat::nat()));
      add_system_function(sort_real::divides(sort_int::int_(), sort_int::int_()));
      add_system_function(sort_real::divides(sort_real::real_(), sort_real::real_()));
      add_system_function(sort_real::exp(sort_pos::pos(), sort_nat::nat()));
      add_system_function(sort_real::exp(sort_nat::nat(), sort_nat::nat()));
      add_system_function(sort_real::exp(sort_int::int_(), sort_nat::nat()));
      add_system_function(sort_real::exp(sort_real::real_(), sort_int::int_()));
      add_system_function(sort_real::floor());
      add_system_function(sort_real::ceil());
      add_system_function(sort_real::round());
      //Lists
      add_system_constant(sort_list::empty(data::untyped_sort()));
      add_system_function(sort_list::cons_(data::untyped_sort()));
      add_system_function(sort_list::count(data::untyped_sort()));
      add_system_function(sort_list::snoc(data::untyped_sort()));
      add_system_function(sort_list::concat(data::untyped_sort()));
      add_system_function(sort_list::element_at(data::untyped_sort()));
      add_system_function(sort_list::head(data::untyped_sort()));
      add_system_function(sort_list::tail(data::untyped_sort()));
      add_system_function(sort_list::rhead(data::untyped_sort()));
      add_system_function(sort_list::rtail(data::untyped_sort()));
      add_system_function(sort_list::in(data::untyped_sort()));

      //Sets

      add_system_function(sort_bag::set2bag(data::untyped_sort()));
      add_system_function(sort_set::in(data::untyped_sort(), data::untyped_sort(), sort_fset::fset(data::untyped_sort())));
      add_system_function(sort_set::in(data::untyped_sort(), data::untyped_sort(), sort_set::set_(data::untyped_sort())));
      add_system_function(sort_set::union_(data::untyped_sort(), sort_fset::fset(data::untyped_sort()), sort_fset::fset(data::untyped_sort())));
      add_system_function(sort_set::union_(data::untyped_sort(), sort_set::set_(data::untyped_sort()), sort_set::set_(data::untyped_sort())));
      add_system_function(sort_set::difference(data::untyped_sort(), sort_fset::fset(data::untyped_sort()), sort_fset::fset(data::untyped_sort())));
      add_system_function(sort_set::difference(data::untyped_sort(), sort_set::set_(data::untyped_sort()), sort_set::set_(data::untyped_sort())));
      add_system_function(sort_set::intersection(data::untyped_sort(), sort_fset::fset(data::untyped_sort()), sort_fset::fset(data::untyped_sort())));
      add_system_function(sort_set::intersection(data::untyped_sort(), sort_set::set_(data::untyped_sort()), sort_set::set_(data::untyped_sort())));
      add_system_function(sort_set::false_function(data::untyped_sort())); // Needed as it is used within the typechecker.
      add_system_function(sort_set::constructor(data::untyped_sort())); // Needed as it is used within the typechecker.
      //**** add_system_function(sort_bag::set2bag(data::untyped_sort()));
      // add_system_constant(sort_set::empty(data::untyped_sort()));
      // add_system_function(sort_set::in(data::untyped_sort()));
      // add_system_function(sort_set::union_(data::untyped_sort()));
      // add_system_function(sort_set::difference(data::untyped_sort()));
      // add_system_function(sort_set::intersection(data::untyped_sort()));
      add_system_function(sort_set::complement(data::untyped_sort()));

      //FSets
      add_system_constant(sort_fset::empty(data::untyped_sort()));
      // add_system_function(sort_fset::in(data::untyped_sort()));
      // add_system_function(sort_fset::union_(data::untyped_sort()));
      // add_system_function(sort_fset::intersection(data::untyped_sort()));
      // add_system_function(sort_fset::difference(data::untyped_sort()));
      add_system_function(sort_fset::count(data::untyped_sort()));
      add_system_function(sort_fset::insert(data::untyped_sort())); // Needed as it is used within the typechecker.

      //Bags
      add_system_function(sort_bag::bag2set(data::untyped_sort()));
      add_system_function(sort_bag::in(data::untyped_sort(), data::untyped_sort(), sort_fbag::fbag(data::untyped_sort())));
      add_system_function(sort_bag::in(data::untyped_sort(), data::untyped_sort(), sort_bag::bag(data::untyped_sort())));
      add_system_function(sort_bag::union_(data::untyped_sort(), sort_fbag::fbag(data::untyped_sort()), sort_fbag::fbag(data::untyped_sort())));
      add_system_function(sort_bag::union_(data::untyped_sort(), sort_bag::bag(data::untyped_sort()), sort_bag::bag(data::untyped_sort())));
      add_system_function(sort_bag::difference(data::untyped_sort(), sort_fbag::fbag(data::untyped_sort()), sort_fbag::fbag(data::untyped_sort())));
      add_system_function(sort_bag::difference(data::untyped_sort(), sort_bag::bag(data::untyped_sort()), sort_bag::bag(data::untyped_sort())));
      add_system_function(sort_bag::intersection(data::untyped_sort(), sort_fbag::fbag(data::untyped_sort()), sort_fbag::fbag(data::untyped_sort())));
      add_system_function(sort_bag::intersection(data::untyped_sort(), sort_bag::bag(data::untyped_sort()), sort_bag::bag(data::untyped_sort())));
      add_system_function(sort_bag::count(data::untyped_sort(), data::untyped_sort(), sort_fbag::fbag(data::untyped_sort())));
      add_system_function(sort_bag::count(data::untyped_sort(), data::untyped_sort(), sort_bag::bag(data::untyped_sort())));
      // add_system_constant(sort_bag::empty(data::untyped_sort()));
      // add_system_function(sort_bag::in(data::untyped_sort()));
      //**** add_system_function(sort_bag::count(data::untyped_sort()));
      // add_system_function(sort_bag::count(data::untyped_sort(), data::untyped_sort(), sort_fset::fset(data::untyped_sort())));
      //add_system_function(sort_bag::join(data::untyped_sort()));
      // add_system_function(sort_bag::difference(data::untyped_sort()));
      // add_system_function(sort_bag::intersection(data::untyped_sort()));
      add_system_function(sort_bag::zero_function(data::untyped_sort())); // Needed as it is used within the typechecker.
      add_system_function(sort_bag::constructor(data::untyped_sort())); // Needed as it is used within the typechecker.

      //FBags
      add_system_constant(sort_fbag::empty(data::untyped_sort()));
      // add_system_function(sort_fbag::count(data::untyped_sort()));
      // add_system_function(sort_fbag::in(data::untyped_sort()));
      // add_system_function(sort_fbag::union_(data::untyped_sort()));
      // add_system_function(sort_fbag::intersection(data::untyped_sort()));
      // add_system_function(sort_fbag::difference(data::untyped_sort()));
      add_system_function(sort_fbag::count_all(data::untyped_sort()));
      add_system_function(sort_fbag::cinsert(data::untyped_sort())); // Needed as it is used within the typechecker.

      // function update
      add_system_function(data::function_update(data::untyped_sort(),data::untyped_sort()));
    }

    void add_constant(const data::function_symbol& f, const std::string& msg)
    {
      if (m_user_constants.count(f.name()) > 0)
      {
        throw mcrl2::runtime_error("double declaration of " + msg + " " + core::pp(f.name()));
      }
      if (m_system_constants.count(f.name()) > 0 || m_system_functions.count(f.name()) > 0)
      {
        throw mcrl2::runtime_error("attempt to declare a constant with the name that is a built-in identifier (" + core::pp(f.name()) + ")");
      }
      m_user_constants[f.name()] = f.sort();
    }

    void add_function(const data::function_symbol& f, const std::string& msg, bool allow_double_decls = false)
    {
      if (m_system_constants.count(f.name()) > 0)
      {
        throw mcrl2::runtime_error("attempt to redeclare the system constant with a " + msg + " " + data::pp(f));
      }

      if (m_system_functions.count(f.name()) > 0)
      {
        throw mcrl2::runtime_error("attempt to redeclare a system function with a " + msg + " " + data::pp(f));
      }

      auto j = m_user_functions.find(f.name());
      const function_sort& fsort = atermpp::down_cast<function_sort>(f.sort());

      // the table m_user_functions contains a list of types for each
      // function name. We need to check if there is already such a type
      // in the list. If so -- error, otherwise -- add
      if (j != m_user_functions.end())
      {
        auto& sorts = j->second;
        if (find_sort(fsort, sorts))
        {
          if (!allow_double_decls)
          {
            throw mcrl2::runtime_error("double declaration of " + msg + " " + core::pp(f.name()));
          }
        }
      }
      m_user_functions[f.name()] = m_user_functions[f.name()] + function_sort_list({ fsort });
    }

    // Adds constants and functions corresponding to the sort x
    void read_sort(const sort_expression& x)
    {
      if (is_basic_sort(x))
      {
        // This should be checked elsewhere
        // check_basic_sort_is_declared(atermpp::down_cast<basic_sort>(x).name());
      }
      else if (is_container_sort(x))
      {
        read_sort(atermpp::down_cast<container_sort>(x).element_sort());
      }
      else if (is_function_sort(x))
      {
        const function_sort& fs = atermpp::down_cast<function_sort>(x);
        read_sort(fs.codomain());
        for (const sort_expression& sort: fs.domain())
        {
          read_sort(sort);
        }
      }
      else if (is_structured_sort(x))
      {
        const structured_sort& struct_sort = atermpp::down_cast<structured_sort>(x);
        for (const structured_sort_constructor& constructor: struct_sort.constructors())
        {
          // recognizer -- if present -- a function from SortExpr to Bool
          core::identifier_string name = constructor.recogniser();
          if (name != core::empty_identifier_string())
          {
            add_function(data::function_symbol(name, make_function_sort(x, sort_bool::bool_())), "recognizer");
          }

          // constructor type and projections
          structured_sort_constructor_argument_list arguments = constructor.arguments();
          name = constructor.name();
          if (arguments.empty())
          {
            add_constant(data::function_symbol(name, x), "constructor constant");
            continue;
          }

          sort_expression_list sorts;
          for (const structured_sort_constructor_argument& arg: arguments)
          {
            read_sort(arg.sort());
            if (arg.name() != core::empty_identifier_string())
            {
              add_function(function_symbol(arg.name(), function_sort({ x }, arg.sort())), "projection", true);
            }
            sorts.push_front(arg.sort());
          }
          add_function(data::function_symbol(name, function_sort(atermpp::reverse(sorts), x)), "constructor");
        }
      }
      // other sorts can be ignored
    }

    void read_constructors_and_mappings(const function_symbol_vector& constructors, const function_symbol_vector& mappings, const function_symbol_vector& normalized_constructors)
    {
      mCRL2log(log::debug) << "Start Read-in Func" << std::endl;

      std::size_t constr_number=constructors.size();
      function_symbol_vector functions_and_constructors=constructors;
      functions_and_constructors.insert(functions_and_constructors.end(),mappings.begin(),mappings.end());
      for (const function_symbol& f: functions_and_constructors)
      {
        sort_expression fsort = f.sort();

        // This should be checked elsewhere
        // check_sort_is_declared(fsort);

        //if fsort is a defined function sort, unwind it
        if (is_basic_sort(fsort))
        {
          const sort_expression s = unwind_sort_expression(fsort);
          if (is_function_sort(s))
          {
            fsort = s;
          }
        }

        if (is_function_sort(fsort))
        {
          add_function(data::function_symbol(f.name(), fsort), "function");
        }
        else
        {
          try
          {
            add_constant(data::function_symbol(f.name(), fsort), "constant");
          }
          catch (mcrl2::runtime_error& e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not add constant");
          }
        }

        if (constr_number)
        {
          constr_number--;

          //Here checks for the constructors
          sort_expression s = fsort;
          if (is_function_sort(s))
          {
            s = atermpp::down_cast<function_sort>(s).codomain();
          }
          s = unwind_sort_expression(s);
          if (!is_basic_sort(s) ||
              sort_bool::is_bool(sort_expression(s)) ||
              sort_pos::is_pos(sort_expression(s)) ||
              sort_nat::is_nat(sort_expression(s)) ||
              sort_int::is_int(sort_expression(s)) ||
              sort_real::is_real(sort_expression(s))
              )
          {
            throw mcrl2::runtime_error("Could not add constructor " + core::pp(f.name()) + " of sort " + data::pp(fsort) + ". Constructors of built-in sorts are not allowed.");
          }
        }

        mCRL2log(log::debug) << "Read-in Func " << f.name() << ", Types " << fsort << "" << std::endl;
      }

      // Check that the constructors are defined such that they cannot generate an empty sort.
      // E.g. in the specification sort D; cons f:D->D; the sort D must be necessarily empty, which is
      // forbidden. The function below checks whether such malicious specifications occur.
      check_for_empty_constructor_domains(normalized_constructors); // throws exception if not ok.
    }

  public:
    type_checker(const data_specification& data_spec)
      : sort_type_checker(data_spec)
    {
      initialise_system_defined_functions();
      try
      {
        for (const alias& a: get_sort_specification().user_defined_aliases())
        {
          read_sort(a.reference());
        }
        read_constructors_and_mappings(data_spec.user_defined_constructors(),data_spec.user_defined_mappings(),data_spec.constructors());
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ntype checking of data expression failed");
      }
    }

    const std::map<core::identifier_string, sort_expression_list>& system_constants() const
    {
      return m_system_constants;
    }

    const std::map<core::identifier_string, function_sort_list>& system_functions() const
    {
      return m_system_functions;
    }

    const std::map<core::identifier_string, sort_expression>& user_constants() const
    {
      return m_user_constants;
    }

    const std::map<core::identifier_string, function_sort_list>& user_functions() const
    {
      return m_user_functions;
    }
};

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_TYPE_CHECKER_H

