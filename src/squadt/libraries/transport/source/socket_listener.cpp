#include <transport/transporter.h>
#include <transport/detail/socket_listener.h>
#include <transport/detail/socket_transceiver.h>

namespace transport {
  namespace listener {

    /** Constructor */
    SocketListener::SocketListener(transporter& m, const address& address, const long port) :
      Listener(m), acceptor(SocketTransceiver::scheduler.demuxer), dispatcher(SocketTransceiver::scheduler.demuxer) {
      using namespace asio;
      using namespace boost;

      ipv4::tcp::endpoint endpoint(port, ipv4::address(address));

      acceptor.open(endpoint.protocol());
      acceptor.set_option(socket_acceptor::reuse_address(true));
      acceptor.bind(endpoint);
      acceptor.listen();
    }

    void SocketListener::activate(transporter::ListenerPtr l) {
      /* Create a new socket transceiver that is not yet connected to the transporter */
      boost::shared_ptr < SocketTransceiver > t(new SocketTransceiver(owner));

      acceptor.async_accept(t->socket, dispatcher.wrap(bind(&SocketListener::handle_accept, this, asio::placeholders::error, t, l)));

      /* Make sure the scheduler is running */
      SocketTransceiver::scheduler.run();
    }

    void SocketListener::handle_accept(const asio::error& e, boost::shared_ptr < SocketTransceiver > t, transporter::ListenerPtr l) {
      if (!e) {
        SocketListener::associate(t);

        t->activate();

        t = boost::shared_ptr < SocketTransceiver > (new SocketTransceiver(owner));

        /* Listen for new connections */
        acceptor.async_accept(t->socket, dispatcher.wrap(bind(&SocketListener::handle_accept, this, asio::placeholders::error, t, l)));
      }
      else if (e == asio::error::connection_aborted) {
        /* Listen for new connections */
        acceptor.async_accept(t->socket, dispatcher.wrap(bind(&SocketListener::handle_accept, this, asio::placeholders::error, t, l)));
      }
      else if (e == asio::error::operation_aborted) {
        return;
      }
      else {
        /* Some other error occurred, abort... */
        std::cerr << "Error : " << e << std::endl;

        throw (new listener::exception(this));
      }

      /* Make sure the scheduler is running */
      SocketTransceiver::scheduler.run();
    }
  }
}

