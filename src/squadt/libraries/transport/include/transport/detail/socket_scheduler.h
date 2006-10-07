#ifndef SOCKET_SCHEDULER_H
#define SOCKET_SCHEDULER_H

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

#include <transport/detail/socket_transceiver.h>

namespace transport {
  namespace listener {
    class socket_listener;
  }

  namespace transceiver {

    /**
     * \brief Basic wrapper around an asio io_service
     *
     * The io_service is the scheduler it responds to incoming events and
     * dispatches tasks (through handler functions) accordingly.
     **/
    class socket_scheduler {
      friend class socket_transceiver;
      friend class listener::socket_listener;

      private:
        /** \brief The current state of the scheduler */
        bool                                active;

        /** \brief The io_service */
        boost::asio::io_service             io_service;

        /** \brief The thread in which the scheduling takes place */
        boost::shared_ptr < boost::thread > thread;

        /** \brief Runs until no more tasks are registered, then resets */
        void task();

      public:
        /** \brief Constructor */
        inline socket_scheduler();

        /** \brief Run the io_service */
        inline void run();

        /** \brief Stop the io_service */
        inline void stop();

        /** \brief Destructor */
        inline ~socket_scheduler();
    };

    inline socket_scheduler::socket_scheduler() : active(false) {
    }

    inline void socket_scheduler::task() {
      io_service.run();
      io_service.reset();

      active = false;
    }

    inline void socket_scheduler::run() {
      using namespace boost;

      if (!active) {
        active = true;

        thread = shared_ptr < boost::thread > (new boost::thread(bind(&socket_scheduler::task, this)));
      }
    }

    inline void socket_scheduler::stop() {
      if (active) {
        io_service.interrupt();

        thread->join();
      }
    }

    inline socket_scheduler::~socket_scheduler() {
      stop();
    }
  }
}

#endif

