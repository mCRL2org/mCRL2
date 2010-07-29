#if !(defined(_MSC_VER) || defined(__MINGW32__) || defined(__CYGWIN__))
#include <cerrno>       // for errno
#include <cstddef>      // for size_t
#include <cstdlib>
#include <sys/types.h>  // for pid_t
#include <sys/wait.h>   // for waitpid()
#include <sys/ioctl.h>  // for ioctl() and FIONREAD
#include <unistd.h>
#include <iostream>
#include "mcrl2/data/detail/prover/smt_lib_solver.h"

namespace mcrl2 {
  namespace data {
    namespace detail {
      namespace prover {
        template < typename T >
        bool binary_smt_solver< T >::execute(std::string const& benchmark) {
          int pipe_stdin[2];
          int pipe_stdout[2];
          int pipe_stderr[2];

          // Create pipes (two pairs r/w)
          ::pipe(&pipe_stdin[0]);
          ::pipe(&pipe_stdout[0]);
          ::pipe(&pipe_stderr[0]);

          // fork process
          pid_t pid = ::fork();

          if (pid == 0) {
            ::dup2(pipe_stdin[0], STDIN_FILENO);
            ::dup2(pipe_stdout[1], STDOUT_FILENO);
            ::dup2(pipe_stderr[1], STDERR_FILENO);

            ::close(pipe_stdin[1]);
            ::close(pipe_stdout[0]);
            ::close(pipe_stderr[0]);

            T::exec();

            ::_exit(errno);
          }
          else if (pid < 0) {
            core::gsErrorMsg(strerror(errno));

            ::close(pipe_stdin[0]);
            ::close(pipe_stdin[1]);
            ::close(pipe_stdout[0]);
            ::close(pipe_stdout[1]);
            ::close(pipe_stderr[0]);
            ::close(pipe_stderr[1]);

            return false;
          }
          else {
            ::write(pipe_stdin[1], benchmark.c_str(), benchmark.size());

            ::close(pipe_stdin[0]);
            ::close(pipe_stdin[1]);
            ::close(pipe_stdout[1]);
            ::close(pipe_stderr[1]);

            char output[64];

            int return_status;

            // check return value
            if (0 < ::read(pipe_stdout[0], output, 8)) {
              if (strncmp(output, "sat", 3) == 0) {
                core::gsVerboseMsg("The formula is satisfiable\n");

                ::close(pipe_stdout[0]);
                ::close(pipe_stderr[0]);

                ::wait(&return_status);

                return true;
              }
              else if (strncmp(output, "unsat", 5) == 0) {
                core::gsVerboseMsg("The formula is unsatisfiable\n");
              }
              else if (strncmp(output, "unknown", 7) == 0) {
                core::gsVerboseMsg("%s cannot determine whether this formula is satisfiable or not.\n", T::name());
              }
            }
            else {
              std::string message;

              while (int i = ::read(pipe_stderr[0], output, 64)) {
                message.append(output, 0, i);
              }

              core::gsErrorMsg("Fatal: SMT prover %s returned :\n\n%s\n", T::name(), message.c_str());
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
        bool binary_smt_solver< T >::usable() {
          if (!binary_smt_solver::execute("(benchmark nameless :formula true)")) {
            core::gsErrorMsg(
              "The SMT solver %s is not available.\n"
              "Consult the manual of the tool you are using for instructions on how to obtain %s.\n", T::name(), T::name());

            return false;
          }

          return true;
        }

        // instantiate for cvc prover
        template class binary_smt_solver< cvc_smt_solver >;

        // instantiate for ario prover
        template class binary_smt_solver< ario_smt_solver >;
      }
    }
  }
}
#endif

