// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes_expression_builder_indenter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBES_EXPRESSION_BUILDER_INDENTER_H
#define MCRL2_PBES_DETAIL_PBES_EXPRESSION_BUILDER_INDENTER_H

namespace mcrl2
{

namespace pbes_system
{

//namespace detail {

#ifdef MCRL2_PBES_EXPRESSION_BUILDER_DEBUG
// use a static variable to store the indentation depth
/// \cond INTERNAL_DOCS
template <class T> // note, T is only a dummy
struct pbes_expression_builder_indenter
{
  static std::size_t depth;
};

template <class T>
std::size_t pbes_expression_builder_indenter<T>::depth = 0;

inline
std::string pbes_expression_builder_indent()
{
  return std::string(pbes_expression_builder_indenter<int>::depth, ' ');
}

inline
void pbes_expression_builder_increase_indent()
{
  pbes_expression_builder_indenter<int>::depth += 2;
}

inline
void pbes_expression_builder_decrease_indent()
{
  pbes_expression_builder_indenter<int>::depth -= 2;
}
/// \endcond
#endif // MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

//} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_EXPRESSION_BUILDER_INDENTER_H
