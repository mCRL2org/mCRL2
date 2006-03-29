#include <iostream>
#include <sstream>

#include <boost/bind/protect.hpp>

#include <transport/detail/socket_transceiver.h>

namespace transport {
  namespace transceiver {

    unsigned int              socket_transceiver::input_buffer_size = 2048;

    socket_scheduler          socket_transceiver::scheduler;

    asio::ipv4::host_resolver socket_transceiver::resolver(scheduler.demuxer);

    long socket_transceiver::default_port = 10946;

    /* Start listening */
    void socket_transceiver::activate(socket_transceiver::ptr w) {
      if (w.get() != 0) {
        using namespace asio;
        using namespace boost;

        socket.set_option(socket_base::keep_alive(true));
        socket.set_option(socket_base::linger(false, 0));

        socket.async_receive(asio::buffer(buffer, input_buffer_size), 0,
                          boost::bind(&socket_transceiver::activate_handler, w, 
                                  boost::protect(boost::bind(&socket_transceiver::handle_receive, this, _1, _2)),
                                        placeholders::error));
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
                          boost::bind(&socket_transceiver::activate_handler, w, 
                                  boost::protect(boost::bind(&socket_transceiver::handle_receive, this, _1, _2)),
                                        placeholders::error));

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
       
        resolver.get_host_by_name(host, h);
       
        connect(host.address(0), p);
      }
    }

    void socket_transceiver::disconnect(basic_transceiver::ptr, socket_transceiver::ptr w) {
      if (w.get() != 0) {
        boost::mutex::scoped_lock l(operation_lock);
       
        socket.shutdown(asio::stream_socket::shutdown_both);
        socket.close();
       
        basic_transceiver::handle_disconnect(this);
      }
    }

    socket_transceiver::host socket_transceiver::get_local_host() {
      asio::ipv4::host h;

      resolver.get_local_host(h);

      return (h);
    }

    /**
     * @param o shared pointer to the object on which the handler will operate
     * @param h the handler function
     * @param e reference to an asio error object
     **/
    void socket_transceiver::activate_handler(socket_transceiver::wptr o, handler_t h, const asio::error& e) {
      socket_transceiver::ptr s = o.lock();

      if (!o.expired()) {
        boost::mutex::scoped_lock l(s->operation_lock);

        h(e, o);
      }
    }

    /**
     * @param e reference to an asio error object
     **/
    void socket_transceiver::handle_receive(const asio::error& e, socket_transceiver::wptr& w) {
      /* Object still exists, so do the receiving and delivery */
      using namespace asio;

      if (!e) {
        basic_transceiver::deliver(boost::cref(std::string(buffer)));

        socket.async_receive(asio::buffer(buffer, input_buffer_size), 0,
                        boost::bind(&socket_transceiver::activate_handler, w, 
                                boost::protect(boost::bind(&socket_transceiver::handle_receive, this, _1, _2)),
                                        placeholders::error));

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
    void socket_transceiver::handle_write(const asio::error& e, socket_transceiver::wptr& w) {
      /* Object still exists, so continue processing the write operation */
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
    void socket_transceiver::send(const std::string& d, socket_transceiver::ptr w) {
      if (w.get() != 0) {
        using namespace asio;
        using namespace boost;
       
        boost::mutex::scoped_lock l(operation_lock);
       
        asio::async_write(socket, asio::buffer(d.c_str(), d.length() + 1), 
                        boost::bind(&socket_transceiver::activate_handler, w, 
                                boost::protect(boost::bind(&socket_transceiver::handle_write, this, _1, _2)),
                                        placeholders::error));
      }
    }

    /**
     * @param d the stream that contains the data that is to be sent
     **/
    void socket_transceiver::send(std::istream& d, socket_transceiver::ptr w) {
      using namespace asio;
      using namespace boost;

      boost::mutex::scoped_lock l(operation_lock);

      std::ostringstream s;

      s << d.rdbuf();

      asio::async_write(socket, asio::buffer(s.str().c_str(), s.str().length() + 1),
                      boost::bind(&socket_transceiver::activate_handler, w, 
                              boost::protect(boost::bind(&socket_transceiver::handle_write, this, _1, _2)),
                                      placeholders::error));
    }
  }
}

