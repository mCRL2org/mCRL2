// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsbinary.cpp
/// \brief Abstract interface for SMT solvers

#ifndef SMT_SOLVER_H
#define SMT_SOLVER_H

#include "aterm2.h"

namespace mcrl2 {
  namespace new_data {
    namespace detail {

      class SMT_Solver {
        public:
          virtual ~SMT_Solver() {};
          virtual bool is_satisfiable(ATermList a_formula) = 0;
      };
    }
  }
}

#endif
