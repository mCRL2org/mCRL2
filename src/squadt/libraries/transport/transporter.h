#ifndef TRANSPORTER_H
#define TRANSPORTER_H

#include <exception>
#include <string>
#include <list>
#include <istream>

#include <boost/asio/ipv4/address.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <transport/detail/exception.h>

/*
 * Socket/Direct communication abstraction
 *
 * for transparent communication, via sockets or direct, between senders and
 * receivers
 *
 *  Direct:
 *   - the sender directly calls the deliver routine of a connected transporter object
 *
 *  Sockets:
 *   - the sender puts the data on a socket, when the data arrives, it is
 *   passed in the very same way to the receiving context.
 *
 *  Currently, unless exceptions occur, a single sender can be connected to
 *  many receivers that will all receive everything that is sent.
 */

namespace transport {
  namespace listener {
    class basic_listener;
  }

  namespace transceiver {
    class basic_transceiver;
  }

  using transceiver::basic_transceiver;
  using listener::basic_listener;

  class transporter : boost::noncopyable {
    friend class basic_transceiver;
    friend class basic_listener;

    public:
      /** Convenience type to hide the shared pointer */
      typedef boost::shared_ptr < basic_transceiver > connection_ptr;

      /** Convenience type to hide the shared pointer */
      typedef std::list < connection_ptr >            connection_list;

      /** Convenience type to hide the shared pointer */
      typedef boost::shared_ptr < basic_listener >    listener_ptr;

      /** Convenience type to hide the shared pointer */
      typedef std::list < listener_ptr >              listener_list;

      /** IP version 4 address verifier (refer to the asio documentation) */
      typedef asio::ipv4::address                     address;

    private:

      /** \brief listeners (for socket communication etc) */
      listener_list   listeners;

      /** \brief The list with connections */
      connection_list connections;

      /** \brief Abstract function for the delivery of streamed data to the client program */
      virtual void deliver(std::istream&) = 0;

      /** \brief Abstract function for the delivery of streamed data to the client program */
      virtual void deliver(std::string&) = 0;

      /** \brief Creates direct connection to another transporter object */
      void connect(basic_transceiver*);
  
    public:
  
      /** \brief Default constructor with no initial connections */
      transporter();
 
      /** \brief Destructor */
      virtual ~transporter();
  
      /** \brief Creates direct connection to another transporter object */
      void connect(transporter&);

      /** \brief Creates socket connection to another transporter object (using a loopback connection by default) */
      void connect(const address& = address::loopback(), const long port = 0);

      /** \brief Creates socket connection to another transporter object (using a loopback connection by default) */
      void connect(const std::string& host_name, const long port = 0);

      /** \brief Disconnect connection number <|number|> */
      void disconnect(size_t number = 0);

      /** \brief Disconnect connection number <|number|> */
      void disconnect(transporter&);

      /** \brief Activate a socket listener (using a loopback connection by default) */
      void add_listener(const address& = address::loopback(), const long port = 0);

      /** \brief Activate a socket listener by its number */
      void remove_listener(size_t number = 0);
  
      /** \brief Communicate a string with all peers */
      void send(const std::string&);
 
      /** \brief Communicate data from a stream with all peers */
      void send(std::istream&);

      /** \brief The number of active listeners */
      inline size_t number_of_listeners();

      /** \brief The number of active connections */
      inline size_t number_of_connections();
  };

  inline size_t transporter::number_of_listeners() {
    return (listeners.size());
  }

  inline size_t transporter::number_of_connections() {
    return (connections.size());
  }
}

#endif

