// Author(s): Ruud Koolen, Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file solver.h

#ifndef MCRL2_SMT_SOLVER_H
#define MCRL2_SMT_SOLVER_H

#ifdef WIN32
  #include <windows.h>
  #include <tchar.h>
  #include <stdio.h>
  #include <strsafe.h>
#else
  #include <sys/wait.h>
#endif

#include <cstring>
#include <csignal>
#include <errno.h>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/smt/constructed_sort_definition.h"
#include "mcrl2/smt/data_specification.h"
#include "mcrl2/smt/named_function_definition.h"
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

#ifdef WIN32
  HANDLE g_hChildStd_IN_Rd = NULL;
  HANDLE g_hChildStd_IN_Wr = NULL;
  HANDLE g_hChildStd_OUT_Rd = NULL;
  HANDLE g_hChildStd_OUT_Wr = NULL;
#else
  int pipe_stdin[2];
  int pipe_stdout[2];
  int pipe_stderr[2];
#endif

private:

  void execute(const std::string& command) const
  {
#ifdef WIN32
    DWORD dwWritten;
    BOOL bSuccess = FALSE;

    bSuccess = WriteFile(g_hChildStd_IN_Wr, command.c_str(), command.size(), &dwWritten, NULL);
    if(!bSuccess || dwWritten != command.size())
    {
      throw mcrl2::runtime_error("Failed to write SMT problem to the solver.\n" + command);
    }
#else
    if(::write(pipe_stdin[1], command.c_str(), command.size()) < 0)
    {
      throw mcrl2::runtime_error("Failed to write SMT problem to the solver.\n" + command);
    }
#endif
  }

  bool execute_and_check(const std::string& command) const
  {
    execute(command);

#ifdef WIN32
    DWORD dwRead, totalRead;
    CHAR chBuf[256];
    BOOL bSuccess = FALSE;

    totalRead = 0;
    do
    {
      bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf + totalRead, 256 - totalRead, &dwRead, NULL);
      totalRead += dwRead;
    } while(bSuccess && chBuf[totalRead-1] != '\n');
    if(!bSuccess || totalRead == 0 )
    {
      throw mcrl2::runtime_error("The SMT prover does not work properly.");
    }
    if(std::strncmp(chBuf, "sat", 3) == 0)
    {
      return true;
    }
    else if(std::strncmp(chBuf, "unsat", 5) == 0)
    {
      return false;
    }
    else if(std::strncmp(chBuf, "unknown", 7) == 0)
    {
      throw mcrl2::runtime_error("Cannot determine whether this formula is satisfiable or not.");
    }
    else
    {
      mCRL2log(error) << "Error when checking satisfiability of \n" << command << std::endl;
      throw mcrl2::runtime_error("Got unexpected response from SMT-solver: " + std::string(chBuf, totalRead));
    }
#else
    char output[256];
    // check return value
    if(0 < ::read(pipe_stdout[0], output, 256))
    {
      if(std::strncmp(output, "sat", 3) == 0)
      {
        return true;
      }
      else if(std::strncmp(output, "unsat", 5) == 0)
      {
        return false;
      }
      else if(std::strncmp(output, "unknown", 7) == 0)
      {
        throw mcrl2::runtime_error("Cannot determine whether this formula is satisfiable or not.");
      }
      else
      {
        mCRL2log(error) << "Error when checking satisfiability of \n" << command << std::endl;
        throw mcrl2::runtime_error("Got unexpected response from SMT-solver: " + std::string(output, 256));
      }
    }
    else
    {
      std::string message;

      while(int i = ::read(pipe_stderr[0], output, 64))
      {
        message.append(output, 0, i);
      }

      throw mcrl2::runtime_error("The SMT prover does not work properly. " + message);
    }
#endif
  }

#ifdef WIN32
  void initialize_solver()
  {
    SECURITY_ATTRIBUTES saAttr;
    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT.
    if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
    {
      throw mcrl2::runtime_error("Could not connect to SMT solver: failed to create pipe Stdout");
    }
    // Ensure the read handle to the pipe for STDOUT is not inherited.
    if(!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
    {
      throw mcrl2::runtime_error("Could not modify SMT solver handle: SetHandleInformation Stdout");
    }
    // Create a pipe for the child process's STDIN.
    if(!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
    {
      throw mcrl2::runtime_error("Could not connect to SMT solver: failed to create pipe Stdin");
    }
    // Ensure the write handle to the pipe for STDIN is not inherited.
    if(!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
    {
      throw mcrl2::runtime_error("Could not modify SMT solver handle: SetHandleInformation Stdin");
    }
    // Create the child process.
    TCHAR szCmdline[] = TEXT("z3.exe -smt2 -in");
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    BOOL bSuccess = FALSE;

    // Set up members of the PROCESS_INFORMATION structure.
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure.
    // This structure specifies the STDIN and STDOUT handles for redirection.
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = g_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.hStdInput = g_hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Create the child process.
    bSuccess = CreateProcess(NULL,
      szCmdline,     // command line
      NULL,          // process security attributes
      NULL,          // primary thread security attributes
      TRUE,          // handles are inherited
      0,             // creation flags
      NULL,          // use parent's environment
      NULL,          // use parent's current directory
      &siStartInfo,  // STARTUPINFO pointer
      &piProcInfo);  // receives PROCESS_INFORMATION

    // If an error occurs, exit the application.
    if(!bSuccess)
    {
      throw mcrl2::runtime_error("Problem occurred while creating SMT solver process.\nIs Z3 in your path?");
    }
    else
    {
      // Close handles to the child process and its primary thread.
      // Some applications might keep these handles to monitor the status
      // of the child process, for example.
      CloseHandle(piProcInfo.hProcess);
      CloseHandle(piProcInfo.hThread);
    }
  }
#else
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

      throw mcrl2::runtime_error("Problem occurred while forking SMT solver process.\nIs Z3 in your path?");
    }
    else
    {
      ::close(pipe_stdin[0]);
      ::close(pipe_stdout[1]);
      ::close(pipe_stderr[1]);
    }
  }
#endif

public:
  solver(data_specification* spec)
  : m_spec(spec)
  {
#ifndef WIN32
    signal(SIGPIPE, SIG_IGN);
#endif
    initialize_solver();
    // to_solver = new fdostream(pipe_stdin[1]);
    execute(m_spec->generate_data_specification());
  }

  ~solver()
  {
    // Clean up the child process
    // It terminates when we close its stdin
#ifdef WIN32
    CloseHandle(g_hChildStd_IN_Wr)
#else
    ::close(pipe_stdin[1]);
    ::close(pipe_stdout[0]);
    ::close(pipe_stderr[0]);

    int return_status;
    ::wait(&return_status);
#endif
  }

  bool solve(const smt_problem& problem) const
  {
    execute("(push)\n");
    bool result = execute_and_check(m_spec->generate_smt_problem(problem));
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


