// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transport/transporter.hpp

#ifndef TRANSPORTER_H
#define TRANSPORTER_H

#include <string>
#include <iosfwd>

#include <boost/noncopyable.hpp>

#include <tipi/detail/transport/detail/transceiver.hpp>

/*
 * Socket/Direct communication abstraction
 *
 * for transparent communication, via sockets or direct, between senders and
 * receivers
 *
 *  Direct:
 *   - the sender directly calls the deliver routine of a connected transporter object
 *
 *  Sockets:
 *   - the sender puts the data on a socket, when the data arrives, it is
 *   passed in the very same way to the receiving context.
 *
 *  Currently, unless exceptions occur, a single sender can be connected to
 *  many receivers that will all receive everything that is sent.
 */

namespace transport {
  using namespace transceiver;

  class transporter_impl;

  /**
   * \brief Connection component used for data transport
   **/
  class transporter : private boost::noncopyable {
    friend class transporter_impl;

    protected:

      /** \brief Pointer to implementation object (handle/body idiom) */
      boost::shared_ptr < transporter_impl > impl;

    protected:

      /** \brief Default constructor with no initial connections */
      transporter(boost::shared_ptr < transporter_impl > const&);

    public:

      /** \brief Creates direct connection to another transporter object */
      void connect(transporter&);

      /** \brief Creates socket connection to another transporter object (using a loopback connection by default) */
      void connect(std::string const& = "", short int const& = 0);

      /** \brief Disconnect all */
      void disconnect();

      /** \brief Disconnect from directly connected peer */
      bool disconnect(basic_transceiver const&);

      /** \brief Disconnect from directly connected peer */
      bool disconnect(transporter&);

      /** \brief Pass a connection through to another transporter */
      void relay_connection(transporter*, basic_transceiver*);

      /** \brief Activate a socket listener */
      void add_listener(std::string const& = "", short int const& port = 0);

      /** \brief Activate a socket listener by its number */
      void remove_listener(size_t number = 0);

      /** \brief Communicate a string with all peers */
      void send(const std::string&);

      /** \brief Communicate data from a stream with all peers */
      void send(std::istream&);

      /** \brief Returns the local hosts name or IP address */
      static std::string get_local_host();

      /** \brief The number of active listeners */
      size_t number_of_listeners() const;

      /** \brief The number of active connections */
      size_t number_of_connections() const;
  };
}

#endif

