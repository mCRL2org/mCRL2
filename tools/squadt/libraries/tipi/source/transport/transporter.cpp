// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <functional>

#include "boost/asio/ip/address.hpp"
#include "boost/bind.hpp"
#include "boost/ref.hpp"
#include "boost/foreach.hpp"
#include "boost/thread/recursive_mutex.hpp"

#include "tipi/detail/transport/detail/transporter.ipp"
#include "tipi/detail/transport/detail/transceiver.ipp"
#include "tipi/detail/transport/detail/socket_listener.hpp"
#include "tipi/detail/transport/detail/direct_transceiver.hpp"
#include "tipi/detail/transport/detail/socket_transceiver.hpp"

namespace transport {

  /**
   * \param[in] d the data to be sent
   **/
  void transporter_impl::send(const std::string& d) {
    boost::recursive_mutex::scoped_lock l(lock);

    for (connection_list::const_iterator i = connections.begin(); i != connections.end(); ++i) {
      (*i)->send(d);
    }
  }

  /**
   * \param[in] s stream that contains the data to be sent
   **/
  void transporter_impl::send(std::istream& s) {
    boost::recursive_mutex::scoped_lock l(lock);

    for (connection_list::const_iterator i = connections.begin(); i != connections.end(); ++i) {
      (*i)->send(s);
    }
  }

  /**
   * \param[in,out] t the transporter to relay the connection to
   * \param[in] c the transceiver that represents the local end point of the connection
   **/
  void transporter_impl::relay_connection(transporter* t, basic_transceiver* c) {
    if(!(t != 0)){
      throw  std::runtime_error("No transporter to relay a connection");
    }

    boost::recursive_mutex::scoped_lock l(lock);
    boost::recursive_mutex::scoped_lock tl(t->impl->lock);

    t->impl->associate(t->impl,c);
  }

  size_t transporter_impl::number_of_connections() const {
    return connections.size();
  }

  transporter_impl::~transporter_impl() {
    boost::recursive_mutex::scoped_lock l(lock);

    /* Shutdown listeners */
    for (listener_list::iterator i = listeners.begin(); i != listeners.end(); ++i) {
      (*i)->shutdown();
    }

    /* Disconnect all peers */
    disconnect();
  }

  /**
   * \param[in] t shared pointer for the local object
   * \param[in] p the peer to connect to
   **/
  void transporter_impl::connect(boost::shared_ptr < basic_transceiver > const& c, boost::shared_ptr < transporter_impl >& p) {
    boost::recursive_mutex::scoped_lock pl(p->lock);
    p->connections.push_back(c);

    boost::recursive_mutex::scoped_lock l(lock);
    connections.push_back(basic_transceiver::ptr(new direct_transceiver(p, boost::dynamic_pointer_cast< direct_transceiver > (c))));
  }

  /**
   * \param[in] a an address
   * \param[in] p a port
   **/
  void transporter_impl::connect(boost::shared_ptr < basic_transceiver > const& c, const boost::asio::ip::address& a, short int const& p) {
    boost::recursive_mutex::scoped_lock l(lock);

    boost::dynamic_pointer_cast < socket_transceiver > (c)->connect(a, p);

    connections.push_back(c);
  }

  /**
   * \param[in] h a hostname
   * \param[in] p a port
   **/
  void transporter_impl::connect(boost::shared_ptr < basic_transceiver > const& c, const std::string& h, short int const& p) {
    boost::recursive_mutex::scoped_lock l(lock);

    boost::dynamic_pointer_cast < socket_transceiver > (c)->connect(h, p);

    connections.push_back(c);
  }

  /**
   * \param[in] t the connection to associate with this transporter
   **/
  void transporter_impl::associate(boost::shared_ptr < transporter_impl > const& c, const basic_transceiver::ptr& t) {
    
    if(!(c.get() == this)){ 
      throw std::runtime_error( "Using incorrect transporter" );
    }

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
   * \param[in] t the transceiver that identifies the connection to be severed
   *
   * \return a shared pointer to the transceiver that is removed
   **/
  boost::shared_ptr< basic_transceiver > transporter_impl::disassociate(basic_transceiver* t) {
    boost::recursive_mutex::scoped_lock l(lock);

    if (t->owner.lock().get() != 0) {
      if(!(t->owner.lock().get() == this)){
        throw std::runtime_error( "Locking incorrect transceiver");
      };

      for (connection_list::iterator i = connections.begin(); i != connections.end(); ++i) {
        if (i->get() == t) {
          boost::shared_ptr< basic_transceiver > target(*i);

          connections.erase(i);

          t->owner.reset();

          on_disconnect(target.get());

          return target;
        }
      }
    }

    return boost::shared_ptr< basic_transceiver >();
  }

  /**
   * \param[in] t the transceiver that identifies the connection to be associated
   * \pre t->owner != 0 && t->owner != this
   **/
  void transporter_impl::associate(boost::shared_ptr < transporter_impl > const& c, basic_transceiver* t) {

    if(!(c.get() == this)){ 
      throw std::runtime_error( "Using incorrect transporter" );
    }

    boost::shared_ptr < transporter_impl > cc(t->owner.lock());

    if(!(c.get() != 0)){ 
      throw std::runtime_error( "Using incorrect transporter, value equals 0" );
    }


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
   * \param[in] t the local endpoint (transceiver) to disconnect
   * \return whether a connection was broken (otherwise it did not exist)
   **/
  bool transporter_impl::disconnect(basic_transceiver const& t) {
    for (connection_list::iterator i = connections.begin(); i != connections.end(); ++i) {
      if ((*i).get() == &t) {
        (*i)->owner.reset();
        (*i)->disconnect(*i);

        connections.erase(i);

        return true;
      }
    }

    return false;
  }

  /**
   * \param[in] m the directly connected peer
   * \return whether a connection was broken (otherwise it did not exist)
   **/
  bool transporter_impl::disconnect(transporter_impl const* m) {
    using namespace boost;

    boost::recursive_mutex::scoped_lock l(lock);

    for (connection_list::iterator i = connections.begin(); i != connections.end(); ++i) {
      if ((*i)->get_owner() == m) {
        (*i)->owner.reset();
        (*i)->disconnect(*i);

        connections.erase(i);

        return true;
      }
    }

    return false;
  }

  /**
   * \param[in] a an address
   * \param[in] p a port
   **/
  void transporter_impl::add_listener(boost::shared_ptr < transporter_impl > const& c, const boost::asio::ip::address& a, short int const& p) {
    basic_listener::ptr new_listener(new socket_listener(c, a, p));

    listeners.push_back(new_listener);

    reinterpret_cast < socket_listener* > (new_listener.get())->activate(new_listener);
  }

  /**
   * \param[in] a an address
   * \param[in] p a port
   **/
  void transporter_impl::add_listener(boost::shared_ptr < transporter_impl > const& c, std::string const& a, short int const& p) {
    basic_listener::ptr new_listener(new socket_listener(c, boost::asio::ip::address_v4::from_string(a), p));

    listeners.push_back(new_listener);

    reinterpret_cast < socket_listener* > (new_listener.get())->activate(new_listener);
  }

  /**
   * \param[in] n the number of the listener that is to be removed
   **/
  void transporter_impl::remove_listener(size_t n) {
    if(!(n < listeners.size())){ 
      throw std::runtime_error( "Trying to remove more listeners than are available\n" );
    }

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
   * \param[in] p the peer to connect to
   **/
  void transporter::connect(transporter& p) {
    boost::shared_ptr < basic_transceiver > c(new direct_transceiver(impl));

    impl->connect(c,p.impl);
  }

  /**
   * \param[in] a a hostname or IP address
   * \param[in] p a port
   **/
  void transporter::connect(std::string const& a, short int const& p) {
    basic_transceiver::ptr c(socket_transceiver::create(impl));

    try {
      if (a.empty()) {
        impl->connect(c,boost::asio::ip::address_v4::loopback(),p);
      }
      else {
        impl->connect(c,boost::asio::ip::address::from_string(a),p);
      }
    }
    catch (...) {
      impl->connect(c,a,p);
    }
  }

  void transporter::disconnect() {
    impl->disconnect();
  }

  /**
   * \param[in] t the local endpoint to disconnect from
   **/
  bool transporter::disconnect(basic_transceiver const& t) {
    return impl->disconnect(t);
  }

  /**
   * \param[in] m the local transporter to disconnect from
   **/
  bool transporter::disconnect(transporter& m) {
    return impl->disconnect(m.impl.get());
  }

  /**
   * \param[in] a a hostname or IP address
   * \param[in] p a port
   **/
  void transporter::add_listener(std::string const& a, short int const& p) {
    try {
      if (a.empty()) {
        impl->add_listener(impl, boost::asio::ip::address_v4::any(), p);
      }
      else {
        impl->add_listener(impl, boost::asio::ip::address::from_string(a), p);
      }
    }
    catch (...) {
      if (a.empty()) {
        impl->add_listener(impl, boost::asio::ip::address_v4::loopback(), p);
      }
      else {
        impl->add_listener(impl, a, p);
      }
    }
  }

  /**
   * \param[in] n the number of the listener that is to be removed
   **/
  void transporter::remove_listener(size_t n) {
    impl->remove_listener(n);
  }

  /**
   * \param[in] d the data to be sent
   **/
  void transporter::send(const std::string& d) {
    impl->send(d);
  }

  /**
   * \param[in] s stream that contains the data to be sent
   **/
  void transporter::send(std::istream& s) {
    impl->send(s);
  }

  /**
   * \param[in,out] t the transporter to relay the connection to
   * \param[in] c the transceiver that represents the local end point of the connection
   **/
  void transporter::relay_connection(transporter* t, basic_transceiver* c) {
    impl->relay_connection(t,c);
  }

  std::string transporter::get_local_host() {
    return (socket_transceiver::get_local_host());
  }
}

