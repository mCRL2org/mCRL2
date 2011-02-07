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
#include "mcrl2/data/variable.h"
#include "mcrl2/core/detail/find_impl.h"
#include "mcrl2/data/detail/find_impl.h"
#include "mcrl2/data/detail/find.h"
#include "mcrl2/data/detail/traverser.h"
#include "mcrl2/data/add_binding.h"
#include "mcrl2/data/traverser.h"

namespace mcrl2 {

namespace data {

namespace detail {

  template <template <class> class Traverser, class OutputIterator>
  struct find_identifiers_traverser: public Traverser<find_identifiers_traverser<Traverser, OutputIterator> >
  {
    typedef Traverser<find_identifiers_traverser<Traverser, OutputIterator> > super; 
    using super::enter;
    using super::leave;
    using super::operator();
  
    OutputIterator out;
  
    find_identifiers_traverser(OutputIterator out_)
      : out(out_)
    {}
  
    void operator()(const identifier& v)
    {
      *out = v;
    }

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
  };

  template <template <class> class Traverser, class OutputIterator>
  find_identifiers_traverser<Traverser, OutputIterator>
  make_find_identifiers_traverser(OutputIterator out)
  {
    return find_identifiers_traverser<Traverser, OutputIterator>(out);
  }

  template <template <class> class Traverser, class OutputIterator>
  struct find_variables_traverser: public Traverser<find_variables_traverser<Traverser, OutputIterator> >
  {
    typedef Traverser<find_variables_traverser<Traverser, OutputIterator> > super; 
    using super::enter;
    using super::leave;
    using super::operator();
  
    OutputIterator out;
  
    find_variables_traverser(OutputIterator out_)
      : out(out_)
    {}
  
    void operator()(const variable& v)
    {
      *out = v;
    }

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
  };

  template <template <class> class Traverser, class OutputIterator>
  find_variables_traverser<Traverser, OutputIterator>
  make_find_variables_traverser(OutputIterator out)
  {
    return find_variables_traverser<Traverser, OutputIterator>(out);
  } 

  template <template <class> class Traverser, template <template <class> class, class> class Binder, class OutputIterator>
  struct find_free_variables_traverser: public Binder<Traverser, find_free_variables_traverser<Traverser, Binder, OutputIterator> >
  {
    typedef Binder<Traverser, find_free_variables_traverser<Traverser, Binder, OutputIterator> > super; 
    using super::enter;
    using super::leave;
    using super::operator();
    using super::is_bound;
    using super::increase_bind_count;
  
    OutputIterator out;
  
    find_free_variables_traverser(OutputIterator out_)
      : out(out_)
    {}

    template <typename VariableContainer>
    find_free_variables_traverser(OutputIterator out_, const VariableContainer& v)
      : out(out_)
    {
      increase_bind_count(v);
    } 
  
    void operator()(const variable& v)
    {
      if (!is_bound(v))
      {
        *out = v;
      }
    }

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
  };
  
  template <template <class> class Traverser, template <template <class> class, class> class Binder, class OutputIterator>
  find_free_variables_traverser<Traverser, Binder, OutputIterator>
  make_find_free_variables_traverser(OutputIterator out)
  {
    return find_free_variables_traverser<Traverser, Binder, OutputIterator>(out);
  }

  template <template <class> class Traverser, template <template <class> class, class> class Binder, class OutputIterator, class VariableContainer>
  find_free_variables_traverser<Traverser, Binder, OutputIterator>
  make_find_free_variables_traverser(OutputIterator out, const VariableContainer& v)
  {
    return find_free_variables_traverser<Traverser, Binder, OutputIterator>(out, v);
  }

} // namespace detail

//--- start generated data find code ---//
#ifdef MCRL2_NEW_FIND_VARIABLES
  /// \brief Returns all variables that occur in an object
  /// \param[in] x an object containing variables
  /// \param[in,out] o an output iterator to which all variables occurring in x are written.
  /// \return All variables that occur in the term x
  template <typename T, typename OutputIterator>
  void find_variables(const T& x, OutputIterator o)
  {
    data::detail::make_find_variables_traverser<data::traverser>(o)(x);
  }

  /// \brief Returns all variables that occur in an object
  /// \param[in] x an object containing variables
  /// \return All variables that occur in the object x
  template <typename T>
  std::set<data::variable> find_variables(const T& x)
  {
    std::set<data::variable> result;
    data::find_variables(x, std::inserter(result, result.end()));
    return result;
  }

  /// \brief Returns all variables that occur in an object
  /// \param[in] x an object containing variables
  /// \param[in,out] o an output iterator to which all variables occurring in x are added.
  /// \return All free variables that occur in the object x
  template <typename T, typename OutputIterator>
  void find_free_variables(const T& x, OutputIterator o)
  {
    data::detail::make_find_free_variables_traverser<data::variable_traverser, data::add_data_variable_binding>(o)(x);
  }

  /// \brief Returns all variables that occur in an object
  /// \param[in] x an object containing variables
  /// \param[in,out] o an output iterator to which all variables occurring in x are written.
  /// \param[in] bound a container of variables
  /// \return All free variables that occur in the object x
  template <typename T, typename OutputIterator, typename VariableContainer>
  void find_free_variables_with_bound(const T& x, OutputIterator o, const VariableContainer& bound)
  {
    data::detail::make_find_free_variables_traverser<data::variable_traverser, data::add_data_variable_binding>(o, bound)(x);
  }

  /// \brief Returns all variables that occur in an object
  /// \param[in] x an object containing variables
  /// \return All free variables that occur in the object x
  template <typename T>
  std::set<data::variable> find_free_variables(const T& x)
  {
    std::set<data::variable> result;
    data::find_free_variables(x, std::inserter(result, result.end()));
    return result;
  }

  /// \brief Returns all variables that occur in an object
  /// \param[in] x an object containing variables
  /// \param[in] bound a bound a container of variables
  /// \return All free variables that occur in the object x
  template <typename T, typename VariableContainer>
  std::set<data::variable> find_free_variables_with_bound(const T& x, VariableContainer const& bound)
  {
    std::set<data::variable> result;
    data::find_free_variables_with_bound(x, std::inserter(result, result.end()), bound);
    return result;
  }

  /// \brief Returns all identifiers that occur in an object
  /// \param[in] x an object containing identifiers
  /// \param[in,out] o an output iterator to which all identifiers occurring in x are written.
  /// \return All identifiers that occur in the term x
  template <typename T, typename OutputIterator>
  void find_identifiers(const T& x, OutputIterator o)
  {
    data::detail::make_find_identifiers_traverser<data::traverser>(o)(x);
  }
  
  /// \brief Returns all identifiers that occur in an object
  /// \param[in] x an object containing identifiers
  /// \return All identifiers that occur in the object x
  template <typename T>
  std::set<core::identifier> find_identifiers(const T& x)
  {
    std::set<core::identifier> result;
    data::find_identifiers(x, std::inserter(result, result.end()), bound);
    return result;
  }
 
#endif // MCRL2_NEW_FIND_VARIABLES
//--- end generated data find code ---//

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \param[in,out] o an output iterator to which all data variables occurring in t
///             are added.
/// \return All data variables that occur in the term t
template <typename Container, typename OutputIterator>
void find_variables(Container const& container, OutputIterator o)
{
  core::detail::make_find_helper<variable, detail::traverser>(o)(container);
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \return All data variables that occur in the term t
template <typename Container>
std::set<variable> find_variables(Container const& container)
{
  std::set<variable> result;
  find_variables(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given variable as subterm.
/// \param[in] container an expression or container with expressions
/// \param[in] v an expression or container with expressions
/// \param d A variable
/// \return True if the term has a given variable as subterm.
template <typename Container>
bool search_variable(Container const& container, const variable& v)
{
  return core::detail::make_search_helper<variable, detail::selective_data_traverser>(detail::compare_variable(v)).apply(container);
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \param[in,out] o an output iterator to which all data variables occurring in t
///             are added.
/// \return All data variables that occur in the term t
template <typename Container, typename OutputIterator>
void find_free_variables(Container const& container, OutputIterator o,
		           typename atermpp::detail::disable_if_container<OutputIterator>::type* = 0)
{
  detail::make_free_variable_find_helper<detail::binding_aware_traverser>(o)(container);
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \param[in,out] o an output iterator to which all data variables occurring in t
///             are added.
/// \param[in] bound a set of variables that should be considered as bound
/// \return All data variables that occur in the term t
/// TODO prevent copy of Sequence
template <typename Container, typename OutputIterator, typename Sequence>
void find_free_variables_with_bound(Container const& container, OutputIterator o, Sequence const& bound)
{
  detail::make_free_variable_find_helper<detail::binding_aware_traverser>(bound, o)(container);
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \return All data variables that occur in the term t
template <typename Container>
std::set<variable> find_free_variables(Container const& container)
{
  std::set<variable> result;
  find_free_variables(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \param[in] bound a set of variables that should be considered as bound
/// \return All data variables that occur in the term t
/// TODO prevent copy of Sequence
template <typename Container, typename Sequence>
std::set<variable> find_free_variables_with_bound(Container const& container, Sequence const& bound,
                                        typename atermpp::detail::enable_if_container<Sequence, variable>::type* = 0)
{
  std::set<variable> result;
  find_free_variables_with_bound(container, std::inserter(result, result.end()), bound);
  return result;
}

/// \brief Returns true if the term has a given variable as subterm.
/// \param[in] container an expression or container with expressions
/// \param d A data variable
/// \return True if the term has a given variable as subterm.
template <typename Container>
bool search_free_variable(Container container, const variable& d)
{
  return detail::make_free_variable_search_helper<detail::selective_binding_aware_traverser>(detail::compare_variable(d)).apply(container);
}


/// \brief Returns true if the term has a given sort expression as subterm.
/// \param[in] container an expression or container of expressions
/// \param[in] s A sort expression
/// \return True if the term has a given sort expression as subterm.
template <typename Container>
bool search_sort_expression(Container const& container, const sort_expression& s)
{
  return core::detail::make_search_helper<sort_expression, detail::selective_sort_traverser>(detail::compare_sort(s)).apply(container);
}

/// \brief Returns all sort expressions that occur in the term t
/// \param[in] container an expression or container of expressions
/// \param[in] o an output iterator
/// \return All sort expressions that occur in the term t
template <typename Container, typename OutputIterator>
void find_sort_expressions(Container const& container, OutputIterator o)
{
  core::detail::make_find_helper<sort_expression, detail::sort_traverser>(o)(container);
}

/// \brief Returns all sort expressions that occur in the term t
/// \param[in] container an expression or container of expressions
/// \return All sort expressions that occur in the term t
template <typename Container>
std::set<sort_expression> find_sort_expressions(Container const& container)
{
  std::set<sort_expression> result;
  find_sort_expressions(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given identifier as subterm.
/// \param[in] container an expression or container of expressions
/// \param[in] s An identifier
/// \return True if the term has a given sort identifier as subterm.
template <typename Container>
bool search_identifiers(Container const& container, const core::identifier_string& s)
{
  return core::detail::make_search_helper<core::identifier_string, detail::selective_sort_traverser>(boost::bind(std::equal_to<core::identifier_string>(), s, _1)).apply(container);
}

/// \brief Returns all identifiers that occur in the term t
/// \param[in] container an expression or container of expressions
/// \param[out] o an output iterator
/// \return All sort identifiers that occur in the term t
template <typename Container, typename OutputIterator>
void find_identifiers(Container const& container, OutputIterator o)
{
  return core::detail::make_find_helper<core::identifier_string, detail::sort_traverser>(o)(container);
}

/// \brief Returns all basic sorts that occur in the term t
/// \param[in] container an expression or container of expressions
/// \param[in] o an output iterator
/// \return All sort expressions that occur in the term t
template <typename Container>
std::set<core::identifier_string> find_identifiers(Container const& container)
{
  std::set<core::identifier_string> result;
  find_identifiers(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given data expression as subterm.
/// \param[in] container an expression or container of expressions
/// \param[in] s A data expression
/// \return True if the term has a given data expression as subterm.
template <typename Container>
bool search_data_expression(Container const& container, const data_expression& s)
{
  return core::detail::make_search_helper<data_expression, detail::selective_data_traverser>(detail::compare_term<data_expression>(s)).apply(container);
}

/// \brief Returns all data expressions that occur in the term t
/// \param[in] container an expression or container of expressions
/// \return All data expressions that occur in the term t
template <typename Container, typename OutputIterator>
void find_data_expressions(Container const& container, OutputIterator o)
{
  core::detail::make_find_helper<data_expression, detail::traverser>(o)(container);
}

/// \brief Returns all data expressions that occur in the term t
/// \param[in] container an expression or container of expressions
/// \return All data expressions that occur in the term t
template <typename Container>
std::set<data_expression> find_data_expressions(Container const& container)
{
  std::set<data_expression> result;
  find_data_expressions(container, std::inserter(result, result.end()));
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
    std::set<variable> variables = data::find_variables(t);

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
