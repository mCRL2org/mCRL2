// Author(s): Jan Friso Groote based on bes/find.h by Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/res/find.h
/// \brief add your file description here.

#ifndef MCRL2_RES_FIND_H
#define MCRL2_RES_FIND_H

#include "mcrl2/res/traverser.h"

namespace mcrl2
{

namespace res
{

namespace detail
{

template <template <class> class Traverser, class OutputIterator>
struct find_res_variables_traverser: public Traverser<find_res_variables_traverser<Traverser, OutputIterator> >
{
  typedef Traverser<find_res_variables_traverser<Traverser, OutputIterator> > super;
  using super::enter;
  using super::leave;
  using super::apply;

  OutputIterator out;

  find_res_variables_traverser(OutputIterator out_)
    : out(out_)
  {}

  void apply(const res_variable& v)
  {
    *out = v;
  }
};

template <template <class> class Traverser, class OutputIterator>
find_res_variables_traverser<Traverser, OutputIterator>
make_find_res_variables_traverser(OutputIterator out)
{
  return find_res_variables_traverser<Traverser, OutputIterator>(out);
}

} // namespace detail

/// \brief Returns all res variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \param[in,out] o an output iterator to which all variables occurring in t
///             are added.
/// \return All variables that occur in the term t
template <typename Container, typename OutputIterator>
void find_res_variables(Container const& container, OutputIterator o)
{
  res::detail::make_find_res_variables_traverser<res::res_expression_traverser>(o).apply(container);
}

/// \brief Returns all variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \return All variables that occur in the term t
template <typename Container>
std::set<res_variable> find_res_variables(Container const& container)
{
  std::set<res_variable> result;
  res::find_res_variables(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given res variable as subterm.
/// \param[in] container an expression or container with expressions
/// \param[in] v a res variable
/// \return True if the term has a given res variable as subterm.
template <typename Container>
bool search_res_variable(Container const& container, const res_variable& v)
{
	using utilities::detail::contains;
  std::set<res_variable> res_variables = res::find_res_variables(container);
  return contains(res_variables, v);
}

} // namespace res

} // namespace mcrl2

#endif // MCRL2_RES_FIND_H
