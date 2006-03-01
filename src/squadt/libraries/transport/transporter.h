#ifndef MESSENGER_H
#define MESSENGER_H

#include <exception>
#include <string>
#include <list>
#include <cassert>
#include <istream>

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include "detail/address.h"

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
 *  Currently a single sender can be connected to many receivers that will all
 *  receive everything that is sent (if nothing goes wrong).
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
      typedef boost::shared_ptr < basic_transceiver > connection_ptr;
      typedef std::list < connection_ptr >            connection_list;
      typedef boost::shared_ptr < basic_listener >    listener_ptr;
      typedef std::list < listener_ptr >              listener_list;

    private:

      /** listeners (for socket communication etc) */
      listener_list   listeners;

      /** The list with connections */
      connection_list connections;

      /** Abstract function for the delivery of streamed data to the client program */
      virtual void deliver(std::istream&) = 0;

      /** Abstract function for the delivery of streamed data to the client program */
      virtual void deliver(std::string&) = 0;

      /** Creates direct connection to another transporter object */
      void connect(basic_transceiver*);
  
    public:
  
      /** Default constructor with no initial connections */
      transporter();
 
      /** Destructor */
      virtual ~transporter();
  
      /** Creates direct connection to another transporter object */
      void connect(transporter&);

      /** Creates socket connection to another transporter object (using a loopback connection by default) */
      void connect(const address& = address::loopback(), const long port = default_port);
  
      /** Disconnect connection number <|number|> */
      void disconnect(size_t number = 0);

      /** Disconnect connection number <|number|> */
      void disconnect(transporter&);

      /** Activate a socket listener (using a loopback connection by default) */
      void add_listener(const address& = address::loopback(), const long port = default_port);

      /** Activate a socket listener by its number */
      void remove_listener(size_t number = 0);
  
      /** Communicate a string with all peers */
      void send(const std::string&);
 
      /** Communicate data from a stream with all peers */
      void send(std::istream&);

      /** The number of active listeners */
      inline size_t number_of_listeners();

      /** The number of active connections */
      inline size_t number_of_connections();
  };

  inline size_t transporter::number_of_listeners() {
    return (listeners.size());
  }

  inline size_t transporter::number_of_connections() {
    return (connections.size());
  }

  class exception : public std::exception {
    public:
      typedef enum {
        listener_failure,
        transceiver_closed
      } exception_type;

      virtual exception_type type() = 0;
  };
}

#endif

