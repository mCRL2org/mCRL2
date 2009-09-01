// Author(s): Wieger Wesselink, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/find.h
/// \brief Search functions of the data library.

#ifndef MCRL2_DATA_FIND_H
#define MCRL2_DATA_FIND_H

#include <functional>
#include <iterator>
#include <functional>
#include <set>
#include "boost/bind.hpp"
#include "boost/utility/enable_if.hpp"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/detail/find.h"

namespace mcrl2 {

namespace data {

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \param[in,out] o an output iterator to which all data variables occurring in t
///             are added.
/// \return All data variables that occur in the term t
template < typename Container, typename OutputIterator >
void find_variables(Container const& container, OutputIterator o)
{
  detail::make_find_helper< variable, detail::traverser >(o)(container);
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \return All data variables that occur in the term t
template < typename Container >
std::set< variable > find_variables(Container const& container)
{
  std::set< variable > result;
  find_variables(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given variable as subterm.
/// \param[in] container an expression or container with expressions
/// \param[in] v an expression or container with expressions
/// \param d A variable
/// \return True if the term has a given variable as subterm.
template < typename Container >
bool search_variable(Container const& container, const variable& v)
{
  return detail::make_search_helper< variable, detail::selective_data_traverser >(detail::compare_variable(v)).apply(container);
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \param[in,out] o an output iterator to which all data variables occurring in t
///             are added.
/// \return All data variables that occur in the term t
template < typename Container, typename OutputIterator >
void find_free_variables(Container const& container, OutputIterator o,
		           typename detail::disable_if_container< OutputIterator >::type* = 0)
{
  detail::make_free_variable_find_helper< detail::binding_aware_traverser >(o)(container);
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \param[in,out] o an output iterator to which all data variables occurring in t
///             are added.
/// \param[in] bound a set of variables that should be considered as bound
/// \return All data variables that occur in the term t
/// TODO prevent copy of Sequence
template < typename Container, typename OutputIterator, typename Sequence >
void find_free_variables(Container const& container, OutputIterator o, Sequence const& bound)
{
  detail::make_free_variable_find_helper< detail::binding_aware_traverser >(bound, o)(container);
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \return All data variables that occur in the term t
template < typename Container >
std::set< variable > find_free_variables(Container const& container)
{
  std::set< variable > result;
  find_free_variables(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \param[in] bound a set of variables that should be considered as bound
/// \return All data variables that occur in the term t
/// TODO prevent copy of Sequence
template < typename Container, typename Sequence >
std::set< variable > find_free_variables(Container const& container, Sequence const& bound,
                                        typename detail::enable_if_container< Sequence, variable >::type* = 0)
{
  std::set< variable > result;
  find_free_variables(container, std::inserter(result, result.end()), bound);
  return result;
}

/// \brief Returns true if the term has a given variable as subterm.
/// \param[in] container an expression or container with expressions
/// \param d A data variable
/// \return True if the term has a given variable as subterm.
template < typename Container >
bool search_free_variable(Container container, const variable& d)
{
  return detail::make_free_variable_search_helper< detail::selective_binding_aware_traverser >(detail::compare_variable(d)).apply(container);
}


/// \brief Returns true if the term has a given sort expression as subterm.
/// \param[in] container an expression or container of expressions
/// \param[in] s A sort expression
/// \return True if the term has a given sort expression as subterm.
template < typename Container >
bool search_sort_expression(Container const& container, const sort_expression& s)
{
  return detail::make_search_helper< sort_expression, detail::selective_sort_traverser >(detail::compare_sort(s)).apply(container);
}

/// \brief Returns all sort expressions that occur in the term t
/// \param[in] container an expression or container of expressions
/// \param[in] o an output iterator
/// \return All sort expressions that occur in the term t
template < typename Container, typename OutputIterator >
void find_sort_expressions(Container const& container, OutputIterator o)
{
  detail::make_find_helper< sort_expression, detail::sort_traverser >(o)(container);
}

/// \brief Returns all sort expressions that occur in the term t
/// \param[in] container an expression or container of expressions
/// \return All sort expressions that occur in the term t
template < typename Container >
std::set<sort_expression> find_sort_expressions(Container const& container)
{
  std::set<sort_expression> result;
  find_sort_expressions(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given sort identifier as subterm.
/// \param[in] container an expression or container of expressions
/// \param[in] s A sort identifier
/// \return True if the term has a given sort identifier as subterm.
template < typename Container >
bool search_basic_sort(Container const& container, const basic_sort& s)
{
  return detail::make_search_helper< basic_sort, detail::selective_sort_traverser >(detail::compare_sort(s)).apply(container);
}

/// \brief Returns all sort identifiers that occur in the term t
/// \param[in] container an expression or container of expressions
/// \param[out] o an output iterator
/// \return All sort identifiers that occur in the term t
template < typename Container, typename OutputIterator >
void find_basic_sorts(Container const& container, OutputIterator o)
{
  return detail::make_find_helper< basic_sort, detail::sort_traverser >(o)(container);
}

/// \brief Returns all basic sorts that occur in the term t
/// \param[in] container an expression or container of expressions
/// \param[in] o an output iterator
/// \return All sort expressions that occur in the term t
template < typename Container >
std::set<basic_sort> find_basic_sorts(Container const& container)
{
  std::set<basic_sort> result;
  find_basic_sorts(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given identifier as subterm.
/// \param[in] container an expression or container of expressions
/// \param[in] s An identifier
/// \return True if the term has a given sort identifier as subterm.
template < typename Container >
bool search_identifiers(Container const& container, const core::identifier_string& s)
{
  return detail::make_search_helper< core::identifier_string, detail::selective_sort_traverser >(boost::bind(std::equal_to< core::identifier_string >(), s, _1)).apply(container);
}

/// \brief Returns all identifiers that occur in the term t
/// \param[in] container an expression or container of expressions
/// \param[out] o an output iterator
/// \return All sort identifiers that occur in the term t
template < typename Container, typename OutputIterator >
void find_identifiers(Container const& container, OutputIterator o)
{
  return detail::make_find_helper< core::identifier_string, detail::sort_traverser >(o)(container);
}

/// \brief Returns all basic sorts that occur in the term t
/// \param[in] container an expression or container of expressions
/// \param[in] o an output iterator
/// \return All sort expressions that occur in the term t
template < typename Container >
std::set< core::identifier_string > find_identifiers(Container const& container)
{
  std::set< core::identifier_string > result;
  find_identifiers(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given data expression as subterm.
/// \param[in] container an expression or container of expressions
/// \param[in] s A data expression
/// \return True if the term has a given data expression as subterm.
template <typename Container >
bool search_data_expression(Container const& container, const data_expression& s)
{
  return detail::make_search_helper< data_expression, detail::selective_data_traverser >(detail::compare_term< data_expression >(s)).apply(container);
}

/// \brief Returns all data expressions that occur in the term t
/// \param[in] container an expression or container of expressions
/// \return All data expressions that occur in the term t
template < typename Container, typename OutputIterator >
void find_data_expressions(Container const& container, OutputIterator o)
{
  detail::make_find_helper< data_expression, detail::traverser >(o)(container);
}

/// \brief Returns all data expressions that occur in the term t
/// \param[in] container an expression or container of expressions
/// \return All data expressions that occur in the term t
template < typename Container >
std::set< data_expression > find_data_expressions(Container const& container)
{
  std::set<data_expression> result;
  find_data_expressions(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Finds a mapping in a data specification.
/// \param data A data specification
/// \param s A string
/// \return The found mapping
inline
function_symbol find_mapping(data_specification const& data, std::string const& s)
{
  data_specification::mappings_const_range r(data.mappings());

  data_specification::mappings_const_range::const_iterator i = std::find_if(r.begin(), r.end(), detail::function_symbol_has_name(s));
  return (i == r.end()) ? function_symbol() : *i;
}

/// \brief Finds a constructor in a data specification.
/// \param data A data specification
/// \param s A string
/// \return The found constructor
inline
function_symbol find_constructor(data_specification const& data, std::string const& s)
{
  data_specification::constructors_const_range r(data.constructors());
  data_specification::constructors_const_range::const_iterator i = std::find_if(r.begin(), r.end(), detail::function_symbol_has_name(s));
  return (i == r.end()) ? function_symbol() : *i;
}

/// \brief Finds a sort in a data specification.
/// \param data A data specification
/// \param s A string
/// \return The found sort
inline
sort_expression find_sort(data_specification const& data, std::string const& s)
{
  data_specification::sorts_const_range r(data.sorts());
  data_specification::sorts_const_range::const_iterator i = std::find_if(r.begin(), r.end(), detail::sort_has_name(s));
  return (i == r.end()) ? sort_expression() : *i;
}

/// \brief Gets all equations with a data expression as head
/// on one of its sides.
///
/// \param[in] specification A data specification.
/// \param[in] d A data expression.
/// \return All equations with d as head in one of its sides.
inline
data_equation_vector find_equations(data_specification const& specification, const data_expression& d)
{
  data_equation_vector result;
  data_specification::equations_const_range equations(specification.equations());
  for (data_specification::equations_const_range::const_iterator i = equations.begin(); i != equations.end(); ++i)
  {
    if (i->lhs() == d || i->rhs() == d)
    {
      result.push_back(*i);
    }
    else if(i->lhs().is_application())
    {
      if(static_cast<application>(i->lhs()).head() == d)
      {
        result.push_back(*i);
      }
    }
    else if (i->rhs().is_application())
    {
      if(static_cast<application>(i->rhs()).head() == d)
      {
        result.push_back(*i);
      }
    }
  }
  return result;
}

/// \cond INTERNAL_DOCS
namespace detail {

  /// \brief Returns all names of data variables that occur in the term t
  /// \param t A term
  /// \return All names of data variables that occur in the term t
  template <typename Term>
  std::set<core::identifier_string> find_variable_names(Term t)
  {
    // find all data variables in t
    std::set<variable> variables(find_variables(t));

    std::set<core::identifier_string> result;
    for (std::set<variable>::iterator j = variables.begin(); j != variables.end(); ++j)
    {
      result.insert(j->name());
    }
    return result;
  }

/// \brief Returns all names of data variables that occur in the term t
/// \param t A term
/// \return All names of data variables that occur in the term t
template <typename Term>
std::set<std::string> find_variable_name_strings(Term t)
{
  // find all data variables in t
  std::set<variable> variables(find_variables(t));

  std::set<std::string> result;
  for (std::set<variable>::iterator j = variables.begin(); j != variables.end(); ++j)
  {
    result.insert(j->name());
  }
  return result;
}

} // namespace detail
/// \endcond


} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FIND_H
