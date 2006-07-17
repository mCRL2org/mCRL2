#ifndef TRANSPORTER_H
#define TRANSPORTER_H

#include <exception>
#include <string>
#include <list>
#include <istream>

#include <boost/noncopyable.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <transport/detail/exception.h>
#include <transport/detail/transceiver.h>
#include <transport/detail/listener.h>

/* Predeclare boost::asio::ip::address */
namespace boost {
  namespace asio {
    namespace ip {
      class address;
    }
  }
}

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

  using transceiver::basic_transceiver;
  using listener::basic_listener;

  class transporter : boost::noncopyable {
    friend class basic_transceiver;
    friend class basic_listener;

    public:

      /** \brief Convenience type to hide the shared pointer */
      typedef std::list < basic_transceiver::ptr >    connection_list;

      /** \brief Convenience type to hide the shared pointer */
      typedef std::list < basic_listener::ptr >       listener_list;

      /** \brief IP version 4 address verifier (refer to the asio documentation) */
      typedef boost::asio::ip::address                address;

      /** \brief Type for host names */
      typedef std::string                             host_name;

      /** \brief Address of the loopback interface */
      static const address                            loopback;

      /** \brief Address of any interface */
      static const address                            any;

    private:

      /** \brief To obtain mutual exclusion for operations on the list of connections */
      mutable boost::recursive_mutex lock;

      /** \brief Listeners (for socket communication etc) */
      listener_list                  listeners;

      /** \brief The list with connections */
      connection_list                connections;

    private:

      /** \brief Abstract function for the delivery of streamed data to the client program */
      virtual void deliver(std::istream&, basic_transceiver*) = 0;

      /** \brief Abstract function for the delivery of streamed data to the client program */
      virtual void deliver(std::string const&, basic_transceiver*) = 0;

      /** \brief Associate a connection with this transporter */
      void associate(const basic_transceiver::ptr&);

      /** \brief Disassociate a connection from this transporter */
      basic_transceiver::ptr disassociate(basic_transceiver*);

    public:
  
      /** \brief Default constructor with no initial connections */
      inline transporter();
 
      /** \brief Destructor */
      virtual ~transporter();
  
      /** \brief Creates direct connection to another transporter object */
      void connect(transporter&);

      /** \brief Creates socket connection to another transporter object (using a loopback connection by default) */
      void connect(address const& = loopback, const long port = 0);

      /** \brief Creates socket connection to another transporter object (using a loopback connection by default) */
      void connect(std::string const&, const long port = 0);

      /** \brief Disconnect all */
      void disconnect();

      /** \brief Disconnect connection number <|number|> */
      void disconnect(size_t number);

      /** \brief Disconnect from directly connected peer */
      void disconnect(transporter&);

      /** \brief Pass a connection through to another transporter */
      inline void relay_connection(transporter*, basic_transceiver*);

      /** \brief Activate a socket listener */
      void add_listener(address const& = any, const long port = 0);

      /** \brief Activate a socket listener by its number */
      void remove_listener(size_t number = 0);
  
      /** \brief Communicate a string with all peers */
      inline void send(const std::string&);
 
      /** \brief Communicate data from a stream with all peers */
      inline void send(std::istream&);

      /** \brief Returns an object with the local hosts name and addresses */
      static host_name get_local_host();

      /** \brief The number of active listeners */
      inline size_t number_of_listeners() const;

      /** \brief The number of active connections */
      inline size_t number_of_connections() const;
  };

  inline transporter::transporter() {
  }
 
  inline size_t transporter::number_of_listeners() const {
    return (listeners.size());
  }

  inline size_t transporter::number_of_connections() const {
    boost::recursive_mutex::scoped_lock l(lock);

    return (connections.size());
  }

  /**
   * @param d the data to be sent
   **/
  inline void transporter::send(const std::string& d) {
    boost::recursive_mutex::scoped_lock l(lock);

    for (connection_list::const_iterator i = connections.begin(); i != connections.end(); ++i) {
      (*i)->send(d);
    }
  }

  /**
   * @param s stream that contains the data to be sent
   **/
  inline void transporter::send(std::istream& s) {
    boost::recursive_mutex::scoped_lock l(lock);

    for (connection_list::const_iterator i = connections.begin(); i != connections.end(); ++i) {
      (*i)->send(s);
    }
  }

  /**
   * @param[in,out] t the transporter to relay the connection to
   * @param[in] c the transceiver that represents the local end point of the connection
   **/
  inline void transporter::relay_connection(transporter* t, basic_transceiver* c) {
    assert(t != 0);

    boost::recursive_mutex::scoped_lock l(lock);
    boost::recursive_mutex::scoped_lock tl(t->lock);

    t->associate(c->owner->disassociate(c));
  }
}

#endif

