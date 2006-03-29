#ifndef TASK_MONITOR_H
#define TASK_MONITOR_H


#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <sip/controller.h>

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
        typedef boost::shared_ptr < task_monitor > ptr;

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

      protected:
 
        /** \brief Unblocks waiters and requests a tool to prepare termination */
        inline void finish();

      private:
 
        /** \brief Signals that a new connection has been established */
        inline void signal_connection(sip::end_point&);

        /** \brief Checks the process status and removes */
        virtual inline void signal_change(const execution::process::status);

        /** \brief Associates a process with this listener */
        inline void set_process(const process::ptr& p);
 
      public:

        /** \brief Constructor */
        task_monitor();
 
        /** \brief Gets a pointer to the associated process */
        inline process::ptr get_process(const bool b = false) const;

        /** \brief Blocks untill the process has registered */
        inline void await_process() const;

        /** \brief Waits until a connection has been established with the running process */
        inline void await_connection();

        /** \brief Blocks until all private member done is true */
        inline void await_completion();

        /** \brief Disconnects from a running process (or make sure not connection exists) */
        inline void disconnect(execution::process*);

        /** \brief Destructor */
        virtual ~task_monitor();
    };

    inline task_monitor::task_monitor() : sip::controller::communicator(), connected(false), done(false) {
    }

    inline task_monitor::~task_monitor() {
    }

    inline void task_monitor::disconnect(execution::process*) {
      connected = false;

      transporter::disconnect();
    }

    /**
     * @param[in] p shared pointer to the process
     **/
    inline void task_monitor::set_process(const process::ptr& p) {
      assert(p.get() != 0);

      boost::mutex::scoped_lock l(register_lock);

      associated_process = p;

      if (register_condition.get() != 0) {
        /* Wake up waiting threads */
        register_condition->notify_all();
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
     * @param p a pointer to the local end_point of the new connection
     **/
    inline void task_monitor::signal_connection(sip::end_point&) {
      boost::mutex::scoped_lock l(register_lock);
 
      connected = true;
 
      register_condition->notify_all();
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
     * Waits until extraction is complete or the process has terminated
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
        }
      }
    }
  }
}

#endif
