// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transport/detail/socket_transceiver.hpp

#ifndef SOCKET_TRANSCEIVER_H__
#define SOCKET_TRANSCEIVER_H__

#include <memory>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <tipi/detail/transport/transporter.hpp>
#include <tipi/detail/transport/detail/transceiver.ipp>
#include <tipi/detail/transport/detail/socket_scheduler.hpp>

namespace transport {
  namespace listener {
    class socket_listener;
  }

  namespace transceiver {
    /**
     * \brief Transport component
     *
     * The component provides transport layer (OSI) functionality. It
     * essentially is an interface on top of TCP/UDP sockets and pipes.
     **/
    class socket_transceiver : public basic_transceiver {
      friend class transport::listener::socket_listener;
  
      private:

        /** \brief Scheduler for asynchronous socket communication */
        static boost::shared_ptr< socket_scheduler > get_scheduler();

        static const short int default_port;

      private:

        /** \brief The input buffer */
        boost::shared_array < char >                 m_buffer;

        /** \brief Scheduler for socket operations */
        boost::shared_ptr< socket_scheduler >        m_scheduler;

        /** \brief The local endpoint of a connection */
        boost::asio::ip::tcp::socket                 m_socket;
 
        /** \brief Used to make operations: read, write and disconnect mutually exclusive */
        boost::mutex                                 m_operation_lock;

        /** \brief Used to make it possible to wait for send operations to complete */
        boost::mutex                                 m_send_lock;

        /** \brief Used to make it possible to wait for send operations to complete */
        boost::condition_variable                    m_send_monitor;

        /** \brief The amount of send operations that are currently in progress */
        unsigned int                                 m_send_count;

        /** \brief Used to make sure that an object is not deleted when its methods are still executing */
        boost::weak_ptr < socket_transceiver >       m_this_ptr;

      private:

        /** \brief Constructor that connects to a port on an address */
        inline socket_transceiver(boost::shared_ptr < transporter_impl > const&);

        /** \brief Wrapper for connect() that ensures establishes that the object is not freed yet */
        void connect(boost::weak_ptr < socket_transceiver >, const std::string&, short int const&);

        /** \brief Wrapper for connect() that ensures establishes that the object is not freed yet */
        void connect(boost::weak_ptr < socket_transceiver >, const boost::asio::ip::address&, short int const&);

        /** \brief Send a string input stream to the peer */
        void send(boost::weak_ptr < socket_transceiver >, const std::string&);
  
        /** \brief Send the contents of an input stream to the peer */
        void send(boost::weak_ptr < socket_transceiver >, std::istream&);

        /** \brief Terminate the connection with the peer */
        void disconnect(boost::weak_ptr < socket_transceiver >&, basic_transceiver::ptr const&);

        /** \brief Start listening for new data */
        void activate(boost::weak_ptr < socket_transceiver >);

        /** \brief Read from the socket */
        void handle_receive(boost::weak_ptr < socket_transceiver >, const boost::system::error_code&);

        /** \brief Process results from a write operation on the socket */
        void handle_write(boost::weak_ptr < socket_transceiver >, boost::shared_array < char >, const boost::system::error_code&);

        /** \brief Actual disconnection operation */
        void handle_disconnect(boost::weak_ptr< socket_transceiver >&);

      public:

        /** \brief Factory function */
        static inline boost::shared_ptr< socket_transceiver > create(boost::shared_ptr < transporter_impl > const&);

        /** \brief Returns an object with the local hosts name and addresses */
        static boost::asio::ip::address resolve(std::string const&);

        /** \brief Returns an object with the local hosts name and addresses */
        static std::string get_local_host();

        /** \brief Send a string input stream to the peer */
        void send(const std::string&);
  
        /** \brief Send the contents of an input stream to the peer */
        void send(std::istream&);

        /** \brief Wrapper for connect() that ensures establishes that the object is not freed yet */
        void connect(const std::string&, short int const&);

        /** \brief Wrapper for connect() that ensures establishes that the object is not freed yet */
        void connect(const boost::asio::ip::address& = boost::asio::ip::address_v4::any(), short int const& = 0);

        /** \brief Terminate connection with peer */
        void disconnect(boost::shared_ptr < basic_transceiver > const&);

        /** \brief Start listening for new data */
        void activate();

        /** \brief Destructor */
        ~socket_transceiver();
    };

    /**
     * @param o a transporter to which to deliver data
     **/
    inline socket_transceiver::socket_transceiver(boost::shared_ptr < transporter_impl > const& o) :
            basic_transceiver(o), m_scheduler(get_scheduler()), m_socket(m_scheduler->io_service), m_send_count(0) {
    }

    /**
     * @param o a transporter to which to deliver data
     **/
    inline boost::shared_ptr < socket_transceiver > socket_transceiver::create(boost::shared_ptr< transporter_impl > const& o) {
      boost::shared_ptr < socket_transceiver > t(new socket_transceiver(o));

      t->m_this_ptr = boost::weak_ptr < socket_transceiver >(t);

      return (t);
    }

    /**
     * \attention Wrapper for the similar looking private function
     **/
    inline void socket_transceiver::connect(const boost::asio::ip::address& a, short int const& p) {
      connect(m_this_ptr.lock(), a, p);
    }

    /**
     * \attention Wrapper for the similar looking private function
     **/
    inline void socket_transceiver::connect(const std::string& a, short int const& p) {
      connect(m_this_ptr.lock(), a, p);
    }

    /**
     * \attention Wrapper for the similar looking private function
     **/
    inline void socket_transceiver::send(const std::string& d) {
      send(m_this_ptr.lock(), d);
    }

    /**
     * \attention Wrapper for the similar looking private function
     **/
    inline void socket_transceiver::send(std::istream& d) {
      send(m_this_ptr.lock(), d);
    }

    /**
     * \attention Wrapper for the similar looking private function
     **/
    inline void socket_transceiver::activate() {
      activate(m_this_ptr.lock());
    }

    /**
     * \attention Wrapper for the similar looking private function
     **/
    inline void socket_transceiver::disconnect(boost::shared_ptr < basic_transceiver > const& p) {
      disconnect(m_this_ptr, p);
    }
  }
}

#endif

