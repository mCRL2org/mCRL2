// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file process.cpp
/// \brief Add your file description here.

#include "boost.hpp" // precompiled headers

#include <cstdlib>
#include <csignal>
#include <cstring>
#include <iostream>

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
# include <windows.h>
# undef __in_range // for STLport
# include <workarounds.h>
# include <tchar.h>
#else
# if defined(__CYGWIN__)
#  define _POSIX_SOURCE
# endif
# include <unistd.h>
# include <sys/wait.h>
# if defined(__APPLE__)
#  include "boost/scoped_array.hpp"
#  include <Carbon/Carbon.h>
#  include <ApplicationServices/ApplicationServices.h>
# endif
#endif

#undef barrier

#include "boost/bind.hpp"
#include "boost/format.hpp"
#include "boost/foreach.hpp"
#include "boost/thread/thread.hpp"
#include "boost/filesystem/convenience.hpp"

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
        void create_process(command const& c, boost::function <  void (process::status) > h);

        /** \brief Waits for a process to terminate */
        void await_termination(boost::weak_ptr < process >, process_impl::information&, boost::function <  void (process::status) > h);

        /** \brief Executed at process termination */
        void termination_handler(boost::weak_ptr < process >, process::status);

        /** \brief Executed at process termination */
        void termination_handler(boost::weak_ptr < process >, process::termination_handler, process::status);

      public:

        /** \brief Constructor with listener */
        process_impl(boost::shared_ptr < process >&);

        /** \brief Start the process by executing a command */
        void execute(const command&);

        /** \brief Start the process by executing a command */
        void execute(const command&, process::termination_handler const& h);

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
     * \param[in] pi the process information structure
     * \param[in] h functor that is invoked when the process ends (if the process object is still alive)
     *
     * \pre p.lock().get() = m_interface
     **/
    inline void process_impl::await_termination(boost::weak_ptr < process > p, process_impl::information& pi, boost::function <  void (process::status) > h) {
      HANDLE& process_handle(m_information.process.hProcess);

      if (process_handle != 0) {
        DWORD exit_code;

        WaitForSingleObject(process_handle, INFINITE);

        h((GetExitCodeProcess(process_handle, &exit_code) && exit_code == 0) ? process::completed : process::aborted);
      }
      else {
        h(process::aborted);
      }
    }

    /**
     * \param[in] c the command to execute
     **/
    inline void process_impl::create_process(command const& c, boost::function < void (process::status) > h) {
      LPTSTR command = _tcsdup(TEXT(c.string().c_str()));

      int identifier  = CreateProcess(0,command,0,0,false,CREATE_NO_WINDOW,0,c.working_directory.string().c_str(),&m_information.startup,&m_information.process);

      m_status = (identifier < 0) ? process::aborted : process::running;

      if (m_status != process::running) {
        h(m_status);
      }
      else {
        boost::thread(boost::bind(&process_impl::await_termination, this, m_interface, m_information, h));
      }
    }
#else
    /**
     * \param[in] p pointer to the process interface object
     * \param[in] pi the process information structure
     * \param[in] h functor that is invoked when the process ends (if the process object is still alive)
     *
     * \pre p.lock().get() = m_interface
     **/
    void process_impl::await_termination(boost::weak_ptr < process > p, process_impl::information& pi, boost::function <  void (process::status) > h) {
      int exit_code;

      waitpid(pi.process_identifier, &exit_code, 0);

      h((WIFEXITED(exit_code)) ? process::completed : process::aborted);
    }

#if defined(__APPLE__)
    class smaller {
      public:

        bool operator() (ProcessSerialNumber const& l, ProcessSerialNumber const& r) const {
          return l.highLongOfPSN < r.highLongOfPSN || (l.highLongOfPSN == r.highLongOfPSN && l.lowLongOfPSN < r.lowLongOfPSN);
        }
    };
#endif

    /**
     * \param[in] c command to execute
     **/
    void process_impl::create_process(command const& c, boost::function < void (process::status) > h) {
#if defined(__APPLE__)
      using namespace boost::filesystem;

      static std::map < ProcessSerialNumber, boost::function < void (process::status) >, smaller > termination_handlers;

      struct local {
        inline static std::string as_string(const CFStringRef s) {
          size_t length = CFStringGetLength(s) + 1;
          boost::scoped_array< char > buffer(new char[length]);

          if (!CFStringGetCString(s, buffer.get(), length, kCFStringEncodingASCII)) {
            throw false;
          }

          return std::string(buffer.get());
        }

        static OSStatus termination_handler(EventHandlerCallRef, EventRef e, void* d) {
          ProcessSerialNumber psn;
          EventParamType      data_type;
          UInt32              data_size;

          if (GetEventParameter(e, kEventParamProcessID, typeProcessSerialNumber,
                                        &data_type, sizeof(ProcessSerialNumber), &data_size, &psn) == noErr) {

            if (data_type == typeProcessSerialNumber && data_size == sizeof(ProcessSerialNumber)) {
              if (0 < termination_handlers.count(psn)) {
                boost::function < void (process::status) > handler(termination_handlers[psn]);

                termination_handlers.erase(psn);

                handler(process::completed);
              }
            }
          }

          return noErr;
        }

        static bool install_termination_handler() {
          EventTypeSpec event_descriptors[1];

          event_descriptors[0].eventClass = kEventClassApplication;
          event_descriptors[0].eventKind  = kEventAppTerminated;

          EventHandlerUPP upp = NewEventHandlerUPP(&local::termination_handler);

          if (upp) {
            if (InstallApplicationEventHandler(upp, 1, event_descriptors, 0, 0) == noErr) {
              return true;
            }

            DisposeEventHandlerUPP(upp);
          }

          return false;
        }
      };

      static bool termination_handling = local::install_termination_handler();

      // Start application bundle
      if (is_directory(c.executable) && extension(c.executable).compare(".app") == 0) {
        CFURLRef bundle_url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
            CFStringCreateWithCString(kCFAllocatorDefault, c.executable.c_str(), kCFStringEncodingASCII),
                                                                                        kCFURLPOSIXPathStyle, true);

        CFBundleRef bundle  = CFBundleCreate(kCFAllocatorDefault, bundle_url);

        // Assume that bundle is unusable
        m_status = process::aborted;

        if (bundle) {
          UInt32 bundle_type, bundle_creator;

          CFBundleGetPackageInfo(bundle, &bundle_type, &bundle_creator);

          // Check to see if bundle is indeed an application bundle
          if(bundle_type == 'APPL') {
            CFMutableArrayRef bundle_arguments = CFArrayCreateMutable(kCFAllocatorDefault,
                                                        c.get_arguments().size(), &kCFTypeArrayCallBacks);

            if (bundle_arguments) {
              command::const_argument_sequence arguments(c.get_arguments());

              BOOST_FOREACH(command::argument_sequence::value_type a, arguments) {
                CFArrayAppendValue(bundle_arguments, CFStringCreateWithCString(kCFAllocatorDefault, a.c_str(), kCFStringEncodingASCII));
              }

              CFURLRef executable_url = CFBundleCopyExecutableURL(bundle);

              if (executable_url) {
                FSRef executable_path;

                if (FSPathMakeRef(reinterpret_cast < const UInt8* > ((path(c.executable) / std::string("Contents/MacOS") / local::as_string(CFURLGetString(executable_url))).string().c_str()), &executable_path, 0) == noErr) {
                  std::string unparsed_command_line_arguments(c.string());

                  AppleEvent          initialEvent;
                  ProcessSerialNumber serial_number;

                  LSApplicationParameters bundle_parameters;
                       bundle_parameters.version           = 0;
                       bundle_parameters.flags             = kLSLaunchNewInstance;
                       bundle_parameters.application       = &executable_path;
                       bundle_parameters.asyncLaunchRefCon = 0;
                       bundle_parameters.environment       = 0;
                       bundle_parameters.argv              = 0;
                       bundle_parameters.initialEvent      = &initialEvent;

                  AEAddressDesc target_descriptor;

                  AECreateDesc(typeProcessSerialNumber, &serial_number, sizeof(ProcessSerialNumber), &target_descriptor);

                  AECreateAppleEvent(kCoreEventClass, kAEOpenApplication, &target_descriptor, kAutoGenerateReturnID, kAnyTransactionID, bundle_parameters.initialEvent);

                  // Apple event for communicating argv
                  AEPutParamPtr(bundle_parameters.initialEvent, keyDirectObject, typeChar, unparsed_command_line_arguments.c_str(), unparsed_command_line_arguments.size());

                  if (LSOpenApplication(&bundle_parameters, &serial_number) == noErr) {
                    GetProcessPID(&serial_number, &m_information.process_identifier);

                    // Register termination handler
                    if (termination_handling) {
                      termination_handlers[serial_number] = h;
                    }

                    m_status = process::running;
                  }
                }

                CFRelease(executable_url);
              }

              CFRelease(bundle_arguments);
            }
          }

          CFRelease(bundle);
        }

        CFRelease(bundle_url);

        if (m_status != process::running) {
          h(m_status);
        }

        return;
      }
#endif
      boost::shared_array < char const* > arguments(c.get_array());

      m_information.process_identifier = fork();

      if (m_information.process_identifier == 0) {
        /* Change working directory to the project directory */
        chdir(c.working_directory.string().c_str());

        execvp(c.executable.c_str(), const_cast < char* const* > (arguments.get()));

        std::cerr << boost::str(boost::format("Execution failed : `%s' %s\n") % c.executable % strerror(errno));

        _exit(1);
      }

      m_status = (m_information.process_identifier < 0) ? process::aborted : process::running;

      if (m_status != process::running) {
        h(m_status);
      }
      else {
        boost::thread(boost::bind(&process_impl::await_termination, this, m_interface, m_information, h));
      }
    }

    void process_impl::terminate() {
      if (m_status == process::running) {
        kill(m_information.process_identifier, SIGHUP);

        m_status = process::aborted;

        boost::this_thread::sleep(boost::posix_time::seconds(2));

        kill(m_information.process_identifier, SIGKILL);
      }
    }
#endif

    /**
     * \param[in] p pointer to the process interface object
     * \param[in] s the new status of the process
     * \pre p.lock().get() = m_interface
     **/
    void process_impl::termination_handler(boost::weak_ptr < process > p, process::status s) {
      boost::shared_ptr< process > alive(p.lock());

      if (alive) {
        m_status = s;
      }
    }

    /**
     * \param[in] p pointer to the process interface object
     * \param[in] h the next handler
     * \param[in] s the new status of the process
     * \pre p.lock().get() = m_interface
     **/
    void process_impl::termination_handler(boost::weak_ptr < process > p, process::termination_handler h, process::status s) {
      boost::shared_ptr< process > alive(p.lock());

      if (alive) {
        m_status = s;

        h(alive);
      }
    }

    /**
     * \param[in] c the command to execute
     **/
    void process_impl::execute(const command& c) {
      m_command.reset(new command(c));

      create_process(c, boost::bind(&process_impl::termination_handler, this, m_interface, _1));
    }

    /**
     * \param[in] c the command to execute
     * \param[in] h handler function to execute when the process terminates
     **/
    void process_impl::execute(const command& c, process::termination_handler const& h) {
      m_command.reset(new command(c));

      create_process(c, boost::bind(&process_impl::termination_handler, this, m_interface, h, _1));
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
      if(!(impl.get() != 0)){
         throw std::runtime_error( "Empty implemention\n" );
      };

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

