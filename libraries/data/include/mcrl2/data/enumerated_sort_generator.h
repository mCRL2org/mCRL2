// Author(s): Jan Friso Groote, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/enumerated_sort_generator.h
/// \brief Generates enumerated sorts with a given number of elements, and case functions on them.

#ifndef MCRL2_DATA_ENUMERATED_SORT_GENERATOR_H
#define MCRL2_DATA_ENUMERATED_SORT_GENERATOR_H

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/standard.h"

namespace mcrl2::data
{

/// \brief Generates enumerated sorts with a given number of elements, and case functions on them, and adds them to a
///        data specification.
/// \details The enumerated sort with two elements is Bool, with elements false and true. The enumerated sort with
///          n != 2 elements is a fresh sort Enumn, declared as an alias of the structured sort
///          struct e(n-1)_n | ... | e0_n. For each n only one enumerated sort is generated.
///
///          A case function on an enumerated sort E with elements e_1, ..., e_n (in the order of elements(n)) and a
///          sort s has sort E # s # ... # s -> s, with n arguments of sort s, and selects the argument that
///          corresponds to the element: C(e_i, y_1, ..., y_n) = y_i. Furthermore, C(e, x, ..., x) = x. For Bool the
///          case function is if, which selects the first argument for true, so the second element. For each
///          enumerated sort and sort s only one case function is generated.
///
///          Fresh names are taken from an identifier generator that is passed to the constructor, so that the caller
///          can make sure that they do not clash with other names.
class enumerated_sort_generator
{
protected:
  /// \brief An enumerated sort with its elements and the case functions on it, indexed by the sort of the
  ///        alternatives.
  struct enumerated_sort
  {
    sort_expression sort;
    data_expression_list elements;
    std::map<sort_expression, function_symbol> case_functions;
  };

  /// \brief The data specification to which the sorts, case functions and equations are added.
  data_specification& m_specification;

  /// \brief The generator for fresh names of sorts, constructors, case functions and variables.
  set_identifier_generator& m_identifier_generator;

  /// \brief The enumerated sorts generated so far, indexed by their number of elements.
  std::map<std::size_t, enumerated_sort> m_enumerated_sorts;

  /// \brief Returns the enumerated sort with n elements; it is generated if needed.
  enumerated_sort& get(const std::size_t n)
  {
    std::map<std::size_t, enumerated_sort>::iterator i = m_enumerated_sorts.find(n);
    if (i != m_enumerated_sorts.end())
    {
      return i->second;
    }

    enumerated_sort& result = m_enumerated_sorts[n];
    if (n == 2)
    {
      result.sort = sort_bool::bool_();
      result.elements = data_expression_list({sort_bool::false_(), sort_bool::true_()});
      return result;
    }

    const basic_sort sort_name(m_identifier_generator("Enum" + std::to_string(n)));
    structured_sort_constructor_list constructors;
    for (std::size_t j = 0; j < n; ++j)
    {
      constructors.push_front(
        structured_sort_constructor(m_identifier_generator("e" + std::to_string(j) + "_" + std::to_string(n)), ""));
    }
    const structured_sort sort_structure(constructors);
    m_specification.add_alias(alias(sort_name, sort_structure));

    // The constructors are taken in the order of the structured sort, and not from
    // m_specification.constructors(sort_name): the order in which the data specification returns them can
    // differ between runs, as it collects them in a std::set ordered on the addresses of terms.
    function_symbol_vector elements;
    for (const structured_sort_constructor& c: sort_structure.constructors())
    {
      elements.push_back(c.constructor_function(sort_name));
    }
    assert(elements.size() == n);
    assert(std::is_permutation(elements.begin(),
      elements.end(),
      m_specification.constructors(sort_name).begin(),
      m_specification.constructors(sort_name).end()));
    result.sort = sort_name;
    result.elements = data_expression_list(elements.begin(), elements.end());
    return result;
  }

  /// \brief Adds the equations of the case function f on the enumerated sort e and the sort s.
  void add_case_function_equations(const enumerated_sort& e, const function_symbol& f, const sort_expression& s)
  {
    const variable x(m_identifier_generator("x"), s);
    variable_list ys;
    data_expression_list xs;
    for (std::size_t j = 0; j < e.elements.size(); ++j)
    {
      ys.push_front(variable(m_identifier_generator("y"), s));
      xs.push_front(x);
    }

    // C(e, x, ..., x) = x
    const variable v(m_identifier_generator("e"), e.sort);
    xs.push_front(v);
    m_specification.add_equation(data_equation(variable_list({x, v}), application(f, xs), x));

    // C(e_i, y_1, ..., y_n) = y_i: the i-th element selects the i-th argument.
    data_expression_list arguments(ys.begin(), ys.end());
    variable_list::const_iterator y = ys.begin();
    for (const data_expression& element: e.elements)
    {
      arguments.push_front(element);
      m_specification.add_equation(data_equation(ys, application(f, arguments), *y));
      arguments.pop_front();
      ++y;
    }
  }

public:
  /// \brief Constructor.
  /// \param specification The data specification to which the sorts, case functions and equations are added.
  /// \param identifier_generator The generator for fresh names.
  enumerated_sort_generator(data_specification& specification, set_identifier_generator& identifier_generator)
    : m_specification(specification),
      m_identifier_generator(identifier_generator)
  {}

  /// \brief Returns the enumerated sort with n elements. It is generated if needed.
  const sort_expression& operator()(const std::size_t n)
  {
    return get(n).sort;
  }

  /// \brief Returns the elements of the enumerated sort with n elements. The sort is generated if needed.
  const data_expression_list& elements(const std::size_t n)
  {
    return get(n).elements;
  }

  /// \brief Returns the case function on the enumerated sort with n elements and the sort s. The sort, the case
  ///        function and its equations are generated if needed.
  const function_symbol& case_function(const std::size_t n, const sort_expression& s)
  {
    enumerated_sort& e = get(n);
    std::map<sort_expression, function_symbol>::const_iterator i = e.case_functions.find(s);
    if (i != e.case_functions.end())
    {
      return i->second;
    }

    if (e.sort == sort_bool::bool_())
    {
      return e.case_functions[s] = if_(s);
    }

    sort_expression_list domain;
    for (std::size_t j = 0; j < n; ++j)
    {
      domain.push_front(s);
    }
    domain.push_front(e.sort);
    const function_symbol f(m_identifier_generator("C" + std::to_string(n) + "_"), function_sort(domain, s));
    m_specification.add_mapping(f);
    add_case_function_equations(e, f, s);
    return e.case_functions[s] = f;
  }
};

} // namespace mcrl2::data

#endif // MCRL2_DATA_ENUMERATED_SORT_GENERATOR_H
