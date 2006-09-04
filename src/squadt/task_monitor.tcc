#include "task_monitor.h"

#include "sip/detail/controller.tcc"

namespace squadt {
  namespace execution {

    class task_monitor_impl : public sip::controller::communicator_impl {
      friend class task_monitor;

      private:
 
        /** \brief Type for event distinction */
        enum event_type {
          connection, ///< A new connection has been established
          completion  ///< A tool completed its operation
        };
       
        /** The type for a map that contains the event handlers */
        typedef std::multimap < const event_type, boost::function < bool () > >  handler_map;

      private:
 
        /** \brief Flag that a connection has been established */
        bool                                            connected;

        /** \brief Flag that a connection has been established */
        bool                                            done;

        /** \brief Semaphore to guarantee mutual exclusion (for use with register_condition) */
        mutable boost::mutex                            register_lock;

        /** \brief Monitor for waiting until process has registered */
        mutable boost::condition                        register_condition;
        
        /** \brief A pointer to the process associated to this listener or 0 */
        process::ptr                                    associated_process;

        /** \brief The event handler that have been registered */
        handler_map                                     handlers;

      private:

        inline task_monitor_impl();
 
        /** \brief Executes a handler a specified number of times and then */
        inline static bool countdown_handler_wrapper(boost::shared_ptr < unsigned int >, boost::function < void () >);

        /** \brief Executes a handler until it is manually removed */
        inline static bool perpetual_handler_wrapper(boost::function < void () >);

        /** \brief Terminate an associated process */
        inline void terminate_process();

        /** \brief Blocks until the process has registered */
        inline void await_process() const;

        /** \brief Waits until a connection has been established with the running process */
        inline void await_connection();

        /** \brief Blocks until private member done is true */
        inline void await_completion();

        /** \brief Signals that a new connection has been established */
        inline void signal_connection(boost::shared_ptr < task_monitor_impl >&, sip::message::end_point);

        /** \brief Checks the process status and removes */
        inline void signal_change(boost::shared_ptr < task_monitor_impl >&, const execution::process::status);

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

        /** \brief Associates a process with this listener */
        void attach_process(const process::ptr& p);

        /** \brief Unblocks waiters and requests a tool to prepare termination */
        inline void finish();
    };

    task_monitor_impl::task_monitor_impl() : connected(false), done(false) {
    }

    /**
     * @param[in] n the amount of times that the handler is executed
     * @param[in] h the function object that is executed once a connection is established
     **/
    inline bool task_monitor_impl::countdown_handler_wrapper(boost::shared_ptr < unsigned int > n, boost::function < void () > h) {
      h();

      return (--*n == 0); 
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    inline bool task_monitor_impl::perpetual_handler_wrapper(boost::function < void () > h) {
      h();

      return (false); 
    }

    inline void task_monitor_impl::await_process() const {
      boost::mutex::scoped_lock l(register_lock);

      if (associated_process.get() == 0) {
        register_condition.wait(l);
      }
    }

    /**
     * Waits until a connection has been established, or the process has terminated
     **/
    inline void task_monitor_impl::await_connection() {
      boost::mutex::scoped_lock l(register_lock);
 
      while (!connected && !done) {
        /* Other side has not connected and the process has not been registered as terminated */
        register_condition.wait(l);
      }
    }

    /**
     * Waits until done is set or the process has terminated
     *
     * \pre associated_process.lock().get() must be unequal 0
     **/
    inline void task_monitor_impl::await_completion() {
      boost::mutex::scoped_lock l(register_lock);
 
      if (!done) {
        while (!done && associated_process.get() != 0) {
          register_condition.wait(l);
        }
      }
    }

    /**
     * @param[in] m a shared pointer to the current object
     * @param[in] e the other end point of the connection
     **/
    inline void task_monitor_impl::signal_connection(boost::shared_ptr < task_monitor_impl >& m, sip::message::end_point) {
      boost::mutex::scoped_lock l(register_lock);
 
      connected = true;
 
      register_condition.notify_all();

      /* Service connection handlers */
      if (0 < handlers.count(completion)) {
        task_monitor_impl::service_handlers(m, connection);
      }
    }

    /**
     * @param[in] m a shared pointer to the current object
     * @param[in] s the current status of the process
     **/
    inline void task_monitor_impl::signal_change(boost::shared_ptr < task_monitor_impl >& m, const execution::process::status s) {
      if ((s == execution::process::completed || s == execution::process::aborted)) {
        /* Unblock any remaining waiters */
        boost::mutex::scoped_lock l(register_lock);
 
        done = true;
 
        /* Signal completion to waiters */
        register_condition.notify_all();

        /* Service connection handlers */
        if (0 < handlers.count(completion)) {
          task_monitor_impl::service_handlers(m, completion);
        }
      }
      else {
        boost::mutex::scoped_lock l(register_lock);
       
        /* Signal completion to waiters */
        register_condition.notify_all();
      }
    }
    /**
     * @param[in] m a shared pointer to this object
     * @param[in] e the event type of which to execute the handler
     **/
    inline void task_monitor_impl::service_handlers(boost::shared_ptr < task_monitor_impl >& m, const event_type e) {
      std::pair < handler_map::iterator, handler_map::iterator > p = m->handlers.equal_range(e);
      
      while (p.first != p.second) {
        handler_map::iterator c = p.first;
      
        ++(p.first);
      
        if ((*(c)).second()) {
          m->handlers.erase(c);
        }
      }
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    inline void task_monitor_impl::on_connection(boost::function < void () > h) {
      handlers.insert(std::make_pair(connection, boost::bind(&task_monitor_impl::perpetual_handler_wrapper, h)));
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    inline void task_monitor_impl::once_on_connection(boost::function < void () > h) {
      boost::shared_ptr < unsigned int > n(new unsigned int);

      *n = 1;

      handlers.insert(std::make_pair(connection, boost::bind(&task_monitor_impl::countdown_handler_wrapper, n, h)));
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    inline void task_monitor_impl::on_completion(boost::function < void () > h) {
      handlers.insert(std::make_pair(completion, boost::bind(&task_monitor_impl::perpetual_handler_wrapper, h)));
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    inline void task_monitor_impl::once_on_completion(boost::function < void () > h) {
      boost::shared_ptr < unsigned int > n(new unsigned int);

      *n = 1;

      handlers.insert(std::make_pair(completion, boost::bind(&task_monitor_impl::countdown_handler_wrapper, n, h)));
    }

    /**
     * @param[in] p shared pointer to the process
     **/
    inline void task_monitor_impl::attach_process(const process::ptr& p) {
      boost::mutex::scoped_lock l(register_lock);

      associated_process = p;

      /* Wake up waiting threads */
      register_condition.notify_all();
    }

    inline void task_monitor_impl::finish() {
      boost::mutex::scoped_lock l(register_lock);
   
      done = true;
   
      /* Signal completion to waiters */
      register_condition.notify_all();
    }

    /** \brief Terminates a running process */
    inline void task_monitor_impl::terminate_process() {
      if (associated_process.get() != 0) {
        disconnect();

        send_message(sip::message_request_termination);

        associated_process->terminate();

        logger->log(1, boost::str(boost::format("process terminated (tool %s pid(%u))\n")
                  % associated_process->get_executable_name() % associated_process->get_identifier()));
      }
    }
  }
}
