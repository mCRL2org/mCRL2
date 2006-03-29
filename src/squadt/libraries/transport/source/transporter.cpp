#include <algorithm>
#include <functional>

#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include <transport/detail/transceiver.tcc>
#include <transport/detail/socket_listener.h>
#include <transport/detail/direct_transceiver.h>
#include <transport/detail/socket_transceiver.h>

namespace transport {

  using namespace transceiver;
  using namespace listener;

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
    basic_transceiver::ptr t(new direct_transceiver(this));

    boost::recursive_mutex::scoped_lock pl(p.lock);
    p.connections.push_back(t);

    boost::recursive_mutex::scoped_lock l(lock);
    connections.push_back(basic_transceiver::ptr(new direct_transceiver(&p, reinterpret_cast < direct_transceiver* > (t.get()))));
  }

  /**
   * @param a an address
   * @param p a port
   **/
  void transporter::connect(const address& a, const long p) {
    basic_transceiver::ptr c = socket_transceiver::create(this);

    boost::recursive_mutex::scoped_lock l(lock);

    reinterpret_cast < socket_transceiver* > (c.get())->connect(a, p);

    connections.push_back(c);
  }

  /**
   * @param h a hostname address
   * @param p a port
   **/
  void transporter::connect(const std::string& h, const long p) {
    basic_transceiver::ptr c = socket_transceiver::create(this);

    boost::recursive_mutex::scoped_lock l(lock);

    reinterpret_cast < socket_transceiver* > (c.get())->connect(h, p);

    connections.push_back(c);
  }

  /**
   * @param t the connection to associate with this transporter
   **/
  void transporter::associate(const basic_transceiver::ptr& t) {
    boost::recursive_mutex::scoped_lock l(lock);

    const basic_transceiver* p = t.get();

    connection_list::iterator i = std::find_if(connections.begin(), connections.end(),
                      boost::bind(std::equal_to< const basic_transceiver* >(), p,
                              boost::bind(&basic_transceiver::ptr::get, _1)));

    if (i == connections.end()) {
      connections.push_back(t);

      t->owner = this;
    }
  }

  /**
   * @param t the transceiver that identifies the connection to be severed
   *
   * \return a shared pointer to the transceiver that is removed
   **/
  basic_transceiver::ptr transporter::disassociate(basic_transceiver* t) {
    boost::recursive_mutex::scoped_lock l(lock);

    basic_transceiver::ptr p;

    connection_list::iterator i = std::find_if(connections.begin(), connections.end(),
                      boost::bind(std::equal_to< const basic_transceiver* >(), t,
                              boost::bind(&basic_transceiver::ptr::get, _1)));

    if (i != connections.end()) {
      p = *i;

      connections.erase(i);

      t->owner = 0;
    }

    return (p);
  }

  void transporter::disconnect() {
    boost::recursive_mutex::scoped_lock l(lock);

    for (connection_list::const_iterator i = connections.begin(); i != connections.end(); ++i) {
      (*i)->disconnect(*i);
    }
  }

  /**
   * @param n the number of the connection that is to be closed
   **/
  void transporter::disconnect(size_t n) {
    assert(n < connections.size());

    boost::recursive_mutex::scoped_lock l(lock);

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
    using namespace boost;

    boost::recursive_mutex::scoped_lock l(lock);

    const transporter* p = &m;

    connection_list::iterator i = std::find_if(connections.begin(), connections.end(),
                      bind(std::equal_to< const transporter* >(), p,
                                      bind(&basic_transceiver::get_owner,
                                                      bind(&basic_transceiver::ptr::get, _1))));

    if (i != connections.end()) {
      (*i)->disconnect(*i);
    }
  }

  /**
   * @param a an address
   * @param p a port
   **/
  void transporter::add_listener(const address& a, const long p) {
    basic_listener::ptr new_listener(new socket_listener(*this, a, p));

    listeners.push_back(new_listener);

    reinterpret_cast < socket_listener* > (new_listener.get())->activate(new_listener);
  }

  /**
   * @param n the number of the listener that is to be removed
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

  transporter::host transporter::get_local_host() {
    return (socket_transceiver::get_local_host());
  }
}

