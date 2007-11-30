//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/basic_messenger.cpp

#include <tipi/detail/basic_messenger.ipp>
#include "tipi/common.hpp"

namespace tipi {

  namespace messaging {

    /** Standard logger that uses std::clog */
    template < >
    boost::shared_ptr < utility::logger > basic_messenger_impl< tipi::message >::standard_logger(new utility::print_logger(std::clog));

    /** Default constructor */
    template < >
    basic_messenger< tipi::message >::basic_messenger() :
        transport::transporter(boost::shared_ptr < transport::transporter_impl > (
                new basic_messenger_impl< tipi::message >(basic_messenger_impl< tipi::message >::standard_logger))) {
    }

    /**
     * \param[in] l pointer to a logger object
     **/
    template < >
    basic_messenger< tipi::message >::basic_messenger(boost::shared_ptr < utility::logger > l) :
        transport::transporter(boost::shared_ptr < transport::transporter_impl > (new basic_messenger_impl< tipi::message >(l))) {
    }

    /**
     * \param[in] c pointer to an implementation object
     **/
    template < >
    basic_messenger< tipi::message >::basic_messenger(boost::shared_ptr < basic_messenger_impl< tipi::message > > const& c) : transport::transporter(c) {
    }
 
    /** Get the current logger */
    template < >
    utility::logger* basic_messenger< tipi::message >::get_logger() {
      return (boost::static_pointer_cast < basic_messenger_impl< tipi::message > > (impl)->logger.get());
    }

    /** Returns the standard logger */
    template < >
    utility::logger* basic_messenger< tipi::message >::get_standard_logger() {
      return (basic_messenger_impl< tipi::message >::standard_logger.get());
    }

    /**
     * Sets the standard logger
     * \param[in] l the logger that is to be used by the messenger
     **/
    template < >
    void basic_messenger< tipi::message >::set_standard_logger(boost::shared_ptr < utility::logger > l) {
      basic_messenger_impl< tipi::message >::standard_logger = l;
    }

    /** Disconnects from all peers */
    template < >
    void basic_messenger< tipi::message >::disconnect() {
      boost::static_pointer_cast < basic_messenger_impl< tipi::message > > (impl)->disconnect();
    }

    /**
     * \param[in] m the message that is to be sent
     **/
    template < >
    void basic_messenger< tipi::message >::send_message(const tipi::message& m) {
      boost::static_pointer_cast < basic_messenger_impl< tipi::message > > (impl)->send_message(m);
    }

    /**
     * \param[in] h the handler function that is to be executed
     * \param[in] t the message type on which delivery h is to be executed
     **/
    template < >
    void basic_messenger< tipi::message >::add_handler(const tipi::message::type_identifier_t t, handler_type h) {
      boost::static_pointer_cast < basic_messenger_impl< tipi::message > > (impl)->add_handler(t, h);
    }

    /**
     * \param[in] t the message type for which to clear the event handler
     **/
    template < >
    void basic_messenger< tipi::message >::clear_handlers(const tipi::message::type_identifier_t t) {
      boost::static_pointer_cast < basic_messenger_impl< tipi::message > > (impl)->clear_handlers(t);
    }

    /**
     * \param[in] t the message type for which to clear the event handler
     * \param[in] h the handler to remove
     **/
    template < >
    inline void basic_messenger< tipi::message >::remove_handler(const tipi::message::type_identifier_t t, handler_type h) {
      remove_handler(t, h);
    }

    /**
     * \param[in] t the type of the message
     **/
    template < >
    const boost::shared_ptr< tipi::message > basic_messenger< tipi::message >::await_message(tipi::message::type_identifier_t t) {
      return boost::static_pointer_cast < basic_messenger_impl< tipi::message > > (impl)->await_message(t);
    }

    /**
     * \param[in] t the type of the message
     * \param[in] ts the maximum time to wait in seconds
     **/
    template < >
    const boost::shared_ptr< tipi::message > basic_messenger< tipi::message >::await_message(tipi::message::type_identifier_t t, long const& ts) {
      return boost::static_pointer_cast < basic_messenger_impl< tipi::message > > (impl)->await_message(t, ts);
    }
  }
}
