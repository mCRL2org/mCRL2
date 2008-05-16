// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file task_monitor.h
/// \brief Add your file description here.

#ifndef TASK_MONITOR_H
#define TASK_MONITOR_H

#include <map>

#include <boost/shared_ptr.hpp>

#include "tipi/controller.hpp"
#include "tipi/common.hpp"

#include "process.hpp"

namespace squadt {

  class tool_manager_impl;
 
  namespace execution {

    class task_monitor_impl;

    /**
     * \brief Base class for task_monitors that provide tool execution via a tool manager
     **/
    class task_monitor : public tipi::controller::communicator {
      friend class processor;
      friend class process_impl;
      friend class executor_impl;
      friend class squadt::tool_manager_impl;

      public:
 
        /** \brief Convenience type for hiding shared pointer implementation */
        typedef boost::shared_ptr < task_monitor > sptr;

      protected:
 
        /** \brief Waits until a connection has been established with the running process */
        bool await_connection(unsigned int const&);

        /** \brief Waits until a connection has been established with the running process */
        bool await_connection();

        /** \brief Waits until the current task has been completed */
        bool await_completion();

        /** \brief Signals that a new connection has been established */
        void signal_connection(tipi::message::end_point);

        /** \brief Checks the process status and removes */
        virtual void signal_change(boost::shared_ptr < execution::process > p, const execution::process::status);

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

        /** \brief Disconnects from a running process (or make sure no connection exists) */
        void disconnect(boost::weak_ptr < execution::process > const&);

        /** \brief Unblocks waiters and requests a tool to prepare termination */
        void finish();

        /** \brief Terminates running processes and deactivates monitor */
        void shutdown();

        /** \brief Destructor */
        virtual ~task_monitor();
    };

    inline task_monitor::~task_monitor() {
      finish();
    }
  }
}

#endif
