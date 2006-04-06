#include <iostream>
#include <sstream>

#include <transport/detail/socket_transceiver.h>

#include <boost/ref.hpp>

namespace transport {
  namespace transceiver {

    unsigned int              socket_transceiver::input_buffer_size = 2048;

    socket_scheduler          socket_transceiver::scheduler;

    asio::ipv4::host_resolver socket_transceiver::resolver(scheduler.io_service);

    long socket_transceiver::default_port = 10946;

    /* Start listening */
    void socket_transceiver::activate(socket_transceiver::ptr w) {
      if (w.get() != 0) {
        using namespace asio;
        using namespace boost;

        socket.set_option(socket_base::keep_alive(true));
        socket.set_option(socket_base::linger(false, 0));

        socket.async_receive(asio::buffer(buffer, input_buffer_size), 0, 
                        boost::bind(&socket_transceiver::handle_receive, this, w, _1));
      }
    }

    /**
     * @param a the addess to connect to
     * @param p the port to use
     * @param w a shared pointer for this object
     *
     * \pre w.lock.get() must be `this'
     **/
    void socket_transceiver::connect(const address& a, const long p, socket_transceiver::ptr w) {
      if (w.get() != 0) {
        using namespace asio;

        error e;

        boost::mutex::scoped_lock l(operation_lock);

        /* Build socket connection */
        ipv4::tcp::endpoint endpoint((p == 0) ? default_port : p, a);

        socket.connect(endpoint, assign_error(e));

        /* Set socket options */
        socket.set_option(socket_base::keep_alive(true));
        socket.set_option(socket_base::linger(false, 0));

        if (!e) {
          socket.async_receive(asio::buffer(buffer, input_buffer_size), 0,
                          boost::bind(&socket_transceiver::handle_receive, this, w, _1));

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
    }

    /**
     * @param w a reference to a weak pointer for this object
     * @param h the host name to use
     * @param p the port to use
     **/
    void socket_transceiver::connect(const std::string& h, const long p, socket_transceiver::ptr w) {
      if (w.get() != 0) {
        asio::ipv4::host host;

        resolver.by_name(host, h);

        connect(host.address(0), p, w);
      }
    }

    void socket_transceiver::disconnect(basic_transceiver::ptr, socket_transceiver::ptr w) {
      if (w.get() != 0) {
        boost::mutex::scoped_lock l(operation_lock);

        socket.close();

        basic_transceiver::handle_disconnect(this);
      }
    }

    socket_transceiver::host socket_transceiver::get_local_host() {
      asio::ipv4::host h;

      resolver.local(h);

      return (h);
    }

    /**
     * @param e reference to an asio error object
     **/
    void socket_transceiver::handle_receive(socket_transceiver::wptr w, const asio::error& e) {
      /* Object still exists, so do the receiving and delivery */
      using namespace asio;

      socket_transceiver::ptr s = w.lock();

      if (!w.expired()) {
        boost::mutex::scoped_lock l(s->operation_lock);

        if (!e) {
          basic_transceiver::deliver(boost::cref(std::string(buffer)));
       
          socket.async_receive(asio::buffer(buffer, input_buffer_size), 0,
                                  boost::bind(&socket_transceiver::handle_receive, this, w, _1));
       
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
    }

    /**
     * @param e reference to an asio error object
     **/
    void socket_transceiver::handle_write(socket_transceiver::wptr w, const asio::error& e) {
      socket_transceiver::ptr s = w.lock();

      if (!w.expired()) {
        /* Object still exists, so continue processing the write operation */
        if (e == asio::error::eof) {
          /* Connection was closed by peer */
          handle_disconnect(this);
        }
        else if (e != asio::error::success && e != asio::error::operation_aborted) {
          throw (exception(exception_identifier::transceiver_failure));
        }
      }
    }

    /**
     * @param d the data that is to be sent
     **/
    void socket_transceiver::send(const std::string& d, socket_transceiver::ptr w) {
      if (w.get() != 0) {
        boost::mutex::scoped_lock l(operation_lock);

        asio::async_write(socket, asio::buffer(d.c_str(), d.length() + 1), 
                      boost::bind(&socket_transceiver::handle_write, this, w, _1));
      }
    }

    /**
     * @param d the stream that contains the data that is to be sent
     **/
    void socket_transceiver::send(std::istream& d, socket_transceiver::ptr w) {
      std::ostringstream s;

      if (w.get() != 0) {
        boost::mutex::scoped_lock l(operation_lock);

        s << d.rdbuf();

        asio::async_write(socket, asio::buffer(s.str().c_str(), s.str().length() + 1), 
                      boost::bind(&socket_transceiver::handle_write, this, w, _1));
      }
    }
  }
}

