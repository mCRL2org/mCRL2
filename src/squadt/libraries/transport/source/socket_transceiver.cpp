#include <iostream>
#include <sstream>

#include <transport/detail/socket_transceiver.h>

namespace transport {
  namespace transceiver {

    unsigned int  SocketTransceiver::input_buffer_size = 2048;

    SocketScheduler          SocketTransceiver::scheduler;

    /* Constructor */
    SocketTransceiver::SocketTransceiver(transporter& o) : Transceiver(o), socket(scheduler.demuxer) {
      using namespace asio;

      buffer = new char[input_buffer_size];
    }

    /* Start listening */
    void SocketTransceiver::activate() {
      using namespace asio;
      using namespace boost;

      socket.set_option(socket_base::keep_alive(true));
      socket.set_option(socket_base::linger(false, 0));

      socket.async_receive(asio::buffer(buffer, input_buffer_size), 0, bind(&SocketTransceiver::handle_receive, this, placeholders::error));
    }

    void SocketTransceiver::connect(const address& address, const long port) {
      using namespace asio;

      /* Build socket connection */
      ipv4::tcp::endpoint endpoint(port, ipv4::address(address));
      error e;

      socket.connect(endpoint, assign_error(e));
      socket.set_option(socket_base::keep_alive(true));
      socket.set_option(socket_base::linger(false, 0));

      if (!e) {
        socket.async_receive(asio::buffer(buffer, input_buffer_size), 0, boost::bind(&SocketTransceiver::handle_receive, this, placeholders::error));

        /* Make sure the scheduler is running */
        scheduler.run();
      }
      else {
        if (e == asio::error::eof) {
          handle_disconnect(this);
        }
        else if (e != asio::error::operation_aborted) {
          /* The safe default error handling */
          std::cerr << "Error : " << e << std::endl;

          throw (new exception(this));
        }
      }
    }

    void SocketTransceiver::handle_receive(const asio::error& e) {
      using namespace asio;

      if (!e) {
        std::istringstream stream;

        stream.str(std::string(buffer));

        Transceiver::deliver(stream);

        socket.async_receive(asio::buffer(buffer, input_buffer_size), 0, boost::bind(&SocketTransceiver::handle_receive, this, placeholders::error));

        /* Make sure the scheduler is running */
        scheduler.run();
      }
      else {
        if (e == asio::error::eof) {
          /* The safe default error handling */
          handle_disconnect(this);
        }
        else if (e != asio::error::operation_aborted) {
          std::cerr << "Error : " << e << std::endl;

          throw (new exception(this));
        }
      }
    }

    void SocketTransceiver::handle_write(const asio::error& e) {
      if (e == asio::error::eof) {
        /* Connection was closed by peer */
        handle_disconnect(this);
      }
      else if (e != asio::error::success && e != asio::error::operation_aborted) {
        std::cerr << "Error : " << e << std::endl;

        throw (new exception(this));
      }
    }

    /* Send via socket */
    void SocketTransceiver::send(const std::string& data) {
      using namespace asio;
      using namespace boost;

      asio::async_write(socket, asio::buffer(data.c_str(), data.length() + 1), bind(&SocketTransceiver::handle_write, this, placeholders::error));
    }

    /* Send via socket */
    void SocketTransceiver::send(std::istream& data) {
      using namespace asio;
      using namespace boost;

      std::ostringstream temporary;

      temporary << data.rdbuf();

      asio::async_write(socket, asio::buffer(temporary.str().c_str(), temporary.str().length() + 1), bind(&SocketTransceiver::handle_write, this, placeholders::error));
    }

    /* Destructor */
    SocketTransceiver::~SocketTransceiver() {
      delete[] buffer;
    }
  }
}

