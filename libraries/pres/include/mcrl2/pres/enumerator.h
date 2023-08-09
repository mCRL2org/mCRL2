// Author(s): Jan Friso Groote. Based on pbes/enumerator.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/enumerator.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_ENUMERATOR_H
#define MCRL2_PRES_ENUMERATOR_H

#include "mcrl2/data/enumerator.h"
#include "mcrl2/pres/pres_expression.h"

namespace mcrl2 {

namespace pres_system {

/* struct is_not_false
{
  bool operator()(const pres_expression& x) const
  {
    return !is_false(x);
  }
};

struct is_not_true
{
  bool operator()(const pres_expression& x) const
  {
    return !is_true(x);
  }
}; */

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_ENUMERATOR_H
