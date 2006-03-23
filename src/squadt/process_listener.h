#ifndef PROCESS_LISTENER_H
#define PROCESS_LISTENER_H

#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "process.h"

namespace squadt {
  namespace execution {

    /** \brief Base class for process change listeners */
    class process_listener {
      friend class executor;
 
      protected:

        /** \brief A pointer to the process associated to this listener or 0 */
        boost::weak_ptr < execution::process >  associated_process;

        /** \brief Semaphore to guarantee mutual exclusion (for use with register_condition) */
        boost::mutex                            register_lock;

        /** \brief Monitor for waiting until process has registered */
        boost::shared_ptr < boost::condition >  register_condition;

      private:

        /** \brief Called when the state changes of the associated executor */
        virtual void report_change(process::status) = 0;

        /** \brief Associates a process with this listener */
        inline void set_process(const process::ptr& p);
 
      public:
 
        /** \brief Abstract destructor */
        virtual ~process_listener() = 0;

        /** \brief Gets a pointer to the associated process */
        inline process* get_process(const bool = false);

        /** \brief Blocks untill the process has registered */
        inline void await_process();
    };
 
    inline process_listener::~process_listener() {
    }

    /**
     * @param[in] p shared pointer to the process
     **/
    inline void process_listener::set_process(const process::ptr& p) {
      assert(p.get() != 0);

      boost::mutex::scoped_lock l(register_lock);

      associated_process = p;

      if (register_condition.get()) {
        /* Wake up waiting threads */
        register_condition->notify_all();

        /* Make sure that register_condition is freed */
        register_condition.reset();
      }
    }

    inline void process_listener::await_process() {
      boost::mutex::scoped_lock l(register_lock);

      if (associated_process.lock().get() == 0 && !register_condition.get()) {
        register_condition = boost::shared_ptr < boost::condition > (new boost::condition());
      }

      register_condition->wait(l);
    }

    /**
     * @param[in] b whether the function should block untill the process has registered or not
     *
     * \return A pointer to the associated process, or 0 on program failure
     **/
    inline process* process_listener::get_process(const bool b) {
      if (b) {
        await_process();
      }

      return (associated_process.lock().get());
    }
  } 
}

#endif
