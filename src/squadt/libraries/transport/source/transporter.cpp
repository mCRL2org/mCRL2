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

  void transporter::connect(transporter& peer) {
    connection_ptr connection = connection_ptr(new direct_transceiver(*this));

    peer.connections.push_back(connection);
    connections.push_back(connection_ptr(new direct_transceiver(peer, reinterpret_cast < direct_transceiver* > (connection.get()))));
  }

  void transporter::connect(const address& address, const long port) {
    connection_ptr new_connection(new socket_transceiver(*this));

    reinterpret_cast < socket_transceiver* > (new_connection.get())->connect(address, port);

    connections.push_back(new_connection);
  }

  void transporter::disconnect(size_t number) {
    assert(number < connections.size());

    connection_list::iterator i = connections.begin();
  
    while (0 < number) {
      assert(i != connections.end());

      --number;

      ++i;
    }

    (*i)->disconnect(*i);
  }

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

  /* May throw listenerException* */
  void transporter::add_listener(const address& address, const long port) {
    listener_ptr new_listener(new socket_listener(*this, address, port));

    listeners.push_back(new_listener);

    reinterpret_cast < socket_listener* > (new_listener.get())->activate(new_listener);
  }

  /* number < listeners.size() */
  void transporter::remove_listener(size_t number) {
    assert(number < listeners.size());

    listener_list::iterator i = listeners.begin();

    while (0 < number) {
      --number;

      ++i;
    }

    if (i != listeners.end()) {
      (*i)->shutdown();

      listeners.erase(i);
    }
  }

  void transporter::send(const std::string& string) {
    connection_list::iterator i = connections.begin();
    connection_list::iterator b = connections.end();
  
    while (i != b) {
      (*i)->send(string);

      ++i;
    }
  }

  void transporter::send(std::istream& stream) {
    connection_list::iterator i = connections.begin();
    connection_list::iterator b = connections.end();

    while (i != b) {
      (*i)->send(stream);
  
      ++i;
    }
  }
}

