// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transport/detail/listener.hpp

#ifndef TRANSPORT_LISTENER_H_
#define TRANSPORT_LISTENER_H_

#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace transport {
  class transporter;
  class transporter_impl;

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
        inline void associate(boost::shared_ptr< transceiver::basic_transceiver >);

      public:

        /** \brief Constructor */
        inline basic_listener(boost::shared_ptr< transport::transporter_impl > const& m);

        /** \brief Shutdown the listener */
        virtual void shutdown() = 0;

        /** \brief Destructor */
        virtual inline ~basic_listener();
    };
  }
}

#endif

