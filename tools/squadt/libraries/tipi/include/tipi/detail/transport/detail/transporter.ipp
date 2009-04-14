// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/tipi/detail/transport/detail/transporter.ipp
#ifndef TIPI_TRANSPORTER_IPP__
#define TIPI_TRANSPORTER_IPP__

#include <exception>
#include <list>
#include <istream>

#include <boost/asio/ip/address.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <tipi/detail/transport/transporter.hpp>
#include <tipi/detail/transport/detail/listener.hpp>

namespace transport {
  using namespace transceiver;
  using namespace listener;

  class transporter_impl : private boost::noncopyable {
    friend class transceiver::basic_transceiver;
    friend class listener::basic_listener;
    friend class transport::transporter;

    public:

      /** \brief Convenience type to hide the shared pointer */
      typedef std::list < basic_transceiver::ptr >    connection_list;

      /** \brief Convenience type to hide the shared pointer */
      typedef std::list < basic_listener::ptr >       listener_list;

    private:

      /** \brief To obtain mutual exclusion for operations on the list of connections */
      mutable boost::recursive_mutex lock;

      /** \brief Listeners (for socket communication etc) */
      listener_list                  listeners;

      /** \brief The list with connections */
      connection_list                connections;

    protected:

      /** \brief Abstract function for the delivery of streamed data to the client program */
      virtual void deliver(std::istream&, basic_transceiver const*) = 0;

      /** \brief Abstract function for the delivery of streamed data to the client program */
      virtual void deliver(std::string const&, basic_transceiver const*) = 0;

      /** \brief Abstract function to signal a disconnection has occurred */
      virtual void on_disconnect(basic_transceiver const*) = 0;

    protected:

      /** \brief Associate a connection with this transporter */
      void associate(boost::shared_ptr < transporter_impl > const&, const basic_transceiver::ptr&);

      /** \brief Associate a connection with this transporter */
      void associate(boost::shared_ptr < transporter_impl > const&, basic_transceiver*);

      /** \brief Disassociate a connection from this transporter */
      basic_transceiver::ptr disassociate(basic_transceiver*);

      /** \brief Creates direct connection to another transporter object */
      void connect(boost::shared_ptr < basic_transceiver > const&, boost::shared_ptr < transporter_impl >&);

      /** \brief Creates socket connection to another transporter object (using a loopback connection by default) */
      void connect(boost::shared_ptr < basic_transceiver > const&, boost::asio::ip::address const&, short int const&);

      /** \brief Creates socket connection to another transporter object (using a loopback connection by default) */
      void connect(boost::shared_ptr < basic_transceiver > const&, std::string const&, short int const&);

      /** \brief Disconnect all */
      virtual void disconnect();

      /** \brief Disconnect based on local endpoint */
      bool disconnect(basic_transceiver const&);

      /** \brief Disconnect from directly connected peer */
      bool disconnect(transporter_impl const*);

      /** \brief Pass a connection through to another transporter */
      void relay_connection(transporter*, basic_transceiver*);

      /** \brief Activate a socket listener */
      void add_listener(boost::shared_ptr < transporter_impl > const&, boost::asio::ip::address const&, short int const& port);

      /** \brief Activate a socket listener */
      void add_listener(boost::shared_ptr < transporter_impl > const&, std::string const&, short int const& port);

      /** \brief Activate a socket listener by its number */
      void remove_listener(size_t number = 0);

      /** \brief Communicate a string with all peers */
      void send(const std::string&);

      /** \brief Communicate data from a stream with all peers */
      void send(std::istream&);

      /** \brief The number of connections */
      size_t number_of_connections() const;

      /** \brief Destructor */
      virtual ~transporter_impl();
  };
}
#endif

