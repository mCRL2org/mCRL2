#ifndef SIP_CONTROLLER_TCC_
#define SIP_CONTROLLER_TCC_

#include <sip/controller.h>
#include <sip/configuration.h>
#include <sip/controller/capabilities.h>
#include <sip/detail/basic_messenger.tcc>
#include <sip/detail/common.h>
#include <sip/visitors.h>

namespace sip {
  namespace controller {

    class communicator_impl : public sip::messaging::basic_messenger_impl< sip:: message > {
      friend class communicator;
      
      private:

        /** \brief call-back function type for layout changes of the display */
        typedef communicator::display_layout_handler_function display_layout_handler_function;

        /** \brief call-back function type for updates to the display */
        typedef communicator::display_update_handler_function display_update_handler_function;

        /** \brief call-back function type for diagnostic messages */
        typedef communicator::status_message_handler_function status_message_handler_function;

      private:

        /** \brief Handler function to replace the current display layout with a new one */
        void display_layout_handler(messenger::message_ptr const&, display_layout_handler_function);
 
        /** \brief Handler function to replace the current display layout with a new one */
        void display_update_handler(messenger::message_ptr const&, sip::layout::tool_display::sptr, display_update_handler_function);

        /** \brief Handler function to replace the current display layout with a new one */
        void status_message_handler(messenger::message_ptr const&, status_message_handler_function);

        /** \brief Send details about the amount of space that the controller currently has reserved for this tool */
        void request_controller_capabilities_handler();
 
        /** \brief Event handler for storing configurations */
        void tool_configuration_handler(messenger::message_ptr const& m);

        /** \brief Creates a new configuration object */
        static boost::shared_ptr < configuration > new_configuration(sip::tool::capabilities::input_combination const& c);

      private:

        /** \brief The current configuration of a tool (may be limited to a main input configuration) */
        boost::shared_ptr < configuration >  m_configuration;

        /** \brief The current handler for layout change events */
        display_layout_handler_function      m_layout_handler;

        /** \brief The current handler for layout state change events */
        display_update_handler_function      m_data_handler;
 
      public:

        /** \brief Default constructor */
        communicator_impl();

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_display_layout_handler(display_layout_handler_function);

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_display_update_handler(sip::layout::tool_display::sptr, display_update_handler_function);

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_status_message_handler(status_message_handler_function);
    };

    inline communicator_impl::communicator_impl() {
      using namespace boost;

      /* set default handlers for delivery events */
      add_handler(sip::message_request_controller_capabilities, bind(&communicator_impl::request_controller_capabilities_handler, this));
      add_handler(sip::message_accept_configuration, bind(&communicator_impl::tool_configuration_handler, this, _1));
    }
    
    /**
     * \param h the function that is called when a new layout for the display has been received
     **/
    inline void communicator_impl::activate_display_layout_handler(display_layout_handler_function h) {
      /* Remove any previous handlers */
      clear_handlers(sip::message_display_layout);

      add_handler(sip::message_display_layout, boost::bind(&communicator_impl::display_layout_handler, this, _1, h));

      m_layout_handler = h;
    }

    /**
     * \param d pointer to a tool display
     * \param h the function that is called when a new layout for the display has been received
     *
     * \pre d.get() != 0
     **/
    inline void communicator_impl::activate_display_update_handler(sip::layout::tool_display::sptr d, display_update_handler_function h) {
      /* Remove any previous handlers */
      clear_handlers(sip::message_display_update);

      add_handler(sip::message_display_update, boost::bind(&communicator_impl::display_update_handler, this, _1, d, h));

      m_data_handler = h;
    }

    /**
     * \param h the function that is called when a new layout for the display has been received
     **/
    inline void communicator_impl::activate_status_message_handler(status_message_handler_function h) {
      /* Remove any previous handlers */
      clear_handlers(sip::message_report);

      add_handler(sip::message_report, boost::bind(&communicator_impl::status_message_handler, this, _1, h));
    }

    /**
     * \param m pointer to the message
     * \param h the function that is called when a new layout for the display has been received
     **/
    inline void communicator_impl::display_layout_handler(messenger::message_ptr const& m, display_layout_handler_function h) {
      sip::layout::tool_display::sptr d(new layout::tool_display);

      visitors::restore(*d, m->to_string());

      h(d);
    }

    /**
     * \param m pointer to the message
     * \param h the function that is called when data for the display has been received
     * \param d a shared pointer to a tool display
     **/
    inline void communicator_impl::display_update_handler(const messenger::message_ptr& m, sip::layout::tool_display::sptr d, display_update_handler_function h) {
      if (d.get() != 0) {
        std::vector < sip::layout::element const* > elements;

        d->update(m->to_string(), elements);

        h(elements);
      }
    }

    /**
     * \param m pointer to the message
     * \param h the function that is called when a new report has been received
     **/
    inline void communicator_impl::status_message_handler(const messenger::message_ptr& m, status_message_handler_function h) {
      boost::shared_ptr < sip::report > r(new sip::report);

      sip::visitors::restore(*r, m->to_string());

      h(r);
    }

    /* Reply details about the amount of reserved display space */
    inline void communicator_impl::request_controller_capabilities_handler() {
      message m(visitors::store(communicator::m_controller_capabilities), sip::message_response_controller_capabilities);

      send_message(m);
    }
 
    /**
     * \param[in] m a reference to the message
     **/
    inline void communicator_impl::tool_configuration_handler(const messenger::message_ptr& m) {
      m_configuration.reset(new configuration);

      visitors::restore(*m_configuration, m->to_string());
    }

    /**
     * \param[in] c the input combination on which to base the new configuration
     **/
    inline boost::shared_ptr < configuration > communicator_impl::new_configuration(sip::tool::capabilities::input_combination const& c) {
      boost::shared_ptr < configuration > nc(new sip::configuration(c.m_category));

      return (nc);
    }
  }
}
 

#endif
