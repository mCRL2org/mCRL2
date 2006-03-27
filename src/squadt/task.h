#ifndef BASIC_PROCESSOR_H
#define BASIC_PROCESSOR_H

#include <sip/controller.h>

#include "process_listener.h"

namespace squadt {

  class tool_manager;
 
  namespace execution {

    /**
     * \brief Base class for tasks that provide tool execution via a tool manager
     **/
    class task : public sip::controller::communicator, public execution::process_listener {
      friend class squadt::tool_manager;

      protected:
 
        /** \brief Flag that a connection has been established */
        bool connected;

        /** \brief Flag that a connection has been established */
        bool done;

      protected:
 
        /** \brief Unblocks waiters and requests a tool to prepare termination */
        inline void finish();

      private:
 
        /** \brief Signals that a new connection has been established */
        inline void signal_connection(sip::end_point&);

      public:
 
        /** \brief Convenience type for hiding shared pointer implementation */
        typedef boost::shared_ptr < task > ptr;

      public:

        /** \brief Constructor */
        task();
 
        /** \brief Waits until a connection has been established with the running process */
        inline void await_connection();

        /** \brief Blocks until all private member done is true */
        inline void await_completion();
    };

    inline task::task() : sip::controller::communicator(), connected(false), done(false) {
    }

    /**
     * @param p a pointer to the local end_point of the new connection
     **/
    inline void task::signal_connection(sip::end_point&) {
      boost::mutex::scoped_lock l(register_lock);
 
      connected = true;
 
      register_condition->notify_all();
    }
 
    /**
     * Waits until a connection has been established, or the process has terminated
     **/
    inline void task::await_connection() {
      boost::mutex::scoped_lock l(register_lock);
 
      while (!connected && !done) {
        /* Other side has not connected and the process has not been registered as terminated */
        if (!register_condition.get()) {
          register_condition = boost::shared_ptr < boost::condition > (new boost::condition());
        }
 
        register_condition->wait(l);
      }
    }

    /**
     * Waits until extraction is complete or the process has terminated
     *
     * \pre associated_process.lock().get() must be unequal 0
     **/
    inline void task::await_completion() {
      boost::mutex::scoped_lock l(register_lock);
 
      assert (associated_process.get() != 0);
 
      while (!done) {
        if (register_condition.get() == 0) {
          register_condition = boost::shared_ptr < boost::condition > (new boost::condition());
        }
       
        register_condition->wait(l);
      }
    }

    /**
     * This method is typically for use by derived classes
     **/
    inline void task::finish() {
      /* Let the tool know that it should prepare for termination */
      request_termination();
   
      boost::mutex::scoped_lock l(register_lock);
   
      done = true;
   
      /* Signal completion to waiters */
      register_condition->notify_all();
    }
  }
}

#endif
