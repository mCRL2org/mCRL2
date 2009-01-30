// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file task_monitor.ipp
/// \brief Add your file description here.

#include "tipi/detail/controller.ipp"

#include "task_monitor.hpp"

namespace squadt {
  namespace execution {
    /// \cond INTERNAL_DOCS

    class task_monitor_impl : public tipi::controller::communicator_impl {
      friend class task_monitor;

      template < typename S, typename T >
      friend class utility::visitor;

      private:

        /** \brief Type for event distinction */
        enum event_type {
          change,     ///< Status of the associated process has changed
          connection, ///< A new connection has been established
          completion  ///< A tool completed its operation
        };

        /** \brief The type for a map that contains the event handlers */
        typedef std::multimap < const event_type, boost::function < bool () > >  handler_map;

      private:

        /** \brief Semaphore to guarantee mutual exclusion (for use with register_condition) */
        mutable boost::mutex                            register_lock;

        /** \brief Monitor for waiting until process has registered */
        boost::condition_variable                       register_condition;

        /** \brief Monitor for waiting until process has registered */
        boost::condition_variable                       connection_condition;

        /** \brief Monitor for waiting until process has registered */
        boost::condition_variable                       completion_condition;

        /** \brief A pointer to the process associated to this listener or 0 */
        boost::shared_ptr< process >                    associated_process;

        /** \brief The event handlers that have been registered */
        handler_map                                     handlers;

        using tipi::messaging::basic_messenger_impl< tipi::message >::disconnect;

      private:

        /** \brief Constructor */
        inline task_monitor_impl();

        /** \brief Executes a handler a specified number of times and then */
        inline static bool countdown_handler_wrapper(boost::shared_ptr < unsigned int >, boost::function < void () >);

        /** \brief Executes a handler until it is manually removed */
        inline static bool perpetual_handler_wrapper(boost::function < void () >);

        /** \brief Terminate an associated process */
        inline void terminate_process(boost::shared_ptr < task_monitor_impl >, boost::shared_ptr < execution::process >);

        /** \brief Blocks until the process has registered */
        inline void await_process(boost::weak_ptr< task_monitor_impl >);

        /** \brief Waits until a connection has been established with the running process */
        inline bool await_connection(boost::weak_ptr< task_monitor_impl >, unsigned int const&);

        /** \brief Waits until a connection has been established with the running process */
        inline bool await_connection(boost::weak_ptr< task_monitor_impl >);

        /** \brief Waits until the current task has been completed */
        inline bool await_completion(boost::weak_ptr< task_monitor_impl >);

        /** \brief Signals that a new connection has been established */
        inline void signal_connection(boost::weak_ptr < task_monitor_impl >, tipi::message::end_point);

        /** \brief Checks the process status and removes */
        inline void signal_change(boost::shared_ptr < task_monitor_impl >&, boost::shared_ptr < execution::process >, const execution::process::status);

        /** \brief Execute event handlers */
        static void service_handlers(boost::shared_ptr < task_monitor_impl >&, event_type);

        /** \brief Executes a handler function once the tool has connected */
        inline void on_connection(boost::function < void () >);

        /** \brief Executes a handler function once the tool has connected */
        inline void once_on_connection(boost::function < void () >);

        /** \brief Executes a handler function once the tool has connected */
        inline void on_completion(boost::function < void () >);

        /** \brief Executes a handler function once on tool completion */
        inline void once_on_completion(boost::function < void () >);

        /** \brief Executes a handler function once status change */
        inline void on_status_change(boost::function < void () >);

        /** \brief Associates a process with this listener */
        inline void attach_process(const boost::shared_ptr< process >& p);

        /** \brief Terminate communication and reset internal state */
        inline bool disconnect(boost::shared_ptr < execution::process > p);

        /** \brief Unblocks waiters and requests a tool to prepare termination */
        inline void finish(boost::shared_ptr < task_monitor_impl > const&, bool b);

        /** \brief Clears handlers and terminates processes */
        inline void shutdown();
    };

    task_monitor_impl::task_monitor_impl() : tipi::controller::communicator_impl() {
    }

    /**
     * \param[in] n the amount of times that the handler is executed
     * \param[in] h the function object that is executed once a connection is established
     **/
    inline bool task_monitor_impl::countdown_handler_wrapper(boost::shared_ptr < unsigned int > n, boost::function < void () > h) {
      h();

      return (--*n == 0);
    }

    /**
     * \param[in] h the function object that is executed once a connection is established
     **/
    inline bool task_monitor_impl::perpetual_handler_wrapper(boost::function < void () > h) {
      h();

      return (false);
    }

    /**
     * \param[in] h the function object that is executed once a connection is established
     **/
    inline void task_monitor_impl::on_connection(boost::function < void () > h) {
      handlers.insert(std::make_pair(connection, boost::bind(&task_monitor_impl::perpetual_handler_wrapper, h)));
    }

    /**
     * \param[in] h the function object that is executed once a connection is established
     **/
    inline void task_monitor_impl::once_on_connection(boost::function < void () > h) {
      boost::shared_ptr < unsigned int > n(new unsigned int);

      *n = 1;

      handlers.insert(std::make_pair(connection, boost::bind(&task_monitor_impl::countdown_handler_wrapper, n, h)));
    }

    /**
     * \param[in] h the function object that is executed after the process status has changed
     **/
    inline void task_monitor_impl::on_status_change(boost::function < void () > h) {
      handlers.insert(std::make_pair(change, boost::bind(&task_monitor_impl::perpetual_handler_wrapper, h)));
    }

    /**
     * \param[in] h the function object that is executed once a connection is established
     **/
    inline void task_monitor_impl::on_completion(boost::function < void () > h) {
      handlers.insert(std::make_pair(completion, boost::bind(&task_monitor_impl::perpetual_handler_wrapper, h)));
    }

    /**
     * \param[in] h the function object that is executed once a connection is established
     **/
    inline void task_monitor_impl::once_on_completion(boost::function < void () > h) {
      boost::shared_ptr < unsigned int > n(new unsigned int);

      *n = 1;

      handlers.insert(std::make_pair(completion, boost::bind(&task_monitor_impl::countdown_handler_wrapper, n, h)));
    }
    /// \endcond
  }
}
