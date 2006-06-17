#ifndef TASK_MONITOR_H
#define TASK_MONITOR_H

#include <map>

#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>

#include <sip/controller.h>
#include <sip/detail/basic_messenger.tcc>

#include "process.h"

namespace squadt {

  class tool_manager;
 
  namespace execution {

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
 
        /** \brief Flag that a connection has been established */
        bool                                            connected;

        /** \brief Flag that a connection has been established */
        bool                                            done;

        /** \brief A pointer to the process associated to this listener or 0 */
        process::ptr                                    associated_process;

        /** \brief Semaphore to guarantee mutual exclusion (for use with register_condition) */
        mutable boost::mutex                            register_lock;

        /** \brief Monitor for waiting until process has registered */
        mutable boost::shared_ptr < boost::condition >  register_condition;
        
        /** \brief Thread for event delivery */
        static boost::thread                            delivery_thread;

      protected:
 
        /** \brief Unblocks waiters and requests a tool to prepare termination */
        inline void finish();

      private:
 
        /** \brief Type for event distinction */
        enum event_type {
          connection, ///< A new connection has been established
          completion  ///< A tool completed its operation
        };
       
        /** The type for a map that contains the event handlers */
        typedef std::multimap < const event_type, boost::function < bool () > >  handler_map;
       
      private:
 
        /** \brief The event handler that have been registered */
        handler_map                      handlers;

      private:
 
        /** \brief Signals that a new connection has been established */
        inline void signal_connection(const sip::end_point&);

        /** \brief Checks the process status and removes */
        inline void signal_change(const execution::process::status);

        /** \brief Executes a handler a specified number of times and then */
        inline static bool countdown_handler_wrapper(boost::shared_ptr < unsigned int >, boost::function < void () >);

        /** \brief Executes a handler until it is manually removed */
        inline static bool perpetual_handler_wrapper(boost::function < void () >);

        /** \brief Associates a process with this listener */
        inline void attach_process(const process::ptr& p);
 
        /** \brief Execute event handlers */
        void service_handlers(event_type e);

      public:

        /** \brief Constructor */
        task_monitor();
 
        /** \brief Gets a pointer to the associated process */
        inline process::ptr get_process(const bool b = false) const;

        /** \brief Terminate an associated process */
        inline void terminate_process();

        /** \brief Blocks untill the process has registered */
        inline void await_process() const;

        /** \brief Waits until a connection has been established with the running process */
        inline void await_connection();

        /** \brief Blocks until private member done is true */
        inline void await_completion();

        /** \brief Executes a handler function once the tool has connected */
        inline void on_connection(boost::function < void () >);

        /** \brief Executes a handler function once the tool has connected */
        inline void once_on_connection(boost::function < void () >);

        /** \brief Executes a handler function once the tool has connected */
        inline void on_completion(boost::function < void () >);

        /** \brief Executes a handler function once on tool completion */
        inline void once_on_completion(boost::function < void () >);

        /** \brief Whether there still exists a connection with the tool */
        inline bool is_connected() const;

        /** \brief Whether the tool is still busy performing its task */
        inline bool is_busy() const;

        /** \brief Disconnects from a running process (or make sure not connection exists) */
        inline void disconnect(execution::process*);

        /** \brief Destructor */
        virtual ~task_monitor();
    };

    inline task_monitor::task_monitor() : sip::controller::communicator(), connected(false), done(false) {
    }

    inline task_monitor::~task_monitor() {
      terminate_process();
    }

    inline void task_monitor::disconnect(execution::process*) {
      connected = false;

      transporter::disconnect();
    }

    /**
     * @param[in] p shared pointer to the process
     **/
    inline void task_monitor::attach_process(const process::ptr& p) {
      assert(p.get() != 0);

      boost::mutex::scoped_lock l(register_lock);

      associated_process = p;

      if (register_condition.get() != 0) {
        /* Wake up waiting threads */
        register_condition->notify_all();
      }
    }

    /** \brief Terminates a running process */
    inline void task_monitor::terminate_process() {
      if (associated_process.get() != 0) {
        send_message(sip::message_signal_termination);

        transporter::disconnect();

        sleep(1);

        associated_process->terminate();

        associated_process.reset();
      }
    }

    /**
     * @param[in] b whether the function should block untill the process has registered or not
     *
     * \return A pointer to the associated process, or 0 on program failure
     **/
    inline process::ptr task_monitor::get_process(const bool b) const {
      if (b) {
        await_process();
      }

      return (associated_process);
    }

    inline void task_monitor::await_process() const {
      boost::mutex::scoped_lock l(register_lock);

      if (associated_process.get() == 0) {
        if (!register_condition.get()) {
          register_condition = boost::shared_ptr < boost::condition > (new boost::condition());
        }

        register_condition->wait(l);
      }
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    inline void task_monitor::on_connection(boost::function < void () > h) {
      handlers.insert(std::make_pair(connection, boost::bind(&task_monitor::perpetual_handler_wrapper, h)));
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    inline void task_monitor::once_on_connection(boost::function < void () > h) {
      boost::shared_ptr < unsigned int > n(new unsigned int);

      *n = 1;

      handlers.insert(std::make_pair(connection, boost::bind(&task_monitor::countdown_handler_wrapper, n, h)));
    }

    inline void task_monitor::signal_connection(const sip::end_point&) {
      boost::mutex::scoped_lock l(register_lock);
 
      connected = true;
 
      if (register_condition.get() != 0) {
        register_condition->notify_all();

        /* Service connection handlers */
        if (0 < handlers.count(connection)) {
          boost::thread s(boost::bind(&task_monitor::service_handlers, this, connection));
        }
      }
    }
 
    /**
     * Waits until a connection has been established, or the process has terminated
     **/
    inline void task_monitor::await_connection() {
      boost::mutex::scoped_lock l(register_lock);
 
      if (!connected) {
        if (register_condition.get() == 0) {
          register_condition = boost::shared_ptr < boost::condition > (new boost::condition());
        }

        while (!connected && !done) {
          /* Other side has not connected and the process has not been registered as terminated */
          register_condition->wait(l);
        }
      }
    }

    /**
     * @param e the event type of which to execute the handler
     **/
    inline void task_monitor::service_handlers(const event_type e) {
      std::pair < handler_map::iterator, handler_map::iterator > p = handlers.equal_range(e);

      while (p.first != p.second) {
        handler_map::iterator c = p.first;

        ++(p.first);

        if ((*(c)).second()) {
          handlers.erase(c);
        }
      }
    }

    /**
     * @param[in] n the amount of times that the handler is executed
     * @param[in] h the function object that is executed once a connection is established
     **/
    inline bool task_monitor::countdown_handler_wrapper(boost::shared_ptr < unsigned int > n, boost::function < void () > h) {
      h();

      return (--*n == 0); 
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    inline bool task_monitor::perpetual_handler_wrapper(boost::function < void () > h) {
      h();

      return (false); 
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    inline void task_monitor::on_completion(boost::function < void () > h) {
      handlers.insert(std::make_pair(completion, boost::bind(&task_monitor::perpetual_handler_wrapper, h)));
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    inline void task_monitor::once_on_completion(boost::function < void () > h) {
      boost::shared_ptr < unsigned int > n(new unsigned int);

      *n = 1;

      handlers.insert(std::make_pair(completion, boost::bind(&task_monitor::countdown_handler_wrapper, n, h)));
    }

    /**
     * Waits until done is set or the process has terminated
     *
     * \pre associated_process.lock().get() must be unequal 0
     **/
    inline void task_monitor::await_completion() {
      boost::mutex::scoped_lock l(register_lock);
 
      if (!done) {
        if (register_condition.get() == 0) {
          register_condition = boost::shared_ptr < boost::condition > (new boost::condition());
        }

        while (!done && associated_process.get() != 0) {
          register_condition->wait(l);
        }
      }
    }

    inline bool task_monitor::is_connected() const {
      return (connected);
    }

    inline bool task_monitor::is_busy() const {
      return (!done);
    }

    /**
     * This method is typically for use by derived classes
     **/
    inline void task_monitor::finish() {
      /* Let the tool know that it should prepare for termination */
      request_termination();
   
      boost::mutex::scoped_lock l(register_lock);
   
      done = true;
   
      /* Signal completion to waiters */
      register_condition->notify_all();
    }

    
    /**
     * @param[in] s the current status of the process
     **/
    inline void task_monitor::signal_change(const execution::process::status s) {
      if (s == execution::process::completed || s == execution::process::aborted) {
        /* Unblock any remaining waiters */
        boost::mutex::scoped_lock l(register_lock);
 
        done = true;
 
        if (register_condition.get() != 0) {
          /* Signal completion to waiters */
          register_condition->notify_all();

          /* Service connection handlers */
          if (0 < handlers.count(completion)) {
            boost::thread s(boost::bind(&task_monitor::service_handlers, this, completion));
          }
        }
      }
    }
  }
}

#endif
