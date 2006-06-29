#include <transport/transporter.h>
#include <transport/detail/socket_listener.h>
#include <transport/detail/socket_transceiver.h>

namespace transport {
  namespace listener {

    using namespace transceiver;
    using namespace listener;

    /**
     * @param m the owner of the listener
     * @param a the address to listen on
     * @param p the port to listen on
     **/
    socket_listener::socket_listener(transporter& m, const address& a, const short int p) :
      basic_listener(m), acceptor(socket_transceiver::scheduler.io_service), dispatcher(socket_transceiver::scheduler.io_service)  {
      using namespace boost::asio;

      ip::address address(a);
      ip::tcp::endpoint endpoint(address, (p == 0) ? socket_transceiver::default_port : p);

      acceptor.open(endpoint.protocol());
      acceptor.set_option(socket_base::reuse_address(true));

      try {
        acceptor.bind(endpoint);
      }
      catch (boost::asio::error e) {
        /* This should probably be logged somewhere someday */
        std::cerr << " socket layer : " << e << " (" << endpoint.address().to_string() << ":" << endpoint.port() << ")" << std::endl;

        throw;
      }

      acceptor.listen();
    }

    /**
     * @param l a shared pointer to the listerner
     **/
    void socket_listener::activate(basic_listener::ptr l) {
      /* Create a new socket transceiver that is not yet connected to the transporter */
      socket_transceiver::ptr t = socket_transceiver::create(&owner);

      acceptor.async_accept(t->socket, bind(&socket_listener::handle_accept, this, boost::asio::placeholders::error, t, l));

      /* Make sure the scheduler is running */
      socket_transceiver::scheduler.run();
    }

    /**
     * @param e asio error object
     * @param t the transceiver that will be the end point of this side of a new connection
     * @param l a shared pointer to the listerner
     **/
    void socket_listener::handle_accept(const boost::asio::error& e, transceiver::socket_transceiver::ptr t, basic_listener::ptr l) {
      if (!e) {
        socket_listener::associate(t);

        t->activate();

        t = socket_transceiver::create(&owner);

        /* Listen for new connections */
        acceptor.async_accept(t->socket, dispatcher.wrap(bind(&socket_listener::handle_accept, this, boost::asio::placeholders::error, t, l)));
      }
      else if (e == boost::asio::error::connection_aborted) {
        /* Listen for new connections */
        acceptor.async_accept(t->socket, dispatcher.wrap(bind(&socket_listener::handle_accept, this, boost::asio::placeholders::error, t, l)));
      }
      else if (e == boost::asio::error::operation_aborted) {
        return;
      }
      else {
        /* Some other error occurred, abort... */
        throw (exception::exception(exception::listener_failure));
      }

      /* Make sure the scheduler is running */
      socket_transceiver::scheduler.run();
    }
  }
}

