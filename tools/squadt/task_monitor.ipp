// Author(s): Jeroen van der Wulp
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file task_monitor.ipp
/// \brief Add your file description here.

#include "task_monitor.hpp"

#include "tipi/detail/controller.ipp"

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
        boost::condition                                register_condition;

        /** \brief Monitor for waiting until process has registered */
        boost::condition                                connection_condition;
        
        /** \brief Monitor for waiting until process has registered */
        boost::condition                                completion_condition;
        
        /** \brief A pointer to the process associated to this listener or 0 */
        process::sptr                                   associated_process;

        /** \brief The event handlers that have been registered */
        handler_map                                     handlers;

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
        inline void await_process();

        /** \brief Waits until a connection has been established with the running process */
        inline bool await_connection(unsigned int const&);

        /** \brief Waits until a connection has been established with the running process */
        inline bool await_connection();

        /** \brief Waits until the current task has been completed */
        inline bool await_completion();

        /** \brief Signals that a new connection has been established */
        inline void signal_connection(boost::shared_ptr < task_monitor_impl >&, tipi::message::end_point);

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
        void attach_process(const process::sptr& p);

        /** \brief Terminate communication and reset internal state */
        inline bool disconnect(boost::shared_ptr < execution::process > p);

        /** \brief Unblocks waiters and requests a tool to prepare termination */
        inline void finish(boost::shared_ptr < task_monitor_impl > const&);

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

    inline void task_monitor_impl::await_process() {
      boost::mutex::scoped_lock l(register_lock);

      if (associated_process.get() == 0) {
        register_condition.wait(l);
      }
    }

    /**
     * Waits until a connection has been established a timeout has occurred, or the process has terminated
     * \return whether a connection is active
     **/
    inline bool task_monitor_impl::await_connection(unsigned int const& ts) {
      boost::mutex::scoped_lock l(register_lock);
 
      boost::xtime time;

      xtime_get(&time, boost::TIME_UTC);

      time.sec += ts;

      /* Other side has not connected and the process has not been registered as terminated */
      connection_condition.timed_wait(l, time);

      return 0 < number_of_connections();
    }

    /**
     * Waits until a connection has been established, or the process has terminated
     * \return whether a connection is active
     **/
    inline bool task_monitor_impl::await_connection() {
      boost::mutex::scoped_lock l(register_lock);
 
      /* Other side has not connected and the process has not been registered as terminated */
      if (number_of_connections() == 0) {
        connection_condition.wait(l);
      }

      return 0 < number_of_connections();
    }

    /**
     * Waits until a connection has been established, or the process has terminated
     * \return whether the task has been completed successfully
     **/
    inline bool task_monitor_impl::await_completion() {
      struct local {
        static void handle_task_completion(task_monitor_impl& t, boost::shared_ptr < const tipi::message > const& m, bool& result) {
          result = m.get() && !m->is_empty();

          boost::mutex::scoped_lock l(t.register_lock);

          t.completion_condition.notify_all();
        }
      };

      bool result  = false;
 
      boost::mutex::scoped_lock l(register_lock);

      add_handler(tipi::message_task_done, boost::bind(&local::handle_task_completion, boost::ref(*this), _1, boost::ref(result)));

      /* Other side has not connected and the process has not been registered as terminated */
      completion_condition.wait(l);

      return result;
    }

    /**
     * \param[in] m a shared pointer to the current object
     **/
    inline void task_monitor_impl::signal_connection(boost::shared_ptr < task_monitor_impl >& m, tipi::message::end_point) {
      boost::mutex::scoped_lock l(register_lock);
 
      logger->log(1, boost::str(boost::format("connection established with `%s' (process id %u)\n")
                % associated_process->get_executable_name() % associated_process->get_identifier()));

      /* Service connection handlers */
      if (0 < handlers.count(connection)) {
        task_monitor_impl::service_handlers(m, connection);
      }

      connection_condition.notify_all();
    }

    inline void task_monitor_impl::shutdown() {
      boost::mutex::scoped_lock l(register_lock);

      handlers.clear();

      /* Signal completion to waiters */
      register_condition.notify_all();
      connection_condition.notify_all();
      completion_condition.notify_all();
    }

    /**
     * \param[in] m a shared pointer to the current object
     * \param[in] s the current status of the process
     **/
    inline void task_monitor_impl::signal_change(boost::shared_ptr < task_monitor_impl >& m,
                        boost::shared_ptr < execution::process > p, const execution::process::status s) {

      boost::mutex::scoped_lock l(register_lock);

      /* Service change handlers */
      if (0 < handlers.count(change)) {
        task_monitor_impl::service_handlers(m, change);
      }

      if (s == execution::process::completed || s == execution::process::aborted) {
        /* Service connection handlers */
        if (0 < handlers.count(completion)) {
          task_monitor_impl::service_handlers(m, completion);
        }

        if (p.get()) {
          logger->log(1, boost::format("process ended `%s' (process id %u)\n")
                    % p->get_executable_name() % p->get_identifier());
        }

        if (associated_process == p) {
          associated_process.reset();
        }

        /* Signal completion to waiters */
        completion_condition.notify_all();
      }
    }

    /**
     * \param[in] m a shared pointer to this object
     * \param[in] e the event type of which to execute the handler
     **/
    inline void task_monitor_impl::service_handlers(boost::shared_ptr < task_monitor_impl >& m, const event_type e) {
      std::pair < handler_map::iterator, handler_map::iterator > p = m->handlers.equal_range(e);
      
      for (handler_map::iterator i = p.first; i != p.second; ++i) {
        if (i->second()) {
          m->handlers.erase(i);
        }
      }
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

    /**
     * \param[in] p shared pointer to the process
     **/
    inline void task_monitor_impl::attach_process(const process::sptr& p) {
      boost::mutex::scoped_lock l(register_lock);

      associated_process = p;

      /* Wake up waiting threads */
      register_condition.notify_all();
    }

    /**
     * \param[in] b whether to wait for the process to terminate (whether or not to block)
     * \param[in] g shared pointer to this object, to ensure its existence
     **/
    inline void task_monitor_impl::finish(boost::shared_ptr < task_monitor_impl > const& g) {
      boost::thread t(boost::bind(&task_monitor_impl::terminate_process, this, g,
			boost::shared_ptr < execution::process > (associated_process)));
     
      associated_process.reset();
    }

    inline bool task_monitor_impl::disconnect(boost::shared_ptr < execution::process > p) {
      boost::mutex::scoped_lock l(register_lock);

      if (p.get() && p->get_status() == process::running && 0 < number_of_connections()) {
        // request termination
        send_message(tipi::message_termination);

        logger->log(1, boost::str(boost::format("termination request sent to %s (process id %u)\n")
                  % p->get_executable_name() % p->get_identifier()));

        tipi::controller::communicator_impl::disconnect();

        return true;
      }

      return false;
    }

    /**
     * \brief Terminates a running process
     *
     * \param[in] g shared pointer to this object, to ensure its existence
     **/
    inline void task_monitor_impl::terminate_process(boost::shared_ptr < task_monitor_impl > g, boost::shared_ptr < execution::process > p) {

      if (disconnect(p)) {
        boost::xtime timeout;
        boost::xtime_get(&timeout, boost::TIME_UTC);
        timeout.sec += 5;

        boost::thread::sleep(timeout);
      }

      if (p && p->get_status() == execution::process::running) {
        logger->log(1, boost::format("forcibly terminating process (tool %s with id %u)\n") %
                      p->get_executable_name() % p->get_identifier());

        p->terminate();
      }
    }

    /// \endcond
  }
}
