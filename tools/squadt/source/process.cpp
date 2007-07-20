// Author(s): Jeroen van der Wulp
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file process.cpp
/// \brief Add your file description here.

#ifndef PROCESS_TCC
#define PROCESS_TCC

#include <cstdlib>
#include <csignal>
#include <cstring>
#include <cerrno>

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
# include <windows.h>
# include <workarounds.h>
# include <tchar.h>
#else
# if defined(__CYGWIN__)
#  define _POSIX_SOURCE
# endif
# include <unistd.h>
# include <sys/wait.h>
#endif

#undef barrier

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/thread/thread.hpp>

#include "task_monitor.hpp"
#include "command.hpp"
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
        boost::weak_ptr < process >          interface_pointer;

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
        STARTUPINFO         si;
        PROCESS_INFORMATION pi;
#else
        /** \brief The system's proces identifier for this process */
        pid_t               identifier;
#endif

      private:

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
        /** \brief Executed at process termination */
        static void termination_handler(boost::weak_ptr < process >, HANDLE&);
#else
        /** \brief Executed at process termination */
        static void termination_handler(boost::weak_ptr < process >, pid_t);
#endif

        /** \brief Initialisation procedure */
        void initialise();
 
      public:

        /** \brief Constructor with listener */
        process_impl(boost::shared_ptr < process >&, process::handler, boost::shared_ptr < task_monitor >&);
 
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

    /**
     * \param[in] h the function to call when the process terminates
     * \param[in] l a reference to a listener for process status change events
     **/
    process_impl::process_impl(boost::shared_ptr < process >& p, process::handler h, boost::shared_ptr < task_monitor >& l) :
        current_status(process::stopped), signal_termination(h), monitor(l), interface_pointer(p) {

      initialise();
    }

    inline void process_impl::initialise() {
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
      ZeroMemory(&si, sizeof(STARTUPINFO));
      si.cb = sizeof(STARTUPINFO);
      ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
#else
      identifier = 0;
#endif
    }
 
    process_impl::~process_impl() {
      /* Inform listener */
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
      if (current_status != process::running && current_status != process::stopped) {
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
      if (pi.hProcess != 0) {
        DWORD exit_code = 0;

        if (::GetExitCodeProcess(pi.hProcess, &exit_code)) {
          ::PostThreadMessage(pi.dwThreadId, WM_CLOSE, 0, 0);

          if (::WaitForSingleObject(pi.hProcess, 1000) == WAIT_TIMEOUT) {
            TerminateProcess(pi.hProcess, 1);
          }

          CloseHandle(pi.hThread);
          CloseHandle(pi.hProcess);
        }
      }
#else
      kill(identifier, SIGKILL);
#endif
    }
 
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
    inline pid_t process_impl::get_identifier() const {
      return (static_cast < pid_t > (pi.dwProcessId));
    }

    /**
     * \param[in,out] ph the process handle
     **/
    void process_impl::termination_handler(boost::weak_ptr < process > p, HANDLE ph) {
      if (ph != 0) {
        DWORD exit_code;
    
        WaitForSingleObject(ph, INFINITE);
    
        boost::shared_ptr < process > alive(p.lock());

        if (alive.get())  {
          boost::shared_ptr < process_impl >& impl(alive->impl);

          impl->current_status = (GetExitCodeProcess(ph, &exit_code) && exit_code == 0) ? process::completed : process::aborted;

          impl->signal_status();
       
          impl->signal_termination(alive.get());
        }
      }
    }
#else
    inline pid_t process_impl::get_identifier() const {
      return (identifier);
    }

    void process_impl::termination_handler(boost::weak_ptr < process > p, pid_t identifier) {
      int exit_code;

      waitpid(identifier, &exit_code, 0);

      boost::shared_ptr < process > alive(p.lock());

      if (alive.get())  {
        boost::shared_ptr < process_impl >& impl(alive->impl);

        impl->current_status = (WIFEXITED(exit_code)) ? process::completed : process::aborted;

        impl->signal_status();

        impl->signal_termination(alive.get());
      }
    }
#endif

    /** \brief Signals the current state to the monitor */
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

      int identifier  = CreateProcess(0,command,0,0,false,CREATE_NO_WINDOW,0,c.working_directory.c_str(),&si,&pi);
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

      signal_status();

      if (current_status == process::running) {
        using namespace boost;

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
        /* Wait for the process to terminate */
        thread t(bind(&process_impl::termination_handler, interface_pointer, pi.hProcess));
#else
        thread t(bind(&process_impl::termination_handler, interface_pointer, identifier));
#endif
      }
      else {
        signal_termination(interface_pointer.lock().get());
      }
    }
 
    /**
     * \param[in] h the function to call when the process terminates
     **/
    process::process(handler h) {
    }
 
    /**
     * \param[in] h the function to call when the process terminates
     * \param[in] l a reference to a listener for process status change events
     **/
    process::process(handler h, boost::shared_ptr < task_monitor >& l) {
    }

    boost::shared_ptr < task_monitor > default_monitor;

    /**
     * \param[in] h the function to call when the process terminates
     **/
    boost::shared_ptr < process > process::create(handler h) {
      boost::shared_ptr < process > p(new process(h));

      p->impl.reset(new process_impl(p, h, default_monitor));

      return p;
    }
 
    /**
     * \param[in] h the function to call when the process terminates
     * \param[in] l a reference to a listener for process status change events
     **/
    boost::shared_ptr < process > process::create(handler h, boost::shared_ptr < task_monitor >& l) {
      boost::shared_ptr < process > p(new process(h, l));

      p->impl.reset(new process_impl(p, h, l));

      return p;
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

      if (impl->m_command.get()) {
        return impl->m_command->get_executable();
      }
      else {
        return "unregistered";
      }
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

