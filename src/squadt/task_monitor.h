#ifndef TASK_MONITOR_H
#define TASK_MONITOR_H

#include <map>

#include <boost/shared_ptr.hpp>

#include <sip/controller.h>
#include <sip/detail/basic_messenger.tcc>

#include "process.h"

namespace squadt {

  class tool_manager;
 
  namespace execution {

    class task_monitor_impl;

    /**
     * \brief Base class for task_monitors that provide tool execution via a tool manager
     **/
    class task_monitor : public sip::controller::communicator {
      friend class process;
      friend class executor;
      friend class squadt::tool_manager;

      public:
 
        /** \brief Convenience type for hiding shared pointer implementation */
        typedef boost::shared_ptr < task_monitor > sptr;

      protected:

        /** \brief The state of this object */
        boost::shared_ptr < task_monitor_impl > m_state;
 
      protected:
 
        /** \brief Waits until a connection has been established with the running process */
        void await_connection();

        /** \brief Waits until a connection has been established with the running process */
        void await_completion();

        /** \brief Unblocks waiters and requests a tool to prepare termination */
        void finish();
 
      private:
 
        /** \brief Associates a process with this listener */
        void attach_process(const process::ptr& p);

        /** \brief Blocks until the process has registered */
        void await_process() const;

        /** \brief Signals that a new connection has been established */
        void signal_connection(const sip::end_point&);

        /** \brief Checks the process status and removes */
        void signal_change(const execution::process::status);

      public:

        /** \brief Constructor */
        task_monitor();
 
        /** \brief Gets a pointer to the associated process */
        process::ptr get_process(const bool b = false) const;

        /** \brief Terminate an associated process */
        void terminate_process();

        /** \brief Executes a handler function once the tool has connected */
        void on_connection(boost::function < void () >);

        /** \brief Executes a handler function once the tool has connected */
        void once_on_connection(boost::function < void () >);

        /** \brief Executes a handler function once the tool has connected */
        void on_completion(boost::function < void () >);

        /** \brief Executes a handler function once on tool completion */
        void once_on_completion(boost::function < void () >);

        /** \brief Whether there still exists a connection with the tool */
        bool is_connected() const;

        /** \brief Whether the tool is still busy performing its task */
        bool is_busy() const;

        /** \brief Disconnects from a running process (or make sure not connection exists) */
        void disconnect(execution::process*);

        /** \brief Destructor */
        virtual ~task_monitor();
    };

    inline task_monitor::~task_monitor() {
      terminate_process();
    }
  }
}

#endif
