#include <cassert>
#include <algorithm>

#include <transporter.h>

#include <detail/socket_listener.h>
#include <detail/direct_transceiver.h>
#include <detail/socket_transceiver.h>

namespace transport {
  using namespace listener;
  using namespace transceiver;

  transporter::transporter() {
  }
 
  transporter::~transporter() {
    using namespace boost;

    /* Disconnect all peers */
    connections.clear();
  
    /* Clean up listeners */
    std::for_each(listeners.begin(),listeners.end(),bind(&Listener::shutdown, bind(&shared_ptr < Listener >::get, _1)));
  }

  void transporter::connect(transporter& peer) {
    ConnectionPtr connection = ConnectionPtr(new DirectTransceiver(*this));

    peer.connections.push_back(connection);
    connections.push_back(ConnectionPtr(new DirectTransceiver(peer, reinterpret_cast < DirectTransceiver* > (connection.get()))));
  }

  void transporter::connect(const address& address, const long port) {
    ConnectionPtr new_connection(new SocketTransceiver(*this));

    reinterpret_cast < SocketTransceiver* > (new_connection.get())->connect(address, port);

    connections.push_back(new_connection);
  }

  void transporter::disconnect(size_t number) {
    assert(number < connections.size());

    ConnectionList::iterator i = connections.begin();
  
    while (0 < number) {
      assert(i != connections.end());

      --number;

      ++i;
    }

    (*i)->disconnect(*i);
  }

  void transporter::disconnect(transporter& m) {
    ConnectionList::iterator i = connections.begin();
  
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

  /* May throw ListenerException* */
  void transporter::add_listener(const address& address, const long port) {
    ListenerPtr new_listener(new SocketListener(*this, address, port));

    listeners.push_back(new_listener);

    reinterpret_cast < SocketListener* > (new_listener.get())->activate(new_listener);
  }

  /* number < listeners.size() */
  void transporter::remove_listener(size_t number) {
    assert(number < listeners.size());

    ListenerList::iterator i = listeners.begin();

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
    ConnectionList::iterator i = connections.begin();
    ConnectionList::iterator b = connections.end();
  
    while (i != b) {
      (*i)->send(string);

      ++i;
    }
  }

  void transporter::send(std::istream& stream) {
    ConnectionList::iterator i = connections.begin();
    ConnectionList::iterator b = connections.end();

    while (i != b) {
      (*i)->send(stream);
  
      ++i;
    }
  }
}

