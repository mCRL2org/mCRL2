#ifndef PROCESS_LISTENER_H
#define PROCESS_LISTENER_H

#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "process.h"
#include <iostream>
namespace squadt {
  namespace execution {

    /** \brief Base class for process change listeners */
    class process_listener {
      friend class process;
      friend class executor;
 
      protected:

        /** \brief A pointer to the process associated to this listener or 0 */
        process::ptr                                    associated_process;

        /** \brief Semaphore to guarantee mutual exclusion (for use with register_condition) */
        mutable boost::mutex                            register_lock;

        /** \brief Monitor for waiting until process has registered */
        mutable boost::shared_ptr < boost::condition >  register_condition;

      private:

        /** \brief Called when the state changes of the associated executor */
        virtual void report_change(process::status);

        /** \brief Associates a process with this listener */
        inline void set_process(const process::ptr& p);
 
      public:
 
        /** \brief Abstract destructor */
        virtual ~process_listener();

        /** \brief Gets a pointer to the associated process */
        inline process::ptr get_process(const bool b = false) const;

        /** \brief Blocks untill the process has registered */
        inline void await_process() const;
    };
 
    inline process_listener::~process_listener() {
    }

    inline void process_listener::report_change(process::status) {
    }

    /**
     * @param[in] p shared pointer to the process
     **/
    inline void process_listener::set_process(const process::ptr& p) {
      assert(p.get() != 0);

      boost::mutex::scoped_lock l(register_lock);

      associated_process = p;

      if (register_condition.get() != 0) {
        /* Wake up waiting threads */
        register_condition->notify_all();
      }
    }

    inline void process_listener::await_process() const {
      boost::mutex::scoped_lock l(register_lock);

      if (associated_process.get() == 0) {
        if (!register_condition.get()) {
          register_condition = boost::shared_ptr < boost::condition > (new boost::condition());
        }

        register_condition->wait(l);
      }
    }

    /**
     * @param[in] b whether the function should block untill the process has registered or not
     *
     * \return A pointer to the associated process, or 0 on program failure
     **/
    inline process::ptr process_listener::get_process(const bool b) const {
      if (b) {
        await_process();
      }

      return (associated_process);
    }
  } 
}

#endif
