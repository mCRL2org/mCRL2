// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/lps2pbes_indenter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_LPS2PBES_INDENTER_H
#define MCRL2_PBES_DETAIL_LPS2PBES_INDENTER_H

namespace mcrl2
{

namespace pbes_system
{

//namespace detail {

#ifdef MCRL2_PBES_TRANSLATE_DEBUG
// use a static variable to store the indentation depth
/// \cond INTERNAL_DOCS
template <class T> // note, T is only a dummy
struct lps2pbes_indenter
{
  // The amount of indentation
  static std::size_t depth;
};

template <class T>
std::size_t lps2pbes_indenter<T>::depth = 0;

inline
std::string lps2pbes_indent()
{
  return std::string(lps2pbes_indenter<int>::depth, ' ');
}

inline
void lps2pbes_increase_indent()
{
  lps2pbes_indenter<int>::depth += 2;
}

inline
void lps2pbes_decrease_indent()
{
  lps2pbes_indenter<int>::depth -= 2;
}

/// \endcond
#endif // MCRL2_PBES_TRANSLATE_DEBUG

//} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_LPS2PBES_INDENTER_H
