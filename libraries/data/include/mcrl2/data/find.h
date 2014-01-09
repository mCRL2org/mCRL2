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
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/add_binding.h"
#include "mcrl2/data/traverser.h"

namespace mcrl2
{

namespace data
{

namespace detail
{
/// \cond INTERNAL_DOCS
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

  void operator()(const core::identifier_string& v)
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
struct find_function_symbols_traverser: public Traverser<find_function_symbols_traverser<Traverser, OutputIterator> >
{
  typedef Traverser<find_function_symbols_traverser<Traverser, OutputIterator> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  OutputIterator out;

  find_function_symbols_traverser(OutputIterator out_)
    : out(out_)
  {}

  void operator()(const function_symbol& v)
  {
    *out = v;
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

template <template <class> class Traverser, class OutputIterator>
find_function_symbols_traverser<Traverser, OutputIterator>
make_find_function_symbols_traverser(OutputIterator out)
{
  return find_function_symbols_traverser<Traverser, OutputIterator>(out);
}

template <template <class> class Traverser, class OutputIterator>
struct find_sort_expressions_traverser: public Traverser<find_sort_expressions_traverser<Traverser, OutputIterator> >
{
  typedef Traverser<find_sort_expressions_traverser<Traverser, OutputIterator> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  OutputIterator out;

  find_sort_expressions_traverser(OutputIterator out_)
    : out(out_)
  {}

  void operator()(const data::sort_expression& v)
  {
    *out = v;

    // also traverse sub-expressions!
    super::operator()(v);
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

template <template <class> class Traverser, class OutputIterator>
find_sort_expressions_traverser<Traverser, OutputIterator>
make_find_sort_expressions_traverser(OutputIterator out)
{
  return find_sort_expressions_traverser<Traverser, OutputIterator>(out);
}

template <template <class> class Traverser, class OutputIterator>
struct find_data_expressions_traverser: public Traverser<find_data_expressions_traverser<Traverser, OutputIterator> >
{
  typedef Traverser<find_data_expressions_traverser<Traverser, OutputIterator> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  OutputIterator out;

  find_data_expressions_traverser(OutputIterator out_)
    : out(out_)
  {}

  void operator()(const data::data_expression& v)
  {
    *out = v;

    // also traverse sub-expressions!
    super::operator()(v);
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

template <template <class> class Traverser, class OutputIterator>
find_data_expressions_traverser<Traverser, OutputIterator>
make_find_data_expressions_traverser(OutputIterator out)
{
  return find_data_expressions_traverser<Traverser, OutputIterator>(out);
}

template <template <class> class Traverser, class OutputIterator>
struct find_all_variables_traverser: public Traverser<find_all_variables_traverser<Traverser, OutputIterator> >
{
  typedef Traverser<find_all_variables_traverser<Traverser, OutputIterator> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  OutputIterator out;

  find_all_variables_traverser(OutputIterator out_)
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
find_all_variables_traverser<Traverser, OutputIterator>
make_find_all_variables_traverser(OutputIterator out)
{
  return find_all_variables_traverser<Traverser, OutputIterator>(out);
}
template <template <class> class Traverser, template <template <class> class, class> class Binder, class OutputIterator>
struct find_free_variables_traverser: public Binder<Traverser, find_free_variables_traverser<Traverser, Binder, OutputIterator> >
{
  typedef Binder<Traverser, find_free_variables_traverser<Traverser, Binder, OutputIterator> > super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::is_bound;
  using super::bound_variables;
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

template <template <class> class Traverser>
struct search_variable_traverser: public Traverser<search_variable_traverser<Traverser> >
{
  typedef Traverser<search_variable_traverser<Traverser> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  bool found;
  const variable& v;

  search_variable_traverser(const variable& v_)
    : found(false), v(v_)
  {}

  void operator()(const variable& x)
  {
    if (x == v)
    {
      found = true;
    }
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

template <template <class> class Traverser>
search_variable_traverser<Traverser>
make_search_variable_traverser(const data::variable& v)
{
  return search_variable_traverser<Traverser>(v);
}

template <template <class> class Traverser, template <template <class> class, class> class Binder>
struct search_free_variable_traverser: public Binder<Traverser, search_free_variable_traverser<Traverser, Binder> >
{
  typedef Binder<Traverser, search_free_variable_traverser<Traverser, Binder> > super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::is_bound;
  using super::bound_variables;
  using super::increase_bind_count;

  bool found;
  const data::variable& v;

  search_free_variable_traverser(const data::variable& v_)
    : found(false), v(v_)
  {}

  void operator()(const variable& x)
  {
    if (v == x && !is_bound(x))
    {
      found = true;
    }
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

template <template <class> class Traverser, template <template <class> class, class> class Binder>
search_free_variable_traverser<Traverser, Binder>
make_search_free_variable_traverser(const data::variable& v)
{
  return search_free_variable_traverser<Traverser, Binder>(v);
}
/// \endcond

} // namespace detail

//--- start generated data find code ---//
/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are written.
/// \return All variables that occur in the term x
template <typename T, typename OutputIterator>
void find_all_variables(const T& x, OutputIterator o)
{
  data::detail::make_find_all_variables_traverser<data::variable_traverser>(o)(x);
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \return All variables that occur in the object x
template <typename T>
std::set<data::variable> find_all_variables(const T& x)
{
  std::set<data::variable> result;
  data::find_all_variables(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are added.
/// \return All free variables that occur in the object x
template <typename T, typename OutputIterator>
void find_free_variables(const T& x, OutputIterator o)
{
  data::detail::make_find_free_variables_traverser<data::data_expression_traverser, data::add_data_variable_binding>(o)(x);
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are written.
/// \param[in] bound a container of variables
/// \return All free variables that occur in the object x
template <typename T, typename OutputIterator, typename VariableContainer>
void find_free_variables_with_bound(const T& x, OutputIterator o, const VariableContainer& bound)
{
  data::detail::make_find_free_variables_traverser<data::data_expression_traverser, data::add_data_variable_binding>(o, bound)(x);
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
  data::detail::make_find_identifiers_traverser<data::identifier_string_traverser>(o)(x);
}

/// \brief Returns all identifiers that occur in an object
/// \param[in] x an object containing identifiers
/// \return All identifiers that occur in the object x
template <typename T>
std::set<core::identifier_string> find_identifiers(const T& x)
{
  std::set<core::identifier_string> result;
  data::find_identifiers(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all sort expressions that occur in an object
/// \param[in] x an object containing sort expressions
/// \param[in,out] o an output iterator to which all sort expressions occurring in x are written.
/// \return All sort expressions that occur in the term x
template <typename T, typename OutputIterator>
void find_sort_expressions(const T& x, OutputIterator o)
{
  data::detail::make_find_sort_expressions_traverser<data::sort_expression_traverser>(o)(x);
}

/// \brief Returns all sort expressions that occur in an object
/// \param[in] x an object containing sort expressions
/// \return All sort expressions that occur in the object x
template <typename T>
std::set<data::sort_expression> find_sort_expressions(const T& x)
{
  std::set<data::sort_expression> result;
  data::find_sort_expressions(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all function symbols that occur in an object
/// \param[in] x an object containing function symbols
/// \param[in,out] o an output iterator to which all function symbols occurring in x are written.
/// \return All function symbols that occur in the term x
template <typename T, typename OutputIterator>
void find_function_symbols(const T& x, OutputIterator o)
{
  data::detail::make_find_function_symbols_traverser<data::data_expression_traverser>(o)(x);
}

/// \brief Returns all function symbols that occur in an object
/// \param[in] x an object containing function symbols
/// \return All function symbols that occur in the object x
template <typename T>
std::set<data::function_symbol> find_function_symbols(const T& x)
{
  std::set<data::function_symbol> result;
  data::find_function_symbols(x, std::inserter(result, result.end()));
  return result;
}
//--- end generated data find code ---//

/// \brief Returns all data expressions that occur in an object
/// \param[in] x an object containing data expressions
/// \param[in,out] o an output iterator to which all data expressions occurring in x are written.
/// \return All data expressions that occur in the term x
template <typename T, typename OutputIterator>
void find_data_expressions(const T& x, OutputIterator o)
{
  data::detail::make_find_data_expressions_traverser<data::data_expression_traverser>(o)(x);
}

/// \brief Returns all data expressions that occur in an object
/// \param[in] x an object containing data expressions
/// \return All data expressions that occur in the object x
template <typename T>
std::set<data::data_expression> find_data_expressions(const T& x)
{
  std::set<data::data_expression> result;
  data::find_data_expressions(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given variable as subterm.
/// \param[in] x an expression
/// \param[in] v a variable
/// \return True if v occurs in x.
template <typename T>
bool search_variable(const T& x, const variable& v)
{
  data::detail::search_variable_traverser<data::variable_traverser> f(v);
  f(x);
  return f.found;
}

/// \brief Returns true if the term has a given free variable as subterm.
/// \param[in] x an expression
/// \param[in] v a variable
/// \return True if v occurs free in x.
template <typename T>
bool search_free_variable(const T& x, const variable& v)
{
  data::detail::search_free_variable_traverser<data::data_expression_traverser, data::add_data_variable_binding> f(v);
  f(x);
  return f.found;
}

/// \brief Returns true if the term has a given sort expression as subterm.
/// \param[in] container an expression or container of expressions
/// \param[in] s A sort expression
/// \return True if the term has a given sort expression as subterm.
template <typename Container>
bool search_sort_expression(Container const& container, const sort_expression& s)
{
  std::set<data::sort_expression> sort_expressions = data::find_sort_expressions(container);
  return sort_expressions.find(s) != sort_expressions.end();
}

/// \brief Returns true if the term has a given data expression as subterm.
/// \param[in] container an expression or container of expressions
/// \param[in] s A data expression
/// \return True if the term has a given data expression as subterm.
template <typename Container>
bool search_data_expression(Container const& container, const data_expression& s)
{
  std::set<data::data_expression> data_expressions = data::find_data_expressions(container);
  return data_expressions.find(s) != data_expressions.end();
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FIND_H
