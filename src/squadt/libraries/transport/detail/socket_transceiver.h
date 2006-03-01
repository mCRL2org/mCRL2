#ifndef SOCKET_TRANSCEIVER_H
#define SOCKET_TRANSCEIVER_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <transport/transporter.h>
#include <transport/detail/transceiver.h>
#include <transport/detail/socket_listener.h>
#include <transport/detail/socket_scheduler.h>

namespace transport {
  namespace transceiver {

    /* Class that is used internally for direct transmitting/receiving */
    class socket_transceiver : public basic_transceiver {
      friend class transport::transporter;
      friend class transport::listener::socket_listener;
  
      private:

        /** Scheduler for asynchronous socket communication */
        static socket_scheduler scheduler;

        /** Default port for socket connections */
        static long             default_port;

        static unsigned int     input_buffer_size;
        char*                   buffer;

        /** The local endpoint of a connection */
        asio::stream_socket     socket;
 
        /** Send a string input stream to the peer */
        void send(const std::string& data);
  
        /** Send the contents of an input stream to the peer */
        void send(std::istream& data);

        /** Connect to a peer using an address and a port */
        void connect(const address& address = address::loopback(), const long port = default_port);

        /** Read from the socket */
        void handle_receive(const asio::error& e);

        /** Process results from a write operation on the socket */
        void handle_write(const asio::error& e);

        /** Start listening for new data */
        void activate();

      public:
        /** Constructor that connects to a port on an address */
        socket_transceiver(transporter& o);

        /** Terminate the connection with the peer */
        void disconnect(transporter::connection_ptr);

        /** Destructor */
        ~socket_transceiver();
    };

    inline void socket_transceiver::disconnect(transporter::connection_ptr) {
      socket.shutdown(asio::stream_socket::shutdown_both);
      socket.close();

      basic_transceiver::handle_disconnect(this);
    }
  }
}

#endif

