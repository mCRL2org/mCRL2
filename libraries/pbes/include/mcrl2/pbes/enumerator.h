// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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

namespace mcrl2::pbes_system {

struct is_not_false
{
  bool operator()(const pbes_expression& x) const
  {
    return !is_false(x);
  }
};

struct is_not_true
{
  bool operator()(const pbes_expression& x) const
  {
    return !is_true(x);
  }
};

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_ENUMERATOR_H
