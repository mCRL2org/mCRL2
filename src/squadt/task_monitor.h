#ifndef TASK_MONITOR_H
#define TASK_MONITOR_H

#include <map>

#include <boost/shared_ptr.hpp>

#include <sip/controller.h>
#include <sip/detail/common.h>

#include "process.h"

namespace squadt {

  class tool_manager_impl;
 
  namespace execution {

    class task_monitor_impl;

    /**
     * \brief Base class for task_monitors that provide tool execution via a tool manager
     **/
    class task_monitor : public sip::controller::communicator {
      friend class processor;
      friend class process_impl;
      friend class executor_impl;
      friend class squadt::tool_manager_impl;

      public:
 
        /** \brief Convenience type for hiding shared pointer implementation */
        typedef boost::shared_ptr < task_monitor > sptr;

      protected:
 
        /** \brief Waits until a connection has been established with the running process */
        void await_connection(unsigned int const&);

        /** \brief Waits until a connection has been established with the running process */
        void await_connection();

        /** \brief Signals that a new connection has been established */
        void signal_connection(sip::message::end_point);

        /** \brief Checks the process status and removes */
        virtual void signal_change(const execution::process::status);

      private:
 
        /** \brief Associates a process with this listener */
        void attach_process(const process::sptr& p);

        /** \brief Blocks until the process has registered */
        void await_process() const;

      public:

        /** \brief Constructor */
        task_monitor();
 
        /** \brief Gets a pointer to the associated process */
        process::sptr get_process(const bool b = false) const;

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

        /** \brief Executes a handler function once status change */
        void on_status_change(boost::function < void () >);

        /** \brief Whether there still exists a connection with the tool */
        bool is_connected() const;

        /** \brief Whether the tool is still busy performing its task */
        bool is_busy() const;

        /** \brief Disconnects from a running process (or make sure no connection exists) */
        void disconnect(execution::process*);

        /** \brief Unblocks waiters and requests a tool to prepare termination */
        void finish();

        /** \brief Terminates running processes and deactivates monitor */
        void shutdown();

        /** \brief Destructor */
        virtual ~task_monitor();
    };

    inline task_monitor::~task_monitor() {
      terminate_process();
    }
  }
}

#endif
