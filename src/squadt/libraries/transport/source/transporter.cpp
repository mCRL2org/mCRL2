#include <cassert>
#include <algorithm>

#include <transport/transporter.h>

#include <transport/detail/socket_listener.h>
#include <transport/detail/direct_transceiver.h>
#include <transport/detail/socket_transceiver.h>

namespace transport {

  using namespace transceiver;
  using namespace listener;

  transporter::transporter() {
  }
 
  transporter::~transporter() {
    using namespace boost;

    /* Disconnect all peers */
    connections.clear();
  
    /* Clean up listeners */
    std::for_each(listeners.begin(),listeners.end(),bind(&basic_listener::shutdown, bind(&shared_ptr < basic_listener >::get, _1)));
  }

  /**
   * @param p the peer to connect to
   **/
  void transporter::connect(transporter& p) {
    connection_ptr connection = connection_ptr(new direct_transceiver(*this));

    p.connections.push_back(connection);
    connections.push_back(connection_ptr(new direct_transceiver(p, reinterpret_cast < direct_transceiver* > (connection.get()))));
  }

  /**
   * @param a an address
   * @param p a port
   **/
  void transporter::connect(const address& a, const long p) {
    connection_ptr c(new socket_transceiver(*this));

    reinterpret_cast < socket_transceiver* > (c.get())->connect(a, p);

    connections.push_back(c);
  }

  /**
   * @param h a hostname address
   * @param p a port
   **/
  void transporter::connect(const std::string& h, const long p) {
    connection_ptr c(new socket_transceiver(*this));

    reinterpret_cast < socket_transceiver* > (c.get())->connect(h, p);

    connections.push_back(c);
  }

  /**
   * @param n the number of the connection that is to be closed
   **/
  void transporter::disconnect(size_t n) {
    assert(n < connections.size());

    connection_list::iterator i = connections.begin();
  
    while (0 < n) {
      assert(i != connections.end());

      --n;

      ++i;
    }

    (*i)->disconnect(*i);
  }

  /**
   * @param m the directly connected peer
   **/
  void transporter::disconnect(transporter& m) {
    connection_list::iterator i = connections.begin();
  
    while (i != connections.end()) {
      if (&(*i)->owner == &m) {
        break;
      }

      ++i;
    }

    if (i != connections.end()) {
      (*i)->disconnect(*i);
    }
  }

  /**
   * @param a an address
   * @param p a port
   **/
  void transporter::add_listener(const address& a, const long p) {
    listener_ptr new_listener(new socket_listener(*this, a, p));

    listeners.push_back(new_listener);

    reinterpret_cast < socket_listener* > (new_listener.get())->activate(new_listener);
  }

  /**
   * @param the number of the listener that is to be removed
   **/
  void transporter::remove_listener(size_t n) {
    assert(n < listeners.size());

    listener_list::iterator i = listeners.begin();

    while (0 < n) {
      --n;

      ++i;
    }

    if (i != listeners.end()) {
      (*i)->shutdown();

      listeners.erase(i);
    }
  }

  /**
   * @param d the data to be sent
   **/
  void transporter::send(const std::string& d) {
    connection_list::iterator i = connections.begin();
    connection_list::iterator b = connections.end();
  
    while (i != b) {
      (*i)->send(d);

      ++i;
    }
  }

  /**
   * @param s stream that contains the data to be sent
   **/
  void transporter::send(std::istream& s) {
    connection_list::iterator i = connections.begin();
    connection_list::iterator b = connections.end();

    while (i != b) {
      (*i)->send(s);
  
      ++i;
    }
  }
}

