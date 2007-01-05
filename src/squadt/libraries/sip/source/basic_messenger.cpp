#include <sip/detail/basic_messenger.tcc>
#include <sip/detail/common.h>

namespace sip {

  namespace messaging {

    template < >
    const std::string basic_messenger_impl< sip::message >::tag_open("<message>");

    template < >
    const std::string basic_messenger_impl< sip::message >::tag_close("</message>");

    template < >
    boost::shared_ptr < utility::logger > basic_messenger_impl< sip::message >::standard_logger(new utility::print_logger(std::clog));

    template < >
    basic_messenger< sip::message >::basic_messenger() : impl(new basic_messenger_impl< sip::message >) {
    }

    /**
     * \param[in] l pointer to a logger object
     **/
    template < >
    basic_messenger< sip::message >::basic_messenger(boost::shared_ptr < utility::logger > l) : impl(new basic_messenger_impl< sip::message >(l)) {
    }

    /**
     * \param[in] i pointer to an implementation object
     **/
    template < >
    basic_messenger< sip::message >::basic_messenger(basic_messenger_impl< sip::message >* i) : impl(i) {
    }
 
    template < >
    utility::logger* basic_messenger< sip::message >::get_logger() {
      return (impl->logger.get());
    }

    template < >
    utility::logger* basic_messenger< sip::message >::get_standard_logger() {
      return (basic_messenger_impl< sip::message >::standard_logger.get());
    }

    template < >
    void basic_messenger< sip::message >::set_standard_logger(boost::shared_ptr < utility::logger > l) {
      basic_messenger_impl< sip::message >::standard_logger = l;
    }

    template < >
    void basic_messenger< sip::message >::disconnect() {
      impl->disconnect();
    }

    /**
     * \param[in] m the message that is to be sent
     **/
    template < >
    void basic_messenger< sip::message >::send_message(const sip::message& m) {
      impl->send_message(m);
    }

    /**
     * \param[in] h the handler function that is to be executed
     * \param[in] t the message type on which delivery h is to be executed
     **/
    template < >
    void basic_messenger< sip::message >::add_handler(const sip::message::type_identifier_t t, handler_type h) {
      impl->add_handler(t, h);
    }

    /**
     * \param[in] t the message type for which to clear the event handler
     **/
    template < >
    void basic_messenger< sip::message >::clear_handlers(const sip::message::type_identifier_t t) {
      impl->clear_handlers(t);
    }

    /**
     * \param[in] t the message type for which to clear the event handler
     * \param[in] h the handler to remove
     **/
    template < >
    inline void basic_messenger< sip::message >::remove_handler(const sip::message::type_identifier_t t, handler_type h) {
      remove_handler(t, h);
    }

    /**
     * \param[in] t the type of the message
     **/
    template < >
    const boost::shared_ptr< sip::message > basic_messenger< sip::message >::await_message(sip::message::type_identifier_t t) {
      return (impl->await_message(t));
    }

    /**
     * \param[in] t the type of the message
     * \param[in] ts the maximum time to wait in seconds
     **/
    template < >
    const boost::shared_ptr< sip::message > basic_messenger< sip::message >::await_message(sip::message::type_identifier_t t, long const& ts) {
      return (impl->await_message(t, ts));
    }
  }
}
