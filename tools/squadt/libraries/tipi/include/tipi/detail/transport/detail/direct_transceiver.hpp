// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transport/detail/direct_transceiver.hpp

#ifndef DIRECT_TRANSCEIVER_H
#define DIRECT_TRANSCEIVER_H

#include <tipi/detail/transport/detail/transceiver.ipp>

namespace transport {
  /// \cond INTERNAL_DOCS
  namespace transceiver {

    /* Class that is used internally for direct transmitting/receiving */
    class direct_transceiver : public basic_transceiver {
      friend class transport::transporter;
      friend class transport::transporter_impl;

      private:

        /** The other side of the connection */
        direct_transceiver* peer;

        /** Deliver data to owner */
        inline void deliver(std::istream& input);

        /** Constructor for use by the transporter constructor */
        inline direct_transceiver(boost::shared_ptr < transporter_impl > const& o);

        /** Constructor for use by the transporter constructor */
        inline direct_transceiver(boost::shared_ptr < transporter_impl > const& o, boost::shared_ptr < direct_transceiver > const& p);

      public:

        /** Terminate the connection with the peer */
        inline void disconnect(boost::shared_ptr < basic_transceiver > const&);

        /** Send a string input stream to the peer */
        inline void send(const std::string& data);

        /** Send the contents of an input stream to the peer */
        inline void send(std::istream& data);

        /** Destructor */
        inline ~direct_transceiver();
    };

    /**
     * @param o the transporter to associate with
     **/
    inline direct_transceiver::direct_transceiver(boost::shared_ptr < transporter_impl > const& o) : basic_transceiver(o) {
    }

    /**
     * @param o the transporter to associate with
     * @param p the other end point
     **/
    inline direct_transceiver::direct_transceiver(boost::shared_ptr < transporter_impl > const& o, boost::shared_ptr < direct_transceiver > const& p) : basic_transceiver(o), peer(p.get()) {
      if(!(p.get() != 0)){
        throw mcrl2::runtime_error("Direct transceiver does not exists");
      }

      p->peer = this;
    }

    /**
     * @param p a shared pointer to this object
     **/
    inline void direct_transceiver::disconnect(boost::shared_ptr < basic_transceiver > const& p) {
      boost::weak_ptr < transporter_impl > o(owner);

      peer->handle_disconnect(this);

      owner = o;

      handle_disconnect(peer);
    }

    /**
     * @param d the data to be sent
     **/
    inline void direct_transceiver::send(const std::string& d) {
      std::istringstream input(d);

      basic_transceiver::deliver(input);
    }

    /**
     * @param s stream that contains the data to be sent
     **/
    inline void direct_transceiver::send(std::istream& s) {
      basic_transceiver::deliver(s);
    }

    inline direct_transceiver::~direct_transceiver() {
    }
  }
  /// \endcond
}

#endif

