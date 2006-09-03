#ifndef TOOL_EXECUTOR_H
#define TOOL_EXECUTOR_H

#include <boost/shared_ptr.hpp>

#include "task_monitor.h"

namespace squadt {
  namespace execution {

    class command;

    class executor_impl;

    /**
     * \brief Basic process execution component
     *
     * Design choices:
     *  - only a predetermined fixed number of processes can is run concurrently,
     *    which means that sometimes execution is delayed
     *  - the execute method is non-blocking; consequently nothing can be
     *    concluded about the execution of the command except through a
     *    task_monitor object
     **/
    class executor {

      private:

        boost::shared_ptr < executor_impl > impl;
    
      private:
    
        /** \brief Start a new process */
        inline void start_process(const command&);
    
        /** \brief Start a new process with a listener */
        inline void start_process(const command&, task_monitor::sptr&);

        /** \brief Start processing commands if the queue contains any waiters */
        inline void start_delayed();
 
        /** \brief Remove a process from the list */
        inline void remove(process*);
    
      public:
    
        /** \brief Constructor */
        executor(unsigned int = 3);
 
        /** \brief Execute a tool */
        void execute(const command&, boost::shared_ptr < task_monitor >& = process::default_monitor, bool = false);
    
        /** \brief Terminate a specific process */
        inline void terminate(process*);
    
        /** \brief Terminate a specific process */
        inline void terminate(process::wptr);
    
        /** \brief Terminate all processes */
        void terminate_all();

        /** \brief Destructor */
        ~executor();
    };

    /**
     * @param[in] p a weak pointer (or reference to) to the process that should be terminated
     **/
    inline void executor::terminate(process* p) {
      p->terminate();
    }
 
    /**
     * @param[in] p a shared pointer (or reference to) to the process that should be terminated
     **/
    inline void executor::terminate(process::wptr p) {
      process::ptr w = p.lock();

      if (w.get() != 0) {
        w->terminate();
      }
    }
  }
}
  
#endif
