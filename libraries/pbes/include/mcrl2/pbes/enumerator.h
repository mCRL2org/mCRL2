// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/enumerator.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_ENUMERATOR_H
#define MCRL2_PBES_ENUMERATOR_H

#include "mcrl2/data/enumerator.h"
#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2 {

namespace pbes_system {

struct is_not_false
{
  typedef core::term_traits<pbes_expression> tr;
  bool operator()(const pbes_expression& x) const
  {
    return !tr::is_false(x);
  }
};

struct is_not_true
{
  typedef core::term_traits<pbes_expression> tr;
  bool operator()(const pbes_expression& x) const
  {
    return !tr::is_true(x);
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ENUMERATOR_H
