// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/bes_equation_limit.h
/// \brief A global variable for counting the number of BES equations in pbes2bes
/// and parity_game_generator. If the number of equations exceeds a limit, an
/// exception is thrown. A static template class variable is used, that can be
/// set from everywhere.

#ifndef MCRL2_PBES_DETAIL_BES_EQUATION_LIMIT_H
#define MCRL2_PBES_DETAIL_BES_EQUATION_LIMIT_H

#include <stdexcept>

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  template <class T> // note, T is only a dummy
  struct bes_equation_limit
  {
  	// -1 means unlimited
    static unsigned int max_bes_equations;
  };

  template <class T>
  unsigned int bes_equation_limit<T>::max_bes_equations = -1;

  inline
  void set_bes_equation_limit(int size)
  {
    bes_equation_limit<int>::max_bes_equations = size;
  }

  inline
  void check_bes_equation_limit(int size)
  {
  	if (bes_equation_limit<int>::max_bes_equations >= 0 && bes_equation_limit<int>::max_bes_equations <= size)
    {
    	throw std::out_of_range("Error: number of BES equations has exceeded the limit");
    }
  }

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_BES_EQUATION_LIMIT_H
