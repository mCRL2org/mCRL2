//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/transport/detail/listener.h

#ifndef TRANSPORT_LISTENER_H_
#define TRANSPORT_LISTENER_H_

#include <boost/shared_ptr.hpp>

namespace transport {
  class transporter;

  namespace transceiver {
    class basic_transceiver;
  }

  namespace listener {

    /**
     * \brief Base class for listeners
     **/
    class basic_listener {

      public:

        /** \brief Convenience type alias to hide the shared pointer implementation */
        typedef boost::shared_ptr < basic_listener > ptr;

      protected:

        /** \brief The communicator that will take over the accepted connections */
        boost::weak_ptr < transport::transporter_impl > owner;

        /** \brief Associates the owner with the connection */
        inline void associate(transceiver::basic_transceiver::ptr);

      public:

        /** \brief Constructor */
        inline basic_listener(boost::shared_ptr < transport::transporter_impl > const& m);

        /** \brief Shutdown the listener */
        virtual void shutdown() = 0;

        /** \brief Destructor */
        virtual inline ~basic_listener();
    };
  }
}

#endif

