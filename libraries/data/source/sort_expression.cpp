// Author(s): Jeroen Keiren, Jeroen van der Wulp, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/sort_expression.cpp
/// \brief Implementation details for sort_expression

#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/real.h"

namespace mcrl2 {
  namespace data {

    bool is_system_defined(const sort_expression& s)
    {
      return sort_bool::is_bool(s) || sort_real::is_real(s) ||
             sort_int::is_int(s) || sort_nat::is_nat(s) ||
             sort_pos::is_pos(s) || is_container_sort(s) || is_structured_sort(s);
    }

  } // namespace data
} // namespace mcrl2

