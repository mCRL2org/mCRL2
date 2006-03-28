#ifndef PROCESS_TCC
#define PROCESS_TCC

#include <cstdlib>
#include <csignal>
#include <unistd.h>
#include <sys/wait.h>

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include "process_listener.h"
#include "command.h"

namespace squadt {
  namespace execution {

    boost::shared_ptr < process_listener > process::default_listener;

    void process::terminate() {
      kill(identifier, SIGKILL);
    }
 
    /**
     * @param c the command to execute
     **/
    void process::operator() (const command& c) {
      boost::shared_array < char const* > arguments(c.argument_array());

      identifier = fork();

      if (identifier == 0) {
        execv(c.executable.c_str(), const_cast < char* const* > (arguments.get()));
      }

      current_status = (identifier < 0) ? aborted : running;

      /* Inform listener */
      listener.lock()->report_change(current_status);

      if (0 < identifier) {
        int exit_code;

        waitpid(identifier, &exit_code, 0);

        current_status = (WIFEXITED(exit_code)) ? completed : aborted;

        /* Inform listener */
        listener.lock()->report_change(current_status);

        signal_termination(this);
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

