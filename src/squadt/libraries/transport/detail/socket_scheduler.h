#ifndef SOCKET_SCHEDULER_H
#define SOCKET_SCHEDULER_H

#include <transport/detail/socket_transceiver.h>

namespace transport {
  namespace transceiver {

    class socket_transceiver;

    /* Wrapper around an asio demuxer */
    class socket_scheduler {
      friend class socket_transceiver;
      friend class listener::socket_listener;

      private:
        bool          active;
        asio::demuxer demuxer;

        boost::shared_ptr < asio::thread > thread;

        void task();

      public:
        /** Constructor */
        inline socket_scheduler();

        /** Run the demuxer */
        inline void run();

        /** Stop the demuxer */
        inline void stop();

        /** Destructor */
        inline ~socket_scheduler();
    };

    inline socket_scheduler::socket_scheduler() : active(false) {
    }

    inline void socket_scheduler::task() {
      demuxer.run();
      demuxer.reset();

      active = false;
    }

    inline void socket_scheduler::run() {
      if (!active) {
        using namespace boost;

        active = true;

        thread = shared_ptr < asio::thread > (new asio::thread(bind(&socket_scheduler::task, this)));
      }
    }

    inline void socket_scheduler::stop() {
      if (active) {
        demuxer.interrupt();
      }
    }

    inline socket_scheduler::~socket_scheduler() {
      stop();
    }
  }
}

#endif

