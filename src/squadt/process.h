#ifndef PROCESS_H
#define PROCESS_H

#include <cstdio>

#include <boost/thread/thread.hpp>
#include <boost/function.hpp>

#include "exception.h"
#include "command.h"

namespace squadt {
  namespace execution {

    class process_listener;

    /**
     * \brief Represents a system process with a status reporting facility
     **/
    class process {
      public:
         /** \brief state of the process */
         enum status {
           stopped,   ///< stopped or not yet started to run
           running,   ///< is running
           completed, ///< has terminated successfully 
           aborted    ///< was aborted
         };
 
         /** \brief Convenience type to hide shared pointer implementation */
         typedef boost::shared_ptr < process >        ptr;
 
         /** \brief Convenience type for handlers */
         typedef boost::function < void (process*) >  handler;

      private:

         /** \brief The default listener for changes in status */
         static process_listener default_listener;
 
      private:

        /** \brief The system's proces identifier for this process */
        long int                            identifier;

        /** \brief The status of this process */
        mutable status                      current_status;

        /** \brief The function that is called when the status changes */
        handler                             signal_termination;
    
        /** \brief A reference to a listener for changes status changes */
        process_listener&                   listener;

        /** \brief Thread in which actual execution and waiting is performed */
        boost::shared_ptr < boost::thread > execution_thread;

      private:

        /** \brief This method does the actual execution and waiting */
        void operator() (const command&);

      public:
    
        /** \brief Constructor */
        inline process(handler);
    
        /** \brief Constructor with listener */
        inline process(handler, process_listener&);
    
        /** \brief Start the process by executing a command */
        void execute(const command&);
     
        /** \brief Returns the process status */
        inline status get_status() const;
 
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
    inline process::process(handler h) : current_status(stopped), signal_termination(h), listener(default_listener) {
    }
 
    /**
     * @param h the function to call when the process terminates
     * @param l a reference to a listener for process status change events
     **/
    inline process::process(handler h, process_listener& l) : current_status(stopped), signal_termination(h), listener(l) {
    }
 
    inline process::~process() {
      if (identifier) {
        terminate();
      }
    }
 
    inline process::status process::get_status() const {
      return (current_status);
    }
  }
}
  
#endif
