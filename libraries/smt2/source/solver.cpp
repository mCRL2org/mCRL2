// Author(s): Ruud Koolen, Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file solver.cpp

#include "mcrl2/data/bool.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/smt2/translate_expression.h"
#include "mcrl2/smt2/translate_specification.h"
#include "mcrl2/smt2/solver.h"
#include "mcrl2/smt2/unfold_pattern_matching.h"
#include "mcrl2/utilities/logger.h"

#ifdef MCRL2_PLATFORM_WINDOWS
  #include <windows.h>
  #include <tchar.h>
  #include <stdio.h>
  #include <strsafe.h>
#else
  #include <unistd.h>
  #include <sys/wait.h>
#endif // MCRL2_PLATFORM_WINDOWS

#include <cstring>
#include <csignal>
#include <errno.h>

namespace mcrl2
{
namespace smt
{

void smt_solver::execute(const std::string& command) const
{
#ifdef MCRL2_PLATFORM_WINDOWS
  DWORD dwWritten;
  BOOL bSuccess = FALSE;

  bSuccess = WriteFile(g_hChildStd_IN_Wr, command.c_str(), command.size(), &dwWritten, NULL);
  if(!bSuccess || dwWritten != command.size())
  {
    throw mcrl2::runtime_error("Failed to write SMT problem to the solver.\n" + command);
  }
#else // MCRL2_PLATFORM_WINDOWS
  if(::write(pipe_stdin[1], command.c_str(), command.size()) < 0)
  {
    throw mcrl2::runtime_error("Failed to write SMT problem to the solver.\n" + command);
  }
#endif
}

bool smt_solver::execute_and_check(const std::string& command) const
{
  execute(command);

#ifdef MCRL2_PLATFORM_WINDOWS
  DWORD dwRead, totalRead;
  CHAR chBuf[512];
  BOOL bSuccess = FALSE;

  totalRead = 0;
  do
  {
    bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf + totalRead, 512 - totalRead, &dwRead, NULL);
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
  char output[512];
  // check return value
  if(0 < ::read(pipe_stdout[0], output, 512))
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
      throw mcrl2::runtime_error("Cannot determine whether this formula is satisfiable or not.\n" + command);
    }
    else
    {
      mCRL2log(log::error) << "Error when checking satisfiability of \n" << command << std::endl;
      throw mcrl2::runtime_error("Got unexpected response from SMT-solver: " + std::string(output, 512));
    }
  }
  else
  {
    std::string message;

    while(int i = ::read(pipe_stderr[0], output, 512))
    {
      message.append(output, 0, i);
    }

    throw mcrl2::runtime_error("The SMT prover does not work properly. " + message);
  }
#endif // MCRL2_PLATFORM_WINDOWS
}

#ifdef MCRL2_PLATFORM_WINDOWS
void smt_solver::initialize_solver()
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
void smt_solver::initialize_solver()
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
#endif // MCRL2_PLATFORM_WINDOWS

smt_solver::smt_solver(const data::data_specification& dataspec)
: m_native(initialise_native_translation(dataspec))
{
#ifndef MCRL2_PLATFORM_WINDOWS
  signal(SIGPIPE, SIG_IGN);
#endif
  initialize_solver();
  std::ostringstream out;
  translate_data_specification(dataspec, out, m_native);
  mCRL2log(log::debug) << out.str() << std::endl;
  execute(out.str());
}

smt_solver::~smt_solver()
{
  // Clean up the child process
  // It terminates when we close its stdin
#ifdef MCRL2_PLATFORM_WINDOWS
  CloseHandle(g_hChildStd_IN_Wr);
#else
  ::close(pipe_stdin[1]);
  ::close(pipe_stdout[0]);
  ::close(pipe_stderr[0]);

  int return_status;
  ::wait(&return_status);
#endif
}

bool smt_solver::solve(const data::variable_list& vars, const data::data_expression& expr) const
{
  execute("(push)\n");
  std::ostringstream out;
  translate_variable_declaration(vars, out, m_native);
  translate_assertion(expr, out, m_native);
  out << "(check-sat)\n";
  mCRL2log(log::debug) << out.str() << std::endl;
  bool result = execute_and_check(out.str());
  execute("(pop)\n");
  return result;
}


inline
native_translations initialise_native_translation(const data::data_specification& dataspec)
{
  using namespace detail;
  using namespace data;
  native_translations nt;

  nt.sorts[sort_bool::bool_()] = "Bool";
  nt.sorts[sort_pos::pos()] = "Int";
  nt.sorts[sort_nat::nat()] = "Int";
  nt.sorts[sort_int::int_()] = "Int";
  nt.sorts[sort_real::real_()] = "Real";

  std::vector<sort_expression> number_sorts({ sort_pos::pos(), sort_nat::nat(), sort_int::int_(), sort_real::real_() });
  for(const sort_expression& sort: dataspec.sorts())
  {
    if(is_basic_sort(sort) || is_container_sort(sort))
    {
      nt.set_native_definition(equal_to(sort), "=");
      nt.set_native_definition(not_equal_to(sort), "distinct");
      nt.set_native_definition(if_(sort), "ite");

      if(std::find(number_sorts.begin(), number_sorts.end(), sort) != number_sorts.end())
      {
        nt.set_native_definition(less(sort));
        nt.set_native_definition(less_equal(sort));
        nt.set_native_definition(greater(sort));
        nt.set_native_definition(greater_equal(sort));
        nt.set_alternative_name(sort_real::exp(sort, sort == sort_real::real_() ? sort_int::int_() : sort_nat::nat()), "^");
        nt.set_alternative_name(sort_real::minus(sort, sort), "-");
        nt.set_alternative_name(sort_real::negate(sort), "-");
      }
      else if(sort == basic_sort("@NatPair"))
      {
        // NatPair is not used and its equations upset Z3
        nt.set_native_definition(less(sort));
        nt.set_native_definition(less_equal(sort));
        nt.set_native_definition(greater(sort));
        nt.set_native_definition(greater_equal(sort));
      }
      else
      {
        // Functions <, <=, >, >= are already defined on Int/Real and cannot be overloaded
        nt.set_alternative_name(less(sort), "@less");
        nt.set_alternative_name(less_equal(sort), "@less_equal");
        nt.set_alternative_name(greater(sort), "@greater");
        nt.set_alternative_name(greater_equal(sort), "@greater_equal");
      }

      std::string printed_sort_name = nt.sorts.find(sort) != nt.sorts.end() ? nt.sorts.find(sort)->second : translate_identifier(pp(sort));
      std::string list_sort_name = "(List " + printed_sort_name + ")";
      nt.set_alternative_name(sort_list::count(sort), "@list_count");
      nt.set_alternative_name(sort_list::snoc(sort), "@snoc");
      nt.set_native_definition(sort_list::empty(sort), "nil");
      nt.set_ambiguous(sort_list::empty(sort));
      nt.set_native_definition(sort_list::cons_(sort), "insert");
      nt.set_native_definition(sort_list::head(sort));
      nt.set_native_definition(sort_list::tail(sort));
      nt.sorts[sort_list::list(sort)] = list_sort_name;
    }
  }

  std::vector<function_symbol_vector> fs_numbers(
    {
      sort_pos::pos_generate_functions_code(),
      sort_nat::nat_generate_functions_code(),
      sort_int::int_generate_functions_code(),
      sort_real::real_generate_functions_code()
    }
  );
  for(const auto& fsv: fs_numbers)
  {
    for(const auto& fs: fsv)
    {
      nt.set_native_definition(fs);
    }
  }

  auto one = [](const sort_expression& s)
  {
    return s == sort_int::int_() ? sort_int::int_(1) : sort_real::real_(1);
  };
  for(const sort_expression& s: { sort_int::int_(), sort_real::real_() })
  {
    variable v1("v1", s);
    variable v2("v2", s);
    variable_list l1({v1});
    variable_list l2({v1,v2});
    nt.mappings[sort_real::minimum(s,s)] = data_equation(l2, sort_real::minimum(v1, v2), if_(less(v1,v2), v1, v2));
    nt.mappings[sort_real::maximum(s,s)] = data_equation(l2, sort_real::maximum(v1, v2), if_(less(v1,v2), v2, v1));
    nt.mappings[sort_real::succ(s)] = data_equation(l1, sort_real::succ(v1), sort_real::plus(v1, one(s)));
    nt.mappings[sort_real::pred(s)] = data_equation(l1, sort_real::pred(v1), sort_real::minus(v1, one(s)));
    nt.set_ambiguous(sort_real::minimum(s,s));
    nt.set_ambiguous(sort_real::maximum(s,s));
    nt.set_ambiguous(sort_real::succ(s));
    nt.set_ambiguous(sort_real::pred(s));
  }
  // TODO come up with equations for these
  // nt.mappings[sort_real::floor(s)]
  // nt.mappings[sort_real::ceil(s)]
  // nt.mappings[sort_real::round(s)]

  nt.set_native_definition(sort_bool::not_(), "not");
  nt.set_native_definition(sort_bool::and_(), "and");
  nt.set_native_definition(sort_bool::or_(), "or");
  nt.set_native_definition(sort_bool::implies());

  nt.set_native_definition(sort_pos::c1(), pp(sort_pos::c1()));
  nt.set_native_definition(sort_nat::c0(), pp(sort_nat::c0()));
  nt.expressions[sort_pos::cdub()] = pp_translation;
  nt.set_native_definition(sort_nat::cnat(), "@id");
  nt.set_native_definition(sort_int::cneg(), "-");
  nt.set_native_definition(sort_int::cint(), "@id");
  nt.expressions[sort_real::creal()] = pp_real_translation;
  nt.set_native_definition(sort_real::creal());

  nt.special_recogniser[data::sort_bool::true_()] = "@id";
  nt.special_recogniser[data::sort_bool::false_()] = "not";
  nt.special_recogniser[data::sort_pos::c1()] = "= 1";
  nt.special_recogniser[data::sort_pos::cdub()] = ">= 2";
  nt.special_recogniser[data::sort_nat::c0()] = "= 0";
  nt.special_recogniser[data::sort_nat::cnat()] = ">= 1";
  nt.special_recogniser[data::sort_int::cneg()] = "< 0";
  nt.special_recogniser[data::sort_int::cint()] = ">= 0";

  nt.set_native_definition(sort_nat::pos2nat(), "@id");
  nt.set_native_definition(sort_nat::nat2pos(), "@id");
  nt.set_native_definition(sort_int::pos2int(), "@id");
  nt.set_native_definition(sort_int::int2pos(), "@id");
  nt.set_native_definition(sort_int::nat2int(), "@id");
  nt.set_native_definition(sort_int::int2nat(), "@id");
  nt.set_native_definition(sort_real::pos2real(), "to_real");
  nt.set_native_definition(sort_real::real2pos(), "to_int");
  nt.set_native_definition(sort_real::nat2real(), "to_real");
  nt.set_native_definition(sort_real::real2nat(), "to_int");
  nt.set_native_definition(sort_real::int2real(), "to_real");
  nt.set_native_definition(sort_real::real2int(), "to_int");

  function_symbol id_int("@id", function_sort({sort_int::int_()}, sort_int::int_()));
  function_symbol id_real("@id", function_sort({sort_real::real_()}, sort_real::real_()));
  variable vi("i", sort_int::int_());
  variable vr("r", sort_real::real_());
  nt.mappings[id_int] = data_equation(variable_list({vi}), id_int(vi), vi);
  nt.mappings[id_real] = data_equation(variable_list({vr}), id_real(vr), vr);
  // necessary for translating the two equations above
  nt.set_native_definition(equal_to(sort_int::int_()), "=");
  nt.set_native_definition(equal_to(sort_real::real_()), "=");

  unfold_pattern_matching(dataspec, nt);

  return nt;
}


} // namespace smt
} // namespace mcrl2
