#ifndef PROCESS_TCC
#define PROCESS_TCC

#include <cstdlib>
#include <csignal>
#include <cstring>
#include <cerrno>

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
# include <substitutes.h>
#else
# include <unistd.h>
# include <sys/wait.h>
#endif

#undef barrier

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/thread/thread.hpp>

#include "task_monitor.h"
#include "command.h"
#include "setup.h"

namespace squadt {
  namespace execution {

    boost::shared_ptr < task_monitor > process::default_monitor;

    void process::terminate() {
      /* Inform monitor */
      boost::shared_ptr < task_monitor > l = monitor.lock();
      
      if (l.get() != 0) {
        l->disconnect(this);
      }

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
      TerminateProcess(reinterpret_cast < void* > (identifier), 1);
#else
      kill(identifier, SIGKILL);
#endif
    }
 
    void process::termination_handler(pid_t identifier) {
      int exit_code;

      waitpid(identifier, &exit_code, 0);

      current_status = (WIFEXITED(exit_code)) ? completed : aborted;

      signal_status();

      signal_termination(this);
    }

    /** Signals the current state to the monitor */
    void process::signal_status() const {
      boost::shared_ptr < task_monitor > l = monitor.lock();

      if (l.get() != 0) {
        l->signal_change(current_status);
      }
    }

    /**
     * @param c the command to execute
     **/
    void process::execute(const command& c) {
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
      command cc(c);

      cc.prepend_argument(c.working_directory);
      cc.prepend_argument(c.executable);
      cc.set_executable(std::string(TOOL_DIRECTORY) + "\\process_wrapper.exe");

      identifier = _spawnv(_P_NOWAIT, cc.executable.c_str(), cc.get_argument_array().get());
#else
      boost::shared_array < char const* > arguments(c.get_argument_array());

      pid_t new_identifier = fork();

      if (new_identifier == 0) {
        /* Change working directory to the project directory */
        chdir(c.working_directory.c_str());

        execvp(c.executable.c_str(), const_cast < char* const* > (arguments.get()));

        std::cerr << boost::str(boost::format("Execution failed : `%s' %s\n") % c.executable % strerror(errno));

        _exit(1);
      }
      else {
        identifier = new_identifier;
      }
#endif

      current_status = (identifier < 0) ? aborted : running;

      if (0 < identifier) {
        using namespace boost;

        last_command.reset(new command(c));

        signal_status();

        /* Wait for the process to terminate */
        thread t(bind(&process::termination_handler, this, identifier));
      }
      else {
        signal_status();

        signal_termination(this);
      }
    }
  }
}

#endif

