#include <iostream>
#include <sstream>

#include <transport/detail/socket_transceiver.h>

namespace transport {
  namespace transceiver {

    unsigned int              socket_transceiver::input_buffer_size = 2048;

    socket_scheduler          socket_transceiver::scheduler;

    asio::ipv4::host_resolver socket_transceiver::resolver(scheduler.demuxer);

    long socket_transceiver::default_port = 10946;

    /**
     * @param o a transporter to deliver data to
     **/
    socket_transceiver::socket_transceiver(transporter& o) : basic_transceiver(o), socket(scheduler.demuxer) {
      using namespace asio;

      buffer = new char[input_buffer_size];
    }

    /* Start listening */
    void socket_transceiver::activate() {
      using namespace asio;
      using namespace boost;

      socket.set_option(socket_base::keep_alive(true));
      socket.set_option(socket_base::linger(false, 0));

      socket.async_receive(asio::buffer(buffer, input_buffer_size), 0, bind(&socket_transceiver::handle_receive, this, placeholders::error));
    }

    /**
     * @param h the host name to use
     * @param p the port to use
     **/
    void socket_transceiver::connect(const std::string& h, const long p) {
      asio::ipv4::host host;

      resolver.get_host_by_name(host, h);

      connect(host.address(0), p);
    }

    socket_transceiver::host socket_transceiver::get_local_host() {
      asio::ipv4::host h;

      resolver.get_local_host(h);

      return (h);
    }

    void socket_transceiver::connect(const address& a, const long p) {
      using namespace asio;

      error e;

      /* Build socket connection */
      ipv4::tcp::endpoint endpoint((p == 0) ? default_port : p, a);

      socket.connect(endpoint, assign_error(e));
      socket.set_option(socket_base::keep_alive(true));
      socket.set_option(socket_base::linger(false, 0));

      if (!e) {
        socket.async_receive(asio::buffer(buffer, input_buffer_size), 0, boost::bind(&socket_transceiver::handle_receive, this, placeholders::error));

        /* Make sure the scheduler is running */
        scheduler.run();
      }
      else {
        if (e == asio::error::eof) {
          handle_disconnect(this);
        }
        else if (e != asio::error::operation_aborted) {
          /* The safe default error handling */
          throw (exception(exception_identifier::transceiver_failure));
        }
      }
    }

    /**
     * @param e reference to an asio error object
     **/
    void socket_transceiver::handle_receive(const asio::error& e) {
      using namespace asio;

      if (!e) {
        std::istringstream stream;

        stream.str(std::string(buffer));

        basic_transceiver::deliver(stream);

        socket.async_receive(asio::buffer(buffer, input_buffer_size), 0, boost::bind(&socket_transceiver::handle_receive, this, placeholders::error));

        /* Make sure the scheduler is running */
        scheduler.run();
      }
      else {
        if (e == asio::error::eof) {
          /* The safe default error handling */
          handle_disconnect(this);
        }
        else if (e != asio::error::operation_aborted) {
          throw (exception(exception_identifier::transceiver_failure));
        }
      }
    }

    /**
     * @param e reference to an asio error object
     **/
    void socket_transceiver::handle_write(const asio::error& e) {
      if (e == asio::error::eof) {
        /* Connection was closed by peer */
        handle_disconnect(this);
      }
      else if (e != asio::error::success && e != asio::error::operation_aborted) {
        throw (exception(exception_identifier::transceiver_failure));
      }
    }

    /**
     * @param d the data that is to be sent
     **/
    void socket_transceiver::send(const std::string& d) {
      using namespace asio;
      using namespace boost;

      asio::async_write(socket, asio::buffer(d.c_str(), d.length() + 1), bind(&socket_transceiver::handle_write, this, placeholders::error));
    }

    /**
     * @param d the stream that contains the data that is to be sent
     **/
    void socket_transceiver::send(std::istream& d) {
      using namespace asio;
      using namespace boost;

      std::ostringstream temporary;

      temporary << d.rdbuf();

      asio::async_write(socket, asio::buffer(temporary.str().c_str(), temporary.str().length() + 1), bind(&socket_transceiver::handle_write, this, placeholders::error));
    }

    socket_transceiver::~socket_transceiver() {
      delete[] buffer;
    }
  }
}

