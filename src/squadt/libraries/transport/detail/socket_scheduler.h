#ifndef SOCKET_SCHEDULER_H
#define SOCKET_SCHEDULER_H

#include <boost/thread/mutex.hpp>

#include <transport/detail/socket_transceiver.h>

namespace transport {
  namespace listener {
    class socket_listener;
  }

  namespace transceiver {

    /**
     * \brief Basic wrapper around an asio demuxer
     *
     * The demuxer is the scheduler it responds to incoming events and
     * dispatches tasks (through handler functions) accordingly.
     **/
    class socket_scheduler {
      friend class socket_transceiver;
      friend class listener::socket_listener;

      private:
        /** \brief The current state of the scheduler */
        bool                               active;

        /** \brief The demuxer */
        asio::demuxer                      demuxer;

        /** \brief This lock is used to ensure that switching between states active or shutdown is atomic */
        boost::mutex                       run_lock;

        /** \brief The thread in which the scheduling takes place */
        boost::shared_ptr < asio::thread > thread;

        /** \brief Runs until no more tasks are registered, then resets */
        void task();

      public:
        /** \brief Constructor */
        inline socket_scheduler();

        /** \brief Run the demuxer */
        inline void run();

        /** \brief Stop the demuxer */
        inline void stop();

        /** \brief Destructor */
        inline ~socket_scheduler();
    };

    inline socket_scheduler::socket_scheduler() : active(false) {
    }

    inline void socket_scheduler::task() {
      demuxer.run();

      boost::mutex::scoped_lock l(run_lock);

      demuxer.reset();

      active = false;
    }

    inline void socket_scheduler::run() {
      boost::mutex::scoped_lock l(run_lock);

      if (!active) {
        using namespace boost;

        active = true;

        thread = shared_ptr < asio::thread > (new asio::thread(bind(&socket_scheduler::task, this)));
      }
    }

    inline void socket_scheduler::stop() {
      boost::mutex::scoped_lock l(run_lock);

      if (active) {
        demuxer.interrupt();

        active = false;
      }
    }

    inline socket_scheduler::~socket_scheduler() {
      stop();
    }
  }
}

#endif

