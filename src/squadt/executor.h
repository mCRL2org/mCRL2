#ifndef EXECUTOR_H__
#define EXECUTOR_H__

#include <boost/shared_ptr.hpp>

#include "task_monitor.h"

namespace squadt {
  class tool_manager;
  class build_system;

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
      friend class squadt::build_system;
      friend class squadt::tool_manager;

      private:

        /** \brief Smart pointer instance that references the associated implementation object */
        boost::shared_ptr < executor_impl > impl;
    
      private:
    
        /** \brief Start a new process */
        void start_process(const command&);
    
        /** \brief Start a new process with a listener */
        void start_process(const command&, task_monitor::sptr&);

        /** \brief Start processing commands if the queue contains any waiters */
        void start_delayed();
 
        /** \brief Remove a process from the list */
        void remove(process*);
    
      public:
    
        /** \brief Constructor */
        executor(unsigned int const& = 3);

        /** \brief Maximum number of tool instances */
        size_t get_maximum_instance_count() const;
 
        /** \brief Maximum number of tool instances */
        void set_maximum_instance_count(size_t);
 
        /** \brief Execute a tool */
        void execute(command const&, boost::shared_ptr < task_monitor >&, bool = false);

        /** \brief Execute a command */
        void execute(command const&, bool = false);
    
        /** \brief Terminate a specific process */
        void terminate(process*);
    
        /** \brief Terminate a specific process */
        void terminate(process::wptr);
    
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
      process::sptr w = p.lock();

      if (w.get() != 0) {
        w->terminate();
      }
    }
  }
}
  
#endif
