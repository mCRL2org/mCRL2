#ifndef SOCKET_SCHEDULER_H
#define SOCKET_SCHEDULER_H

#include <transport/detail/socket_transceiver.h>

namespace transport {
  namespace transceiver {

    class SocketTransceiver;

    /* Wrapper around an asio demuxer */
    class SocketScheduler {
      friend class SocketTransceiver;
      friend class listener::SocketListener;

      private:
        bool          active;
        asio::demuxer demuxer;

        boost::shared_ptr < asio::thread > thread;

        void task();

      public:
        /** Constructor */
        inline SocketScheduler();

        /** Run the demuxer */
        inline void run();

        /** Stop the demuxer */
        inline void stop();

        /** Destructor */
        inline ~SocketScheduler();
    };

    inline SocketScheduler::SocketScheduler() : active(false) {
    }

    inline void SocketScheduler::task() {
      demuxer.run();
      demuxer.reset();

      active = false;
    }

    inline void SocketScheduler::run() {
      if (!active) {
        using namespace boost;

        active = true;

        thread = shared_ptr < asio::thread > (new asio::thread(bind(&SocketScheduler::task, this)));
      }
    }

    inline void SocketScheduler::stop() {
      if (active) {
        demuxer.interrupt();
      }
    }

    inline SocketScheduler::~SocketScheduler() {
      stop();
    }
  }
}

#endif

