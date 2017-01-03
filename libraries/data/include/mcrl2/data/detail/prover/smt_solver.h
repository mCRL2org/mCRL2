// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file smt_solver.h
/// \brief Abstract interface for SMT solvers

#ifndef SMT_SOLVER_H
#define SMT_SOLVER_H

#include "mcrl2/data/data_expression.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

class SMT_Solver
{
  public:
    virtual ~SMT_Solver() {};
    virtual bool is_satisfiable(const data_expression_list &a_formula) = 0;
};
}
}
}

#endif
