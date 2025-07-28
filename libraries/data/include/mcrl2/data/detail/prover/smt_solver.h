// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file smt_solver.h
/// \brief Abstract interface for SMT solvers

#ifndef MCRL2_DATA_DETAIL_PROVER_SMT_SOLVER_H
#define MCRL2_DATA_DETAIL_PROVER_SMT_SOLVER_H

#include "mcrl2/data/data_expression.h"

namespace mcrl2::data::detail
{

class SMT_Solver
{
  public:
    virtual ~SMT_Solver() = default;
    virtual bool is_satisfiable(const data_expression_list &a_formula) = 0;
};
} // namespace mcrl2::data::detail

#endif
