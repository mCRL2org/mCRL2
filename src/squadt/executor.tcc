#include "executor.h"

namespace squadt {
  /// \cond PRIVATE_PART

  namespace execution {

    class executor_impl : public utility::visitable< executor_impl > {
      friend class executor;
      friend class restore_visitor_impl;
      friend class store_visitor_impl;

      private:

        /** Convenient short-hand type */
        typedef std::pair < command, task_monitor::sptr > command_pair;

      private:

        /** \brief The maximum number of processes that is allowed to run concurrently */
        unsigned int                   maximum_instance_count;
 
        /** \brief List of active processes */
        std::list < process::sptr >    processes;
 
        /** \brief Data of processes that will be started */
        std::deque < command_pair >    delayed_commands;

      private:

        /** \brief handler that is invoked when a process is terminated */
        inline void handle_process_termination(process* p, boost::weak_ptr < executor_impl >);
  
        /** \brief Start a new process */
        inline void start_process(const command&, boost::shared_ptr < executor_impl >&);
    
        /** \brief Start a new process with a listener */
        inline void start_process(const command&, task_monitor::sptr&, boost::shared_ptr < executor_impl >&);

        /** \brief Start processing commands if the queue contains any waiters */
        inline void start_delayed(boost::shared_ptr < executor_impl >&);
 
        /** \brief Remove a process from the list */
        inline void remove(process*);
  
      public:

        /** \brief Constructor */
        inline executor_impl(unsigned int);
 
        /** \brief Execute a tool */
        void execute(const command&, boost::shared_ptr < task_monitor >&, bool, boost::shared_ptr < executor_impl >&);
    
        /** \brief Execute a command */
        void execute(const command&, bool, boost::shared_ptr < executor_impl >&);

        /** \brief Maximum number of tool instances */
        size_t get_maximum_instance_count() const;
 
        /** \brief Maximum number of tool instances */
        void set_maximum_instance_count(size_t);
    
        /** \brief Terminate a specific process */
        inline void terminate(process*);
    
        /** \brief Terminate a specific process */
        inline void terminate(process::wptr);
    
        /** \brief Terminate all processes */
        void terminate_all();
    };
  }
  /// \endcond
}
