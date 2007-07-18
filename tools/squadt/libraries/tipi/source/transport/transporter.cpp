//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/transport/transporter.cpp

#include <algorithm>
#include <functional>

#include <boost/asio/ip/address.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/foreach.hpp>

#define IMPORT_STATIC_DEFINITIONS
#include "transport/detail/basics.hpp"
#include "transport/detail/transporter.tcc"
#include "transport/detail/transceiver.tcc"
#include "transport/detail/socket_listener.hpp"
#include "transport/detail/direct_transceiver.hpp"
#include "transport/detail/socket_transceiver.hpp"

namespace transport {

  /**
   * @param d the data to be sent
   **/
  void transporter_impl::send(const std::string& d) {
    boost::recursive_mutex::scoped_lock l(lock);

    for (connection_list::const_iterator i = connections.begin(); i != connections.end(); ++i) {
      (*i)->send(d);
    }
  }

  /**
   * @param s stream that contains the data to be sent
   **/
  void transporter_impl::send(std::istream& s) {
    boost::recursive_mutex::scoped_lock l(lock);

    for (connection_list::const_iterator i = connections.begin(); i != connections.end(); ++i) {
      (*i)->send(s);
    }
  }

  /**
   * @param[in,out] t the transporter to relay the connection to
   * @param[in] c the transceiver that represents the local end point of the connection
   **/
  void transporter_impl::relay_connection(transporter* t, basic_transceiver* c) {
    assert(t != 0);

    boost::recursive_mutex::scoped_lock l(lock);
    boost::recursive_mutex::scoped_lock tl(t->impl->lock);

    t->impl->associate(t->impl,c);
  }

  transporter_impl::~transporter_impl() {
    using namespace boost;

    boost::recursive_mutex::scoped_lock l(lock);

    /* Disconnect all peers */
    disconnect();
  
    /* Clean up listeners */
    for (listener_list::iterator i = listeners.begin(); i != listeners.end(); ++i) {
      (*i)->shutdown();
    }
  }

  /**
   * \param t shared pointer for the local object
   * \param p the peer to connect to
   **/
  void transporter_impl::connect(boost::shared_ptr < basic_transceiver > const& c, boost::shared_ptr < transporter_impl >& p) {
    boost::recursive_mutex::scoped_lock pl(p->lock);
    p->connections.push_back(c);

    boost::recursive_mutex::scoped_lock l(lock);
    connections.push_back(basic_transceiver::ptr(new direct_transceiver(p, boost::dynamic_pointer_cast< direct_transceiver > (c))));
  }

  /**
   * \param a an address
   * \param p a port
   **/
  void transporter_impl::connect(boost::shared_ptr < basic_transceiver > const& c, const ip_address_t& a, port_t const& p) {
    boost::recursive_mutex::scoped_lock l(lock);

    boost::dynamic_pointer_cast < socket_transceiver > (c)->connect(a, p);

    connections.push_back(c);
  }

  /**
   * \param h a hostname
   * \param p a port
   **/
  void transporter_impl::connect(boost::shared_ptr < basic_transceiver > const& c, const std::string& h, port_t const& p) {
    boost::recursive_mutex::scoped_lock l(lock);

    boost::dynamic_pointer_cast < socket_transceiver > (c)->connect(h, p);

    connections.push_back(c);
  }

  /**
   * \param t the connection to associate with this transporter
   **/
  void transporter_impl::associate(boost::shared_ptr < transporter_impl > const& c, const basic_transceiver::ptr& t) {
    assert(c.get() == this);

    boost::recursive_mutex::scoped_lock l(lock);

    const basic_transceiver* p = t.get();

    connection_list::iterator i = std::find_if(connections.begin(), connections.end(),
                      boost::bind(std::equal_to< const basic_transceiver* >(), p,
                              boost::bind(&basic_transceiver::ptr::get, _1)));

    if (i == connections.end()) {
      connections.push_back(t);

      t->owner = c;
    }
  }

  /**
   * \param t the transceiver that identifies the connection to be severed
   *
   * \return a shared pointer to the transceiver that is removed
   **/
  basic_transceiver::ptr transporter_impl::disassociate(basic_transceiver* t) {
    basic_transceiver::ptr p;

    assert(t->owner.lock().get() == this);

    boost::recursive_mutex::scoped_lock l(lock);

    for (connection_list::iterator i = connections.begin(); i != connections.end(); ++i) {
      if (i->get() == t) {
        p = *i;
       
        connections.erase(i);
       
        t->owner.reset();

        break;
      }
    }

    return (p);
  }

  /**
   * \param t the transceiver that identifies the connection to be associated
   * \pre t->owner != 0 && t->owner != this
   **/
  void transporter_impl::associate(boost::shared_ptr < transporter_impl > const& c, basic_transceiver* t) {
    assert(c.get() == this);

    boost::shared_ptr < transporter_impl > cc(t->owner.lock());

    assert(cc.get() != 0);

    if (t->owner.lock().get() != this) {
      boost::recursive_mutex::scoped_lock l(lock);
      boost::recursive_mutex::scoped_lock ll(cc->lock);

      for (connection_list::iterator i = cc->connections.begin(); i != cc->connections.end(); ++i) {
        if (t == (*i).get()) {
          connections.push_back(*i);

          cc->connections.erase(i);

          break;
        }
      }

      t->owner = c;
    }
  }

  void transporter_impl::disconnect() {
    boost::recursive_mutex::scoped_lock l(lock);

    for (connection_list::const_iterator i = connections.begin(); i != connections.end(); ++i) {
      basic_transceiver* p = (*i).get();

      p->owner.reset();

      p->disconnect(*i);
    }

    connections.clear();
  }

  /**
   * \param n the number of the connection that is to be closed
   **/
  void transporter_impl::disconnect(size_t n) {
    assert(n < connections.size());

    boost::recursive_mutex::scoped_lock l(lock);

    connection_list::iterator i = connections.begin();
  
    while (0 < n) {
      assert(i != connections.end());

      --n;

      ++i;
    }

    (*i)->owner.reset();
    (*i)->disconnect(*i);

    connections.erase(i);
  }

  /**
   * \param m the directly connected peer
   **/
  void transporter_impl::disconnect(transporter_impl const* m) {
    using namespace boost;

    boost::recursive_mutex::scoped_lock l(lock);

    for (connection_list::iterator i = connections.begin(); i != connections.end(); ++i) {
      if ((*i)->get_owner() == m) {
        (*i)->owner.reset();
        (*i)->disconnect(*i);

        connections.erase(i);

        break;
      }
    }
  }

  /**
   * \param a an address
   * \param p a port
   **/
  void transporter_impl::add_listener(boost::shared_ptr < transporter_impl > const& c, const ip_address_t& a, port_t const& p) {
    basic_listener::ptr new_listener(new socket_listener(c, a, p));

    listeners.push_back(new_listener);

    reinterpret_cast < socket_listener* > (new_listener.get())->activate(new_listener);
  }

  /**
   * \param a an address
   * \param p a port
   **/
  void transporter_impl::add_listener(boost::shared_ptr < transporter_impl > const& c, const host_name_t& a, port_t const& p) {
    basic_listener::ptr new_listener(new socket_listener(c, boost::asio::ip::address_v4::from_string(a), p));

    listeners.push_back(new_listener);

    reinterpret_cast < socket_listener* > (new_listener.get())->activate(new_listener);
  }

  /**
   * \param n the number of the listener that is to be removed
   **/
  void transporter_impl::remove_listener(size_t n) {
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

  transporter::transporter(boost::shared_ptr < transporter_impl > const& c) : impl(c) {
  }
 
  size_t transporter::number_of_listeners() const {
    return (impl->listeners.size());
  }

  size_t transporter::number_of_connections() const {
    boost::recursive_mutex::scoped_lock l(impl->lock);

    return (impl->connections.size());
  }

  /**
   * \param p the peer to connect to
   **/
  void transporter::connect(transporter& p) {
    boost::shared_ptr < basic_transceiver > c(new direct_transceiver(impl));

    impl->connect(c,p.impl);
  }

  /**
   * \param a an address
   * \param p a port
   **/
  void transporter::connect(const ip_address_t& a, port_t const& p) {
    basic_transceiver::ptr c(socket_transceiver::create(impl));

    impl->connect(c, a,p);
  }

  /**
   * \param h a hostname
   * \param p a port
   **/
  void transporter::connect(const std::string& h, port_t const& p) {
    basic_transceiver::ptr c(socket_transceiver::create(impl));

    impl->connect(c,h,p);
  }

  void transporter::disconnect() {
    impl->disconnect();
  }

  void transporter::disconnect(size_t n) {
    impl->disconnect(n);
  }

  void transporter::disconnect(transporter& m) {
    impl->disconnect(m.impl.get());
  }

  /**
   * \param a an address
   * \param p a port
   **/
  void transporter::add_listener(const ip_address_t& a, port_t const& p) {
    impl->add_listener(impl, a, p);
  }

  /**
   * \param a an address
   * \param p a port
   **/
  void transporter::add_listener(const host_name_t& a, port_t const& p) {
    impl->add_listener(impl, a,p);
  }

  /**
   * \param n the number of the listener that is to be removed
   **/
  void transporter::remove_listener(size_t n) {
    impl->remove_listener(n);
  }

  /**
   * @param d the data to be sent
   **/
  void transporter::send(const std::string& d) {
    impl->send(d);
  }

  /**
   * @param s stream that contains the data to be sent
   **/
  void transporter::send(std::istream& s) {
    impl->send(s);
  }

  /**
   * @param[in,out] t the transporter to relay the connection to
   * @param[in] c the transceiver that represents the local end point of the connection
   **/
  void transporter::relay_connection(transporter* t, basic_transceiver* c) {
    impl->relay_connection(t,c);
  }

  host_name_t transporter::get_local_host() {
    return (socket_transceiver::get_local_host());
  }
}

