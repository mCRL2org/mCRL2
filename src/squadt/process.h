#ifndef PROCESS_H
#define PROCESS_H

#include <cstdio>

#include <boost/thread/thread.hpp>
#include <boost/function.hpp>
#include <boost/weak_ptr.hpp>

#include "exception.h"
#include "command.h"

namespace squadt {
  namespace execution {

    class task_monitor;

    /**
     * \brief Represents a system process with a status reporting facility
     **/
    class process {
      friend class executor;

      public:
         /** \brief state of the process */
         enum status {
           stopped,   ///< stopped or not yet started to run
           running,   ///< is running
           completed, ///< has terminated successfully 
           aborted    ///< was aborted
         };
 
         /** \brief Convenience type for weak pointer implementation */
         typedef boost::weak_ptr < process >            wptr;

         /** \brief Convenience type to hide shared pointer implementation */
         typedef boost::shared_ptr < process >          sptr;

         /** \brief Convenience type to hide shared pointer implementation */
         typedef boost::shared_ptr < process >          ptr;

         /** \brief Convenience type to hide shared pointer implementation */
         typedef boost::shared_ptr < task_monitor >     task_monitor_ptr;

         /** \brief Convenience type for handlers */
         typedef boost::function < void (process*) >    handler;

      private:

        /** \brief The default listener for changes in status */
        static boost::shared_ptr < task_monitor >       default_monitor;
 
      private:

        /** \brief The system's proces identifier for this process */
        pid_t                                identifier;

        /** \brief The status of this process */
        mutable status                       current_status;

        /** \brief The function that is called when the status changes */
        handler                              signal_termination;
    
        /** \brief A reference to a monitor for this process */
        boost::weak_ptr < task_monitor >     monitor;

        /** \brief Thread in which actual execution and waiting is performed */
        boost::shared_ptr < boost::thread >  execution_thread;

        /** \brief The command that is currently being exected (or 0) */
        std::auto_ptr < command >            last_command;

      private:

        /** \brief This method does the actual execution and waiting */
        void operator() (const command&);

      public:
    
        /** \brief Constructor */
        inline process(handler);
    
        /** \brief Constructor with listener */
        inline process(handler, task_monitor_ptr&);
    
        /** \brief Start the process by executing a command */
        void execute(const command&);
     
        /** \brief Returns the process status */
        inline status get_status() const;

        /** \brief Returns the process id */
        inline pid_t get_identifier() const;
 
        /** \brief Returns the process id */
        inline std::string get_executable_name() const;

        /** \brief Returns the last command that is (or was) executing */
        inline command const& get_command() const;
 
        /** \brief Terminates the process */
        void terminate();
 
        /** \brief Wait for the process to terminate */
        void wait() const;
 
        /** \brief Destructor */
        inline ~process();
    };
 
    /**
     * @param h the function to call when the process terminates
     **/
    inline process::process(handler h) : current_status(stopped), signal_termination(h), monitor(default_monitor) {
    }
 
    /**
     * @param h the function to call when the process terminates
     * @param l a reference to a listener for process status change events
     **/
    inline process::process(handler h, task_monitor_ptr& l) : current_status(stopped), signal_termination(h), monitor(l) {
    }
 
    inline process::~process() {
      /* Inform listener */
      if (identifier) {
        terminate();
      }
    }
 
    inline process::status process::get_status() const {
      return (current_status);
    }

    inline pid_t process::get_identifier() const {
      return (identifier);
    }

    inline std::string process::get_executable_name() const {
      return (last_command->get_executable_name());
    }

    inline command const& process::get_command() const {
      return (*last_command);
    }
  }
}
  
#endif
