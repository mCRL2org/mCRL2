// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_SOLVER_H
#define MCRL2_SMT_SOLVER_H

#include <cstring>
#include <csignal>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/smt/constructed_sort_definition.h"
#include "mcrl2/smt/data_specification.h"
#include "mcrl2/smt/named_function_definition.h"
#include "mcrl2/smt/pp_function_definition.h"
#include "mcrl2/smt/sort_definition.h"

using namespace mcrl2::log;

namespace mcrl2
{
namespace smt
{

class solver
{
private:
  data_specification* m_spec;

  int pipe_stdin[2];
  int pipe_stdout[2];
  int pipe_stderr[2];

private:

  void execute(const std::string& command)
  {
    if(::write(pipe_stdin[1], command.c_str(), command.size()) < 0)
    {
      throw mcrl2::runtime_error("Failed to write SMT problem to the solver.");
    }
  }

  bool execute_and_check(const std::string& command)
  {
    execute(command);

    char output[64];

    // check return value
    if (0 < ::read(pipe_stdout[0], output, 64))
    {
      if (std::strncmp(output, "sat", 3) == 0)
      {
        mCRL2log(verbose) << "The formula is satisfiable" << std::endl;
        return true;
      }
      else if (std::strncmp(output, "unsat", 5) == 0)
      {
        mCRL2log(verbose) << "The formula is unsatisfiable" << std::endl;
        return false;
      }
      else if (std::strncmp(output, "unknown", 7) == 0)
      {
        throw mcrl2::runtime_error("Cannot determine whether this formula is satisfiable or not.");
      }
      else
      {
        mCRL2log(error) << "Error when checking satisfiability of \n" << command << std::endl;
        throw mcrl2::runtime_error("Got unexpected response from SMT-solver: " + std::string(output, 64));
      }
    }
    else
    {
      std::string message;

      while (int i = ::read(pipe_stderr[0], output, 64))
      {
        message.append(output, 0, i);
      }

      throw mcrl2::runtime_error("The SMT prover does not work properly. " + message );
    }
  }

  void initialize_solver()
  {
    // Create pipes (two pairs r/w)
    if (::pipe(&pipe_stdin[0]) < 0)
    {
      throw mcrl2::runtime_error("failed to create pipe");
    }

    if (::pipe(&pipe_stdout[0]) < 0)
    {
      throw mcrl2::runtime_error("failed to create pipe");
    }

    if (::pipe(&pipe_stderr[0]) < 0)
    {
      throw mcrl2::runtime_error("failed to create pipe");
    }

    // fork process
    pid_t pid = ::fork();
    if (pid == 0)
    {
      // Forked process keeps waiting for input
      ::dup2(pipe_stdin[0], STDIN_FILENO);
      ::dup2(pipe_stdout[1], STDOUT_FILENO);
      ::dup2(pipe_stderr[1], STDERR_FILENO);

      ::close(pipe_stdin[1]);
      ::close(pipe_stdout[0]);
      ::close(pipe_stderr[0]);

      ::execlp("z3", "z3", "-smt2", "-in", (char*)nullptr);

      ::_exit(errno);
    }
    else if (pid < 0)
    {
      mCRL2log(log::error) << strerror(errno) << std::endl;

      ::close(pipe_stdin[0]);
      ::close(pipe_stdin[1]);
      ::close(pipe_stdout[0]);
      ::close(pipe_stdout[1]);
      ::close(pipe_stderr[0]);
      ::close(pipe_stderr[1]);

      throw mcrl2::runtime_error("Problem occured while forking SMT solver process.");
    }
    else
    {
      ::close(pipe_stdin[0]);
      ::close(pipe_stdout[1]);
      ::close(pipe_stderr[1]);
    }
  }

public:
  solver(data_specification* spec)
  : m_spec(spec)
  {
    signal(SIGPIPE, SIG_IGN);
    initialize_solver();
    execute(m_spec->generate_data_specification());
  }

  ~solver()
  {
    // Clean up the child process
    // It terminates when we close its stdin
    ::close(pipe_stdin[1]);
    ::close(pipe_stdout[0]);
    ::close(pipe_stderr[0]);

    int return_status;
    ::wait(&return_status);
  }

  bool solve(const smt_problem& problem)
  {
    const std::string& problem_string = m_spec->generate_smt_problem(problem);
    execute("(push)\n");
    bool result = execute_and_check(problem_string);
    execute("(pop)\n");
    return result;
  }
};

// class z3_solver: public solver<z3_solver>
// {
// public:
//   typedef solver<z3_solver> super;

// public:

//   z3_solver()
//   : super()
//   {}

//   bool execute(const smt_problem& problem)
//   {

//   }

// };

} // namespace smt
} // namespace mcrl2

#endif // MCRL2_SMT_SOLVER_H
