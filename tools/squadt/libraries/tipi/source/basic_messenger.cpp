//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/basic_messenger.cpp

#include <tipi/detail/basic_messenger.tcc>
#include <tipi/common.h>

namespace tipi {

  namespace messaging {

    template < >
    const std::string basic_messenger_impl< tipi::message >::tag_open("<message>");

    template < >
    const std::string basic_messenger_impl< tipi::message >::tag_close("</message>");

    template < >
    boost::shared_ptr < utility::logger > basic_messenger_impl< tipi::message >::standard_logger(new utility::print_logger(std::clog));

    /** Default constructor */
    template < >
    basic_messenger< tipi::message >::basic_messenger() : impl(new basic_messenger_impl< tipi::message >) {
    }

    /**
     * \param[in] l pointer to a logger object
     **/
    template < >
    basic_messenger< tipi::message >::basic_messenger(boost::shared_ptr < utility::logger > l) : impl(new basic_messenger_impl< tipi::message >(l)) {
    }

    /**
     * \param[in] i pointer to an implementation object
     **/
    template < >
    basic_messenger< tipi::message >::basic_messenger(basic_messenger_impl< tipi::message >* i) : impl(i) {
    }
 
    /** Get the current logger */
    template < >
    utility::logger* basic_messenger< tipi::message >::get_logger() {
      return (impl->logger.get());
    }

    /** Returns the standard logger */
    template < >
    utility::logger* basic_messenger< tipi::message >::get_standard_logger() {
      return (basic_messenger_impl< tipi::message >::standard_logger.get());
    }

    /** Sets the standard logger */
    template < >
    void basic_messenger< tipi::message >::set_standard_logger(boost::shared_ptr < utility::logger > l) {
      basic_messenger_impl< tipi::message >::standard_logger = l;
    }

    /** Disconnects from all peers */
    template < >
    void basic_messenger< tipi::message >::disconnect() {
      impl->disconnect();
    }

    /**
     * \param[in] m the message that is to be sent
     **/
    template < >
    void basic_messenger< tipi::message >::send_message(const tipi::message& m) {
      impl->send_message(m);
    }

    /**
     * \param[in] h the handler function that is to be executed
     * \param[in] t the message type on which delivery h is to be executed
     **/
    template < >
    void basic_messenger< tipi::message >::add_handler(const tipi::message::type_identifier_t t, handler_type h) {
      impl->add_handler(t, h);
    }

    /**
     * \param[in] t the message type for which to clear the event handler
     **/
    template < >
    void basic_messenger< tipi::message >::clear_handlers(const tipi::message::type_identifier_t t) {
      impl->clear_handlers(t);
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
      return (impl->await_message(t));
    }

    /**
     * \param[in] t the type of the message
     * \param[in] ts the maximum time to wait in seconds
     **/
    template < >
    const boost::shared_ptr< tipi::message > basic_messenger< tipi::message >::await_message(tipi::message::type_identifier_t t, long const& ts) {
      return (impl->await_message(t, ts));
    }
  }
}
