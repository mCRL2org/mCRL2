// Author(s): Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/find.h
/// \brief add your file description here.

#ifndef MCRL2_BES_FIND_H
#define MCRL2_BES_FIND_H

#include <iterator>
#include "mcrl2/bes/boolean_expression.h"
#include "mcrl2/bes/traverser.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2
{

namespace bes
{

namespace detail
{

template <template <class> class Traverser, class OutputIterator>
struct find_boolean_variables_traverser: public Traverser<find_boolean_variables_traverser<Traverser, OutputIterator> >
{
  typedef Traverser<find_boolean_variables_traverser<Traverser, OutputIterator> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  OutputIterator out;

  find_boolean_variables_traverser(OutputIterator out_)
    : out(out_)
  {}

  void operator()(const boolean_variable& v)
  {
    *out = v;
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

template <template <class> class Traverser, class OutputIterator>
find_boolean_variables_traverser<Traverser, OutputIterator>
make_find_boolean_variables_traverser(OutputIterator out)
{
  return find_boolean_variables_traverser<Traverser, OutputIterator>(out);
}

} // namespace detail

/// \brief Returns all boolean variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \param[in,out] o an output iterator to which all variables occurring in t
///             are added.
/// \return All variables that occur in the term t
template <typename Container, typename OutputIterator>
void find_boolean_variables(Container const& container, OutputIterator o)
{
  bes::detail::make_find_boolean_variables_traverser<bes::boolean_expression_traverser>(o)(container);
}

/// \brief Returns all variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \return All variables that occur in the term t
template <typename Container>
std::set<boolean_variable> find_boolean_variables(Container const& container)
{
  std::set<boolean_variable> result;
  bes::find_boolean_variables(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given boolean variable as subterm.
/// \param[in] container an expression or container with expressions
/// \param[in] v a boolean variable
/// \return True if the term has a given boolean variable as subterm.
template <typename Container>
bool search_boolean_variable(Container const& container, const boolean_variable& v)
{
	using utilities::detail::contains;
  std::set<boolean_variable> boolean_variables = bes::find_boolean_variables(container);
  return contains(boolean_variables, v);
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_FIND_H
