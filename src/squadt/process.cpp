#ifndef PROCESS_TCC
#define PROCESS_TCC

#include <cstdlib>
#include <csignal>
#include <unistd.h>
#include <sys/wait.h>

#undef barrier

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include "task_monitor.h"
#include "command.h"

namespace squadt {
  namespace execution {

    boost::shared_ptr < task_monitor > process::default_monitor;

    void process::terminate() {
      /* Inform monitor */
      boost::shared_ptr < task_monitor > l = monitor.lock();
      
      if (l.get() != 0) {
        l->disconnect(this);
      }

      kill(identifier, SIGKILL);
    }
 
    /**
     * @param[in] c the command to execute
     **/
    void process::operator() (const command& c) {
      boost::shared_array < char const* > arguments(c.argument_array());

      identifier = fork();

      if (identifier == 0) {
        /* Change working directory to the project directory */
        chdir(c.get_working_directory().c_str());

        execv(c.executable.c_str(), const_cast < char* const* > (arguments.get()));
      }

      current_status = (identifier < 0) ? aborted : running;

      /* Inform monitor */
      boost::shared_ptr < task_monitor > l = monitor.lock();

      if (0 < identifier) {
        int exit_code;

        last_command = std::auto_ptr < command > (new command(c));

        l->signal_change(current_status);

        waitpid(identifier, &exit_code, 0);

        current_status = (WIFEXITED(exit_code)) ? completed : aborted;

        /* Inform monitor */
        if (l.get() != 0) {
          l->signal_change(current_status);
        }

        signal_termination(this);
      }
      else {
        if (l.get() != 0) {
          l->signal_change(current_status);
        }
      }
    }

    /**
     * @param c the command to execute
     **/
    void process::execute(const command& c) {
      execution_thread = boost::shared_ptr < boost::thread >(new boost::thread(
                              boost::bind(&process::operator(), this, c)));
    }

    void process::wait() const {
      int exit_code;

      waitpid(identifier, &exit_code, 0);
    }
  }
}

#endif

