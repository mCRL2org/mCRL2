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

#include "mcrl2/utilities/platform.h"
#include "mcrl2/smt/native_translation.h"
#ifdef MCRL2_PLATFORM_WINDOWS
  #include <windows.h>
  #include <tchar.h>
  #include <stdio.h>
  #include <strsafe.h>
#endif // MCRL2_PLATFORM_WINDOWS

namespace mcrl2
{
namespace smt
{

class smt_solver
{
protected:
  native_translations m_native;

#ifdef MCRL2_PLATFORM_WINDOWS
  HANDLE g_hChildStd_IN_Rd = NULL;
  HANDLE g_hChildStd_IN_Wr = NULL;
  HANDLE g_hChildStd_OUT_Rd = NULL;
  HANDLE g_hChildStd_OUT_Wr = NULL;
#else
  int pipe_stdin[2];
  int pipe_stdout[2];
  int pipe_stderr[2];
#endif // MCRL2_PLATFORM_WINDOWS

protected:

  void execute(const std::string& command) const;

  bool execute_and_check(const std::string& command) const;

  void initialize_solver();

public:
  smt_solver(const data::data_specification& dataspec);

  ~smt_solver();

  bool solve(const data::variable_list& vars, const data::data_expression& expr) const;
};

} // namespace smt
} // namespace mcrl2

#endif // MCRL2_SMT_SOLVER_H
