// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transport/detail/socket_scheduler.hpp

#ifndef SOCKET_SCHEDULER_H
#define SOCKET_SCHEDULER_H

#include <boost/asio/io_service.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

namespace transport {
  namespace listener {
    class socket_listener;
  }

  namespace transceiver {
    class socket_transceiver;

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
        bool                                         active;

        /** \brief The io_service */
        boost::asio::io_service                      io_service;

        /** \brief The thread in which the scheduling takes place */
        boost::shared_ptr< boost::thread >           thread;

      private:

        /** \brief Runs until no more tasks are registered, then resets */
        void task() {
          io_service.run();
          io_service.reset();

          active = false;
        }

      public:

        /** \brief Constructor */
        inline socket_scheduler() : active(false) {
        }

        /** \brief Run the io_service */
        inline void run() {
          if (!active) {
            active = true;

            thread.reset(new boost::thread(boost::bind(&socket_scheduler::task, this)));
          }
        }

        /** \brief Stop the io_service */
        inline void stop() {
          if (active) {
            io_service.stop();

            thread->join();
          }
        }

        /** \brief Destructor */
        inline ~socket_scheduler() {
          stop();
        }
    };
  }
}

#endif

