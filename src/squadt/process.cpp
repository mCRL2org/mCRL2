#ifndef PROCESS_TCC
#define PROCESS_TCC

#include <cstdlib>
#include <csignal>
#include <cstring>
#include <cerrno>

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
# include <windows.h>
# include <substitutes.h>
# include <tchar.h>
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

    class process_impl {

      friend class process;

      private:

        /** \brief The status of this process */
        process::status                      current_status;

        /** \brief The function that is called when the status changes */
        process::handler                     signal_termination;
    
        /** \brief A reference to a monitor for this process */
        boost::weak_ptr < task_monitor >     monitor;

        /** \brief The command that is currently being executed (or 0) */
        std::auto_ptr < command >            m_command;

        /** \brief The process object that serves as interface to this object */
        process*                             interface_pointer;

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
        STARTUPINFO         si;
        PROCESS_INFORMATION pi;
#else
        /** \brief The system's proces identifier for this process */
        pid_t                                identifier;
#endif

      private:

        /** \brief Executed at process termination */
        void termination_handler(pid_t);

        /** \brief Initialisation procedure */
        void initialise();
 
      private:

        /** \brief The default listener for changes in status */
        static boost::shared_ptr < task_monitor >      default_monitor;

      public:

        /** \brief Constructor */
        process_impl(process::handler);
    
        /** \brief Constructor with listener */
        process_impl(process::handler, boost::shared_ptr < task_monitor >&);
 
        /** \brief Start the process by executing a command */
        void execute(const command&);
     
        /** \brief Returns the process id */
        pid_t get_identifier() const;
 
        /** \brief Signals the current state to the monitor */
        void signal_status() const;

        /** \brief Terminates the process */
        void terminate();
 
        /** \brief Destructor */
        ~process_impl();
    };

    boost::shared_ptr < task_monitor > process_impl::default_monitor;

    /**
     * \param h the function to call when the process terminates
     **/
    process_impl::process_impl(process::handler h) : current_status(process::stopped), signal_termination(h), monitor(default_monitor) {
      initialise();
    }

    /**
     * \param[in] h the function to call when the process terminates
     * \param[in] l a reference to a listener for process status change events
     **/
    process_impl::process_impl(process::handler h, boost::shared_ptr < task_monitor >& l) : current_status(process::stopped), signal_termination(h), monitor(l) {
      initialise();
    }

    inline void process_impl::initialise() {
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
      ZeroMemory(&si, sizeof(STARTUPINFO));
      si.cb = sizeof(STARTUPINFO);
      ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
#endif
    }
 
    process_impl::~process_impl() {
      /* Inform listener */
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
      LPDWORD exit_code = 0;

      if (GetExitCodeProcess(pi.hProcess, exit_code) || *exit_code == STILL_ACTIVE) {
        terminate();
      }
#else
      if (identifier) {
        terminate();
      }
#endif
    }
 
    void process_impl::terminate() {
      /* Inform monitor */
      boost::shared_ptr < task_monitor > l = monitor.lock();
      
      if (l.get() != 0) {
        l->disconnect(interface_pointer);
      }

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
      TerminateProcess(pi.hProcess, 1);

      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
#else
      kill(identifier, SIGKILL);
#endif
    }
 
    inline pid_t process_impl::get_identifier() const {
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
      return (static_cast < pid_t > (pi.dwProcessId));
#else
      return (identifier);
#endif
    }

    void process_impl::termination_handler(pid_t identifier) {
      boost::shared_ptr < process_impl > guard(interface_pointer->impl);

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
      LPDWORD exit_code = 0;

      WaitForSingleObject(pi.hProcess, INFINITE);

      if (GetExitCodeProcess(pi.hProcess, exit_code)) {
        current_status = (exit_code) ? process::completed : process::aborted;
      }
      else {
        current_status = process::aborted;
      }
#else
      int exit_code;

      waitpid(identifier, &exit_code, 0);

      current_status = (WIFEXITED(exit_code)) ? process::completed : process::aborted;
#endif

      signal_status();

      signal_termination(interface_pointer);
    }

    /** Signals the current state to the monitor */
    void process_impl::signal_status() const {
      boost::shared_ptr < task_monitor > l = monitor.lock();

      if (l.get() != 0) {
        l->signal_change(current_status);
      }
    }

    /**
     * \param[in] c the command to execute
     **/
    void process_impl::execute(const command& c) {
      m_command.reset(new command(c));

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
      LPTSTR command = _tcsdup(TEXT(c.as_string().c_str()));

      int identifier  = CreateProcess(0,command,0,0,false,0,0,c.working_directory.c_str(),&si,&pi);
#else
      boost::shared_array < char const* > arguments(c.get_array());

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

      current_status = (identifier < 0) ? process::aborted : process::running;

      if (current_status == process::running) {
        using namespace boost;

        signal_status();

        /* Wait for the process to terminate */
        thread t(bind(&process_impl::termination_handler, this, identifier));
      }
      else {
        signal_status();

        signal_termination(interface_pointer);
      }
    }
 
    /**
     * \param[in] h the function to call when the process terminates
     **/
    process::process(handler h) : impl(new process_impl(h)) {
      impl->interface_pointer = this;
    }
 
    /**
     * \param[in] h the function to call when the process terminates
     * \param[in] l a reference to a listener for process status change events
     **/
    process::process(handler h, boost::shared_ptr < task_monitor >& l) : impl(new process_impl(h, l)) {
      impl->interface_pointer = this;
    }
 
    pid_t process::get_identifier() const {
      return (impl->get_identifier());
    }

    void process::execute(command const& c) {
      impl->execute(c);
    }

    process::status process::get_status() const {
      if (impl.get() == 0) {
        return (stopped);
      }

      return (impl->current_status);
    }

    std::string process::get_executable_name() const {
      assert(impl.get() != 0);

      return (impl->m_command->get_executable());
    }

    command const& process::get_command() const {
      return (*impl->m_command);
    }

    bool process::terminate() {
      if (impl.get() != 0) {
        impl->terminate();

        return (true);
      }

      return (false);
    }
  }
}

#endif

