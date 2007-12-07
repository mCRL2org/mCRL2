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
# if defined(__APPLE__)
#  include <CoreFoundation/CoreFoundation.h>
#  include <ApplicationServices/ApplicationServices.h>
# endif
#endif

#undef barrier

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/thread/thread.hpp>

#include "command.hpp"
#include "process.hpp"

namespace squadt {
  namespace execution {

    class process_impl {
      friend class process;

      private:
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
        class information {
          friend class process_impl;

          private:

            STARTUPINFO         startup;
            PROCESS_INFORMATION process;

          public:

            inline information() {
              ZeroMemory(&startup, sizeof(STARTUPINFO));
              startup.cb = sizeof(STARTUPINFO);
              ZeroMemory(&process, sizeof(PROCESS_INFORMATION));
            }

            inline pid_t get_identifier() const {
              return (static_cast < pid_t > (process.dwProcessId));
            }
        };
#else
        class information {
          friend class process_impl;

          private:

            /** \brief The system's proces identifier for this process */
            pid_t process_identifier;

          public:

            inline information() : process_identifier(0) {
            }

            /** \brief Gets the process identifier */
            inline pid_t get_identifier() const {
              return process_identifier;
            }
        };
#endif

      private:

        /** \brief The status of this process */
        process::status                      m_status;

        /** \brief The command that is currently being executed (or 0) */
        std::auto_ptr < command >            m_command;

        /** \brief The process object that serves as interface to this object */
        boost::weak_ptr < process >          m_interface;

        /** \brief Platform dependent process information */
        process_impl::information            m_information;

      private:

        /** \brief Creates the new process */
        void create_process(command const& c);

        /** \brief Executed at process termination */
        void await_termination(boost::weak_ptr < process >, process_impl::information&);

        /** \brief Executed at process termination */
        void await_termination(boost::weak_ptr < process >, process::termination_handler, process_impl::information&);

      public:

        /** \brief Constructor with listener */
        process_impl(boost::shared_ptr < process >&);
 
        /** \brief Start the process by executing a command */
        void execute(const command&);
     
        /** \brief Start the process by executing a command */
        void execute(const command&, process::termination_handler h);
     
        /** \brief Returns the process id */
        pid_t get_identifier() const;
 
        /** \brief Terminates the process */
        void terminate();
 
        /** \brief Destructor */
        ~process_impl();
    };

    /**
     * \param[in] h the function to call when the process terminates
     * \param[in] l a reference to a listener for process status change events
     **/
    process_impl::process_impl(boost::shared_ptr < process >& p) :
        m_status(process::stopped), m_interface(p) {
    }
 
    process_impl::~process_impl() {
      /* Inform listener */
      if (m_status != process::running && m_status != process::stopped) {
        terminate();
      }
    }
 
    inline pid_t process_impl::get_identifier() const {
      return m_information.get_identifier();
    }

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
    void process_impl::terminate() {
      HANDLE& process_handle(m_information.process.hProcess);

      if (process_handle != 0) {
        DWORD exit_code = 0;

        if (::GetExitCodeProcess(process_handle, &exit_code)) {
          ::PostThreadMessage(m_information.process.dwThreadId, WM_CLOSE, 0, 0);

          if (::WaitForSingleObject(process_handle, 1000) == WAIT_TIMEOUT) {
            TerminateProcess(process_handle, 1);
          }

          CloseHandle(m_information.process.hThread);
          CloseHandle(process_handle);
        }
      }
    }

    /**
     * \param[in] p pointer to the process interface object
     * \param[in] h functor that is invoked when the process ends (if the process object is still alive)
     * \param[in,out] ph the process information object
     *
     * \pre p.lock().get() = m_interface
     **/
    inline void process_impl::await_termination(boost::weak_ptr < process > p, process_impl::information& pi) {
      HANDLE& process_handle(m_information.process.hProcess);

      if (process_handle != 0) {
        DWORD exit_code;
    
        WaitForSingleObject(process_handle, INFINITE);
    
        boost::shared_ptr < process > alive(p.lock());

        if (alive) {
          m_status = (GetExitCodeProcess(process_handle, &exit_code) && exit_code == 0) ? process::completed : process::aborted;
        }
      }
    }

    /**
     * \param[in] c the command to execute
     **/
    inline void process_impl::create_process(command const& c) {
      LPTSTR command = _tcsdup(TEXT(c.as_string().c_str()));

      int identifier  = CreateProcess(0,command,0,0,false,CREATE_NO_WINDOW,0,c.working_directory.c_str(),&m_information.startup,&m_information.process);

      m_status = (identifier < 0) ? process::aborted : process::running;
    }
#else
    /**
     * \param[in] p pointer to the process interface object
     * \param[in] h functor that is invoked when the process ends (if the process object is still alive)
     * \param[in,out] ph the process information object
     *
     * \pre p.lock().get() = m_interface
     **/
    void process_impl::await_termination(boost::weak_ptr < process > p, process_impl::information& pi) {
      int exit_code;

      waitpid(pi.process_identifier, &exit_code, 0);

      boost::shared_ptr < process > alive(p.lock());

      if (alive.get()) {
        m_status = (WIFEXITED(exit_code)) ? process::completed : process::aborted;
      }
    }

    inline void process_impl::create_process(command const& c) {
      boost::shared_array < char const* > arguments(c.get_array());

      m_information.process_identifier = fork();

      if (m_information.process_identifier == 0) {
        /* Change working directory to the project directory */
        chdir(c.working_directory.c_str());

        execvp(c.executable.c_str(), const_cast < char* const* > (arguments.get()));

        std::cerr << boost::str(boost::format("Execution failed : `%s' %s\n") % c.executable % strerror(errno));

        _exit(1);
      }

      m_status = (m_information.process_identifier < 0) ? process::aborted : process::running;
    }

    void process_impl::terminate() {
      if (m_status == process::running) {
        kill(m_information.process_identifier, SIGHUP);

        m_status = process::aborted;

        boost::timed_mutex m;

        boost::xtime time;

        xtime_get(&time, boost::TIME_UTC);

        time.sec += 2;

        boost::timed_mutex::scoped_timed_lock(m, time);

        kill(m_information.process_identifier, SIGKILL);
      }
    }
#endif

    /**
     * \param[in] p pointer to the process interface object
     * \pre p.lock().get() = m_interface
     **/
    void process_impl::await_termination(boost::weak_ptr < process > p, process::termination_handler h, process_impl::information& pi) {
      await_termination(p, pi);

      boost::shared_ptr< process > alive(p.lock());

      if (alive) {
        h(alive);
      }
    }

    /**
     * \param[in] c the command to execute
     **/
    void process_impl::execute(const command& c) {
      m_command.reset(new command(c));

      create_process(c);

      if (m_status == process::running) {
        /* Wait for the process to terminate */
        boost::thread t(bind(&process_impl::await_termination, this, m_interface, m_information));
      }
    }

    /**
     * \param[in] c the command to execute
     **/
    void process_impl::execute(const command& c, process::termination_handler h) {
      m_command.reset(new command(c));

      create_process(c);

      if (m_status == process::running) {
        /* Wait for the process to terminate */
        boost::thread t(bind(&process_impl::await_termination, this, m_interface, h, m_information));
      }
      else {
        h(m_interface.lock());
      }
    }
 
    process::process() {
    }

    boost::shared_ptr < process > process::create() {
      boost::shared_ptr < process > p(new process());

      p->impl.reset(new process_impl(p));

      return p;
    }
 
    pid_t process::get_identifier() const {
      return (impl->get_identifier());
    }

    void process::execute(command const& c) {
      impl->execute(c);
    }

    void process::execute(command const& c, process::termination_handler const& h) {
      impl->execute(c, h);
    }

    process::status process::get_status() const {
      if (impl.get() == 0) {
        return (stopped);
      }

      return (impl->m_status);
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

