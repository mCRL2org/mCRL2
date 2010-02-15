// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <sstream>

#include "tipi/detail/transport/detail/socket_transceiver.hpp"

namespace transport {
  /// \internal
  namespace transceiver {

    const short int socket_transceiver::default_port = 10947;

    static const size_t input_buffer_size = 8192;

    boost::shared_ptr< socket_scheduler > socket_transceiver::get_scheduler() {
      static boost::shared_ptr< socket_scheduler > scheduler(new socket_scheduler);

      return scheduler;
    }

    /* Start listening */
    void socket_transceiver::activate(boost::weak_ptr < socket_transceiver > w) {
      boost::shared_ptr< socket_transceiver > l(w.lock());

      if (l.get() != 0) {
        using namespace boost;
        using namespace boost::asio;

        m_socket.set_option(socket_base::keep_alive(true));
        m_socket.set_option(socket_base::linger(false, 0));

        m_buffer.reset(new char[input_buffer_size + 1]);

        /* Clear buffer */
        for (size_t i = 0; i <= input_buffer_size; ++i) {
          m_buffer[i] = 0;
        }

        m_socket.async_receive(asio::buffer(m_buffer.get(), input_buffer_size), 0,
                        boost::bind(&socket_transceiver::handle_receive, this, w, _1));

        /* Make sure the scheduler is running */
        m_scheduler->run();
      }
    }

    /**
     * \param[in] a the address to connect to
     * \param[in] p the port to use
     * \param[in] w a shared pointer for this object
     *
     * \pre w.lock.get() must be `this'
     **/
    void socket_transceiver::connect(boost::weak_ptr< socket_transceiver > w, boost::asio::ip::address const& a, short int const& p) {
      boost::shared_ptr< socket_transceiver > l(w.lock());

      if (l.get() != 0) {
        using namespace boost;
        using namespace boost::asio;

        boost::system::error_code e;

        boost::mutex::scoped_lock l(m_operation_lock);

        /* Build socket connection */
        ip::tcp::endpoint endpoint(a, (p == 0) ? default_port : p);
        m_socket.connect(endpoint, e);

        /* Set socket options */
        m_socket.set_option(socket_base::keep_alive(true));
        m_socket.set_option(socket_base::linger(false, 0));

        if (!e) {
          activate();
        }
        else {
          if (e == asio::error::eof || e == asio::error::connection_reset) {
            basic_transceiver::handle_disconnect(this);
          }
          else if (e != asio::error::operation_aborted) {
            /* The safe default error handling */
            throw boost::system::system_error(e.value(), boost::system::get_system_category());
          }
        }
      }
    }

    /**
     * \param[in] w a reference to a weak pointer for this object
     * \param[in] h the host name to use
     * \param[in] p the port to use
     **/
    void socket_transceiver::connect(boost::weak_ptr< socket_transceiver > w, const std::string& h, short int const& p) {
      using namespace boost::asio;

      boost::shared_ptr< socket_transceiver > l(w.lock());

      if (l.get() != 0) {
        connect(w, resolve(h), p);
      }
    }

    /**
     * @param w a reference to a weak pointer for this object (w.lock().get() == this (or 0)
     **/
    void socket_transceiver::disconnect(boost::weak_ptr< socket_transceiver >& w, boost::shared_ptr < basic_transceiver > const&) {
      boost::shared_ptr< socket_transceiver > l(w.lock());

      if (l.get() != 0) {
        basic_transceiver::handle_disconnect(this);
      }
    }

    void socket_transceiver::handle_disconnect(boost::weak_ptr< socket_transceiver >& w) {
      boost::shared_ptr< socket_transceiver > p = w.lock();

      if (!w.expired()) {
        boost::mutex::scoped_lock s(m_send_lock);

        /* Wait until send operations complete */
        if (0 < m_send_count) {
          m_send_monitor.wait(s);
        }

        boost::mutex::scoped_lock ll(m_operation_lock);

        boost::system::error_code e;

        m_socket.close(e);

        if (e) { // An error occurred.
          throw boost::system::system_error(e.value(), boost::system::get_system_category());
        }
      }
    }

    boost::asio::ip::address socket_transceiver::resolve(std::string const& host_name) {
      using namespace boost::asio;

      return ip::tcp::resolver(get_scheduler()->io_service).resolve(
        ip::tcp::resolver::query(host_name, "",
          ip::resolver_query_base::numeric_service|
                ip::resolver_query_base::address_configured))->endpoint().address();
    }

    std::string socket_transceiver::get_local_host() {
      using namespace boost::asio;

      ip::tcp::resolver resolver(get_scheduler()->io_service);

      std::string current_host_name(ip::host_name());

      ip::tcp::resolver::iterator i(resolver.resolve(ip::tcp::resolver::query(current_host_name, "",
                        ip::resolver_query_base::numeric_service|ip::resolver_query_base::address_configured)));

      if (i == ip::tcp::resolver::iterator()) {
        return (boost::asio::ip::address_v4::loopback().to_string());
      }

      return ((*i).endpoint().address().to_string());
    }

    /**
     * @param w a reference to a weak pointer for this object (w.lock().get() == this (or 0)
     * @param e reference to an asio error object
     **/
    void socket_transceiver::handle_receive(boost::weak_ptr< socket_transceiver > w, const boost::system::error_code& e) {
      /* Object still exists, so do the receiving and delivery */
      using namespace boost;
      using namespace boost::asio;

      boost::shared_ptr< socket_transceiver > s = w.lock();

      if (!w.expired()) {
        boost::mutex::scoped_lock l(s->m_operation_lock);

        if (!e) {
          basic_transceiver::deliver(std::string(m_buffer.get()));

          /* Clear buffer */
          for (size_t i = 0; i <= input_buffer_size; ++i) {
            m_buffer[i] = 0;
          }

          m_socket.async_receive(asio::buffer(m_buffer.get(), input_buffer_size), 0,
                                  boost::bind(&socket_transceiver::handle_receive, this, w, _1));

          /* Make sure the scheduler is running */
          m_scheduler->run();
        }
        else {
          l.unlock();

          if (e == asio::error::eof || e == asio::error::connection_reset) {
            /* The safe default error handling */
            basic_transceiver::handle_disconnect(this);
          }
          else if (e == asio::error::try_again || e.value() == 11) { // value comparison is a workaround for failing erroc_code comparison
            m_socket.async_receive(asio::buffer(m_buffer.get(), input_buffer_size), 0,
              boost::bind(&socket_transceiver::handle_receive, this, w, _1));
          }
          else if (e != asio::error::operation_aborted) {
            throw boost::system::system_error(e.value(), boost::system::get_system_category());
          }
        }
      }
    }

    /**
     * @param w a reference to a weak pointer for this object (w.lock().get() == this (or 0)
     * @param e reference to an asio error object
     **/
    void socket_transceiver::handle_write(boost::weak_ptr< socket_transceiver > w, boost::shared_array < char >, const boost::system::error_code& e) {
      using namespace boost;

      boost::shared_ptr< socket_transceiver > s = w.lock();

      if (!w.expired()) {
        boost::mutex::scoped_lock k(m_send_lock);

        if (--m_send_count == 0) {
          m_send_monitor.notify_all();
        }

        /* Object still exists, so continue processing the write operation */
        if (e) {
          k.unlock();

          if (e == asio::error::eof || e == asio::error::connection_reset) {
            /* Connection was closed by peer */
            basic_transceiver::handle_disconnect(this);
          }
          else if (e != boost::asio::error::operation_aborted) {
            throw boost::system::system_error(e.value(), boost::system::get_system_category());
          }
        }
      }
    }

    /**
     * @param w a reference to a weak pointer for this object (w.lock().get() == this (or 0)
     * @param d the data that is to be sent
     **/
    void socket_transceiver::send(boost::weak_ptr< socket_transceiver > w ,const std::string& d) {
      boost::shared_ptr< socket_transceiver > l(w.lock());

      if (l.get() != 0) {
        boost::mutex::scoped_lock k(m_send_lock);

        ++m_send_count;

        boost::mutex::scoped_lock l(m_operation_lock);

        /* The null character is added so that the buffer on the receiving end does not have to be cleared every time */
        boost::shared_array< char > buffer(new char[d.size() + 1]);

        d.copy(buffer.get(), d.size(), 0);

        boost::asio::async_write(m_socket, boost::asio::buffer(buffer.get(), d.size()),
               boost::asio::transfer_all(),
               boost::bind(&socket_transceiver::handle_write, this, w, buffer, _1));
      }
    }

    /**
     * @param w a reference to a weak pointer for this object (w.lock().get() == this (or 0)
     * @param d the stream that contains the data that is to be sent
     **/
    void socket_transceiver::send(boost::weak_ptr< socket_transceiver > w, std::istream& d) {
      using namespace boost;
      using namespace boost::asio;

      boost::shared_ptr< socket_transceiver > l(w.lock());

      if (l.get() != 0) {
        boost::mutex::scoped_lock k(m_send_lock);

        ++m_send_count;

        std::ostringstream s;

        boost::mutex::scoped_lock l(m_operation_lock);

        s << d.rdbuf();

        /* The null character is added so that the buffer on the receiving end does not have to be cleared every time */
        shared_array< char > buffer(new char[s.str().size() + 1]);

        s.str().copy(buffer.get(), s.str().size(), 0);

        async_write(m_socket, asio::buffer(buffer.get(), s.str().size()),
               boost::asio::transfer_all(),
               bind(&socket_transceiver::handle_write, this, w, buffer, _1));
      }
    }

    socket_transceiver::~socket_transceiver() {
      handle_disconnect(m_this_ptr);
    }
  }
}

