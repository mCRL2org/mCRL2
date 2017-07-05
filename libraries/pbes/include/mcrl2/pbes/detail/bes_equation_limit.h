// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/bes_equation_limit.h
/// \brief A global variable for counting the number of BES equations in pbesinst
/// and parity_game_generator. If the number of equations exceeds a limit, an
/// exception is thrown. A static template class variable is used, that can be
/// set from everywhere.

#ifndef MCRL2_PBES_DETAIL_BES_EQUATION_LIMIT_H
#define MCRL2_PBES_DETAIL_BES_EQUATION_LIMIT_H

#include <limits>
#include <stdexcept>

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

template <class T> // note, T is only a dummy
struct bes_equation_limit
{
  static std::size_t max_bes_equations;
};

template <class T>
std::size_t bes_equation_limit<T>::max_bes_equations = (std::numeric_limits<std::size_t>::max)();

inline
void set_bes_equation_limit(std::size_t size)
{
  bes_equation_limit<std::size_t>::max_bes_equations = size;
}

inline
void check_bes_equation_limit(std::size_t size)
{
  if (size >= bes_equation_limit<std::size_t>::max_bes_equations)
  {
    throw std::out_of_range("Error: number of BES equations has exceeded the limit");
  }
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_BES_EQUATION_LIMIT_H
