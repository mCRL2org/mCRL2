// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "boost/format.hpp"

#include <tipi/detail/transport/transporter.hpp>
#include <tipi/detail/transport/detail/socket_listener.hpp>
#include <tipi/detail/transport/detail/socket_transceiver.hpp>

namespace transport {
  /// \cond INTERNAL_DOCS
  namespace listener {

    using namespace transceiver;
    using namespace listener;

    /**
     * @param m the owner of the listener
     * @param a the address to listen on
     * @param p the port to listen on
     **/
    socket_listener::socket_listener(boost::shared_ptr < transport::transporter_impl > const& m, const boost::asio::ip::address& a, short int const& p) :
      basic_listener(m), scheduler(socket_transceiver::get_scheduler()), acceptor(scheduler->io_service), dispatcher(scheduler->io_service)  {

      using namespace boost;
      using namespace boost::asio;

      ip::tcp::endpoint endpoint(a, (p == 0) ? socket_transceiver::default_port : p);

      try {
        acceptor.open(endpoint.protocol());

        if ((a.is_v4() && endpoint.address() == ip::address_v4::loopback()) ||
            (a.is_v6() && endpoint.address() == ip::address_v6::loopback())) {
          acceptor.set_option(socket_base::do_not_route(true));
        }

        acceptor.set_option(socket_base::reuse_address(true));
        acceptor.bind(endpoint);
        acceptor.listen();
      }
      catch (std::exception& e) {
        /* This should probably be logged somewhere someday */
        throw std::runtime_error(str(format("Socket setup on %s:%u failed with error `%s'") % endpoint.address().to_string() % endpoint.port() % e.what()));
      }
    }

    /**
     * @param l a shared pointer to the listerner
     **/
    void socket_listener::activate(boost::shared_ptr< basic_listener > l) {
      /* Create a new socket transceiver that is not yet connected to the transporter */
      boost::shared_ptr < socket_transceiver > t(socket_transceiver::create(owner.lock()));

      acceptor.async_accept(t->m_socket, bind(&socket_listener::handle_accept, this, boost::asio::placeholders::error, t, l));

      /* Make sure the scheduler is running */
      scheduler->run();
    }

    /**
     * @param e asio error object
     * @param t the transceiver that will be the end point of this side of a new connection
     * @param l a shared pointer to the listerner
     **/
    void socket_listener::handle_accept(const boost::system::error_code& e, boost::shared_ptr< transceiver::socket_transceiver > t, boost::shared_ptr< basic_listener > l) {
      if (!e) {
        boost::shared_ptr< transceiver::socket_transceiver > new_t(socket_transceiver::create(owner.lock()));

        /* Listen for new connections */
        acceptor.async_accept(new_t->m_socket, dispatcher.wrap(bind(&socket_listener::handle_accept, this, boost::asio::placeholders::error, new_t, l)));

        socket_listener::associate(t);

        t->activate();
      }
      else if (e == boost::asio::error::connection_aborted) {
        /* Listen for new connections */
        acceptor.async_accept(t->m_socket, dispatcher.wrap(bind(&socket_listener::handle_accept, this, boost::asio::placeholders::error, t, l)));
      }
      else if (e == boost::asio::error::operation_aborted) {
        return;
      }
      else {
        using namespace boost;

        /* Some other error occurred, abort... */
        throw std::runtime_error(str(format("Socket connection failed with error `%s'") % e.message()));
      }

      /* Make sure the scheduler is running */
      scheduler->run();
    }
  }
  /// \endcond
}

