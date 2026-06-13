#if !(defined(_MSC_VER) || defined(__MINGW32__) || defined(__CYGWIN__))
#include <array>
#include <cerrno>       // for errno
#include <sys/types.h>  // for pid_t
#include <sys/wait.h>   // for waitpid()
#include <sys/ioctl.h>  // for ioctl() and FIONREAD
#include "mcrl2/data/detail/prover/smt_lib_solver.h"

using namespace mcrl2::log;




namespace mcrl2::data::detail::prover
{
template < typename T >
bool binary_smt_solver< T >::execute(std::string const& benchmark)
{
  std::array<int, 2> pipe_stdin;
  std::array<int, 2> pipe_stdout;
  std::array<int, 2> pipe_stderr;

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
    ::dup2(pipe_stdin[0], STDIN_FILENO);
    ::dup2(pipe_stdout[1], STDOUT_FILENO);
    ::dup2(pipe_stderr[1], STDERR_FILENO);

    ::close(pipe_stdin[1]);
    ::close(pipe_stdout[0]);
    ::close(pipe_stderr[0]);

    T::exec();

    ::_exit(errno);
  }
  else if (pid < 0)
  {
    mCRL2log(error) << strerror(errno) << std::endl;

    ::close(pipe_stdin[0]);
    ::close(pipe_stdin[1]);
    ::close(pipe_stdout[0]);
    ::close(pipe_stdout[1]);
    ::close(pipe_stderr[0]);
    ::close(pipe_stderr[1]);

    return false;
  }
  else
  {
    if(::write(pipe_stdin[1], benchmark.c_str(), benchmark.size()) < 0)
    {
      throw mcrl2::runtime_error("failed to write benchmark");
    }

    ::close(pipe_stdin[0]);
    ::close(pipe_stdin[1]);
    ::close(pipe_stdout[1]);
    ::close(pipe_stderr[1]);

    std::array<char, 64> output;

    int return_status;

    // check return value
    if (0 < ::read(pipe_stdout[0], output.data(), 8))
    {
      if (strncmp(output.data(), "sat", 3) == 0)
      {
        mCRL2log(verbose) << "The formula is satisfiable" << std::endl;

        ::close(pipe_stdout[0]);
        ::close(pipe_stderr[0]);

        ::wait(&return_status);

        return true;
      }
      else if (strncmp(output.data(), "unsat", 5) == 0)
      {
        mCRL2log(verbose) << "The formula is unsatisfiable" << std::endl;
      }
      else if (strncmp(output.data(), "unknown", 7) == 0)
      {
        mCRL2log(verbose) << T::name() << " cannot determine whether this formula is satisfiable or not." << std::endl;
      }
    }
    else
    {
      std::string message;

      while (ssize_t i = ::read(pipe_stderr[0], output.data(), 64))
      {
        message.append(output.data(), static_cast<std::size_t>(i));
      }

      throw mcrl2::runtime_error(std::string("The SMT prover ") + T::name() + " does not work properly. " + message );
    }

    ::close(pipe_stdout[0]);
    ::close(pipe_stderr[0]);

    ::wait(&return_status);
  }

  return false;
}

/**
 * \brief Checks the availability/usability of the prover
 **/
template < typename T >
bool binary_smt_solver< T >::usable()
{
  if (!binary_smt_solver::execute("(benchmark nameless :formula true)"))
  {
    throw mcrl2::runtime_error(std::string("The SMT solver ") + T::name() + " is not available. \n" + 
                     "Consult the manual of the tool you are using for instructions on how to obtain " + T::name() + ".");

    return false;
  }

  return true;
}

// instantiate for cvc prover
template class binary_smt_solver< cvc_smt_solver >;

// instantiate for z3 prover
template class binary_smt_solver< z3_smt_solver >;

// instantiate for ario prover
template class binary_smt_solver< ario_smt_solver >;
}



#endif

