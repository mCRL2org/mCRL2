// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transport/detail/transceiver.hpp

#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace transport {

  class transporter_impl;

  namespace transceiver {

    /**
     * \brief Base class for transceivers
     **/
    class basic_transceiver : private boost::noncopyable {
      friend class transport::transporter_impl;

      public:

        /** \brief Convenience type to hide the shared pointer */
        typedef boost::shared_ptr < basic_transceiver > ptr;

      protected:

        /** \brief The local owner on this side of the connection */
        boost::weak_ptr < transporter_impl > owner;

      private:

        /** \brief Get the current owner */
        inline transporter_impl const* get_owner() const;

      protected:

        /** \brief Deliver the data from the input stream to the owner */
        inline void deliver(std::istream&);

        /** \brief Deliver the data from the input stream to the owner */
        inline void deliver(const std::string&);

        /** \brief Removes this transceiver object from a list of connections */
        inline void handle_disconnect(basic_transceiver*);

      public:

        /** \brief Constructor */
        inline basic_transceiver(boost::shared_ptr < transporter_impl > const&);

        /** \brief Function that facilitates disconnection (on both sides of a connection) */
        virtual void disconnect(boost::shared_ptr < basic_transceiver > const&) = 0;

        /** \brief Send a string input stream to the peer */
        virtual void send(const std::string&) = 0;

        /** \brief Send the contents of an input stream to the peer */
        virtual void send(std::istream&) = 0;

        /** \brief Destructor */
        virtual inline ~basic_transceiver();
    };
  }
}

#endif

