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

    class process_impl;

    /**
     * \brief Represents a system process with a status reporting facility
     **/
    class process {
      friend class executor;
      friend class process_impl;

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

        /** \brief Convenience type for handlers */
        typedef boost::function < void (process*) >    handler;

      private:

        /** \brief Pointer to implementation object (handle-body idiom) */
        boost::shared_ptr < process_impl >             impl;

      public:
    
        /** \brief Constructor */
        process(handler);
    
        /** \brief Constructor with listener */
        process(handler, boost::shared_ptr < task_monitor >&);
    
        /** \brief Start the process by executing a command */
        void execute(const command&);
     
        /** \brief Returns the process status */
        status get_status() const;

        /** \brief Returns the process id */
        pid_t get_identifier() const;
 
        /** \brief Returns the process id */
        std::string get_executable_name() const;

        /** \brief Terminates the process */
        bool terminate();
 
        /** \brief Returns the last command that is (or was) executing */
        command const& get_command() const;
    };
  }
}
  
#endif
