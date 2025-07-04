// Author(s): Ruud Koolen, Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file solver.h

#ifndef MCRL2_SMT_SOLVER_H
#define MCRL2_SMT_SOLVER_H

#include "mcrl2/smt/child_process.h"
#include "mcrl2/smt/native_translation.h"
#include "mcrl2/smt/answer.h"

namespace mcrl2::smt
{

class smt_solver
{
protected:
  native_translations m_native;
  std::unordered_map<data::data_expression, std::string> m_cache;
  child_process z3;

protected:

  answer execute_and_check(const std::string& command, const std::chrono::microseconds& timeout) const;

public:
  smt_solver(const data::data_specification& dataspec);

  answer solve(const data::variable_list& vars, const data::data_expression& expr, const std::chrono::microseconds& timeout = std::chrono::microseconds::zero());
};

} // namespace mcrl2::smt

#endif // MCRL2_SMT_SOLVER_H
